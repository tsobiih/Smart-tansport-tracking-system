#include "driver_manager.h"
#include "json_helper.h" /* must provide C linkage */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* If you want automatic queue operations, compile with -DUSE_QUEUE and
   provide queue_manager.h with the expected APIs. */
#ifdef USE_QUEUE
#include "queue_manager.h"
#endif

/* Global head (declared extern in the header) */
DriverNode* all_drivers_head = NULL;

/* Helper: create node */
static DriverNode* create_driver_node(const Driver* d)
{
    DriverNode* node = (DriverNode*)malloc(sizeof(DriverNode));
    if (!node) return NULL;
    node->next = NULL;
    node->data = *d; /* struct copy */
    return node;
}

/* Map dest id to human name */
const char* dest_id_to_dist_name(const int id)
{
    switch (id) {
        case CAIRO_SHOUBRA:    return "Cairo - Shoubra";
        case CAIRO_METRO:      return "Cairo - Metro";
        case CAIRO_RAMSES:     return "Cairo - Ramses";
        case ALEX:             return "Alexandria";
        case MENOUFIA_SHEPIN:  return "Menoufia - Shepin";
        case MENOUFIA_QWESNA:  return "Menoufia - Quesna";
        case SHARQIA_ZAQAZIQ:  return "Sharqia - Zaqaziq";
        case ELMANSOURA:       return "El-Mansoura";
        case TANTA:            return "Tanta";
        default:               return "Unknown Destination";
    }
}

/* Free list and reload from JSON */
void init_drivers_list(void)
{
    DriverNode* cur = all_drivers_head;
    while (cur) {
        DriverNode* next = cur->next;
        free(cur);
        cur = next;
    }
    all_drivers_head = NULL;

    /* Load persisted drivers if file exists */
    load_drivers_from_json();
}

/* Add driver to in-memory list (no persistence here) */
int add_driver_to_list(Driver d)
{
    /* check duplicates (bus_id and mobile) */
    DriverNode* it = all_drivers_head;
    while (it) {
        if (strncmp(it->data.bus_id, d.bus_id, sizeof(it->data.bus_id)) == 0) return 0;
        if (strncmp(it->data.mobile, d.mobile, sizeof(it->data.mobile)) == 0) return 0;
        it = it->next;
    }

    DriverNode* node = create_driver_node(&d);
    if (!node) return 0;
    node->next = all_drivers_head;
    all_drivers_head = node;
    return 1;
}

/* Find by mobile */
Driver* find_driver_by_mobile(const char* mobile)
{
    if (!mobile) return NULL;
    DriverNode* cur = all_drivers_head;
    while (cur) {
        if (strncmp(cur->data.mobile, mobile, sizeof(cur->data.mobile)) == 0) return &cur->data;
        cur = cur->next;
    }
    return NULL;
}

/* Find by bus_id */
Driver* find_driver_by_id(const char* bus_id)
{
    if (!bus_id) return NULL;
    DriverNode* cur = all_drivers_head;
    while (cur) {
        if (strncmp(cur->data.bus_id, bus_id, sizeof(cur->data.bus_id)) == 0) return &cur->data;
        cur = cur->next;
    }
    return NULL;
}

/* Debug print */
void print_all_drivers(void)
{
    DriverNode* cur = all_drivers_head;
    printf("=== Drivers List ===\n");
    while (cur) {
        printf("Name   : %s\n", cur->data.name);
        printf("Mobile : %s\n", cur->data.mobile);
        printf("Bus ID : %s\n", cur->data.bus_id);
        printf("Dest   : %d (%s)\n", (int)cur->data.dest, dest_id_to_dist_name((int)cur->data.dest));
        printf("Status : %d\n", (int)cur->data.status);
        printf("---------------------\n");
        cur = cur->next;
    }
}

/* Register driver and persist to drivers.json
   gen_id_buffer must have space >= 20 bytes.
*/
bool register_driver(const char* name, const char* mobile, Destination dest, char* gen_id_buffer, const char* pass)
{
    if (!name || !mobile || !gen_id_buffer || !pass) return false;

    if (find_driver_by_mobile(mobile) != NULL) return false;

    static unsigned int id_counter = 0;
    if (id_counter == 0) {
        id_counter = (unsigned int)(time(NULL) % 1000000) + 1;
    }

    bool unique = false;
    for (int tries = 0; tries < 10000 && !unique; ++tries) {
        ++id_counter;
        snprintf(gen_id_buffer, 20, "DRV%06u", id_counter);
        if (find_driver_by_id(gen_id_buffer) == NULL) unique = true;
    }
    if (!unique) return false;

    Driver d;
    memset(&d, 0, sizeof(d));
    strncpy(d.name, name, sizeof(d.name) - 1);
    d.name[sizeof(d.name) - 1] = '\0';

    strncpy(d.mobile, mobile, sizeof(d.mobile) - 1);
    d.mobile[sizeof(d.mobile) - 1] = '\0';

    strncpy(d.password, pass, sizeof(d.password) - 1);
    d.password[sizeof(d.password) - 1] = '\0';

    strncpy(d.bus_id, gen_id_buffer, sizeof(d.bus_id) - 1);
    d.bus_id[sizeof(d.bus_id) - 1] = '\0';

    d.dest = dest;
    d.status = STATUS_NOT_ACTIVE;

    if (!add_driver_to_list(d)) return false;

    /* Persist to JSON */
    save_drivers_to_json();
    return true;
}

/* Change status and persist */
bool change_driver_status(const char* bus_id, DriverStatus new_state)
{
    Driver* d = find_driver_by_id(bus_id);
    if (!d) return false;
    d->status = new_state;

#ifdef USE_QUEUE
    if (new_state == STATUS_READY) {
        /* Example: enqueue by bus_id — adjust to your queue_manager API */
        enqueue_driver_by_bus_id(d->bus_id);
        save_queues_to_json();
    } else if (new_state == STATUS_ON_TRIP) {
        remove_driver_from_queue_by_bus_id(d->bus_id);
        save_queues_to_json();
    }
#endif

    save_drivers_to_json();
    return true;
}

/* Start trip: mark ON_TRIP, remove from queue if applicable, persist */
bool start_trip(const char* bus_id)
{
    Driver* d = find_driver_by_id(bus_id);
    if (!d) return false;
    d->status = STATUS_ON_TRIP;

#ifdef USE_QUEUE
    remove_driver_from_queue_by_bus_id(bus_id);
    save_queues_to_json();
#endif

    save_drivers_to_json();
    return true;
}

/* Login: validate credentials (plaintext compare).
   If valid and status is READY you may want to ensure the driver is enqueued (optional).
*/
bool diver_login(const char* mobile, const char* pass)
{
    if (!mobile || !pass) return false;
    Driver* d = find_driver_by_mobile(mobile);
    if (!d) return false;
    if (strncmp(d->password, pass, sizeof(d->password)) != 0) return false;

#ifdef USE_QUEUE
    if (d->status == STATUS_READY) {
        enqueue_driver_by_bus_id(d->bus_id);
        save_queues_to_json();
    }
#endif

    return true;
}
