#ifndef DRIVER_MANAGER_H
#define DRIVER_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// Destinations
typedef enum{
    CAIRO_SHOUBRA,
    CAIRO_METRO,    // كلية الزراعه
    CAIRO_RAMSES,
    ALEX,
    MENOUFIA_SHEPIN,
    MENOUFIA_QWESNA,
    SHARQIA_ZAQAZIQ,
    ELMANSOURA,
    TANTA,

    DEST_COUNT
}Destination;

// Driver Status
typedef enum {
    STATUS_NOT_ACTIVE = 0,
    STATUS_READY,     // in the queue
    STATUS_ON_TRIP    // in a trip
} DriverStatus;

// Driver Data
typedef struct {
    char name[50];
    char mobile[15];
    Destination dest; // "Cairo", "Alex", "Menoufia"
    char bus_id[20];      // BEN-CAI-001
    DriverStatus status;
    char password[20];
} Driver;

//Driver Linked List
typedef struct DriverNode {
    Driver data;
    struct DriverNode* next;
}DriverNode;

// To be accessed to any header
extern DriverNode* all_drivers_head;

// Driver APIs
void init_drivers_list();
int add_driver_to_list(Driver d);
Driver* find_driver_by_mobile(const char* mobile);
Driver* find_driver_by_id(const char* bus_id);
void print_all_drivers();
char* dest_id_to_dist_name(const int id);
bool register_driver(const char* name, const char* mobile, Destination dest, const char* gen_id_buffer, const char* pass);
bool change_driver_status(const char* bus_id, DriverStatus new_state);
bool start_trip(const char* bus_id);
bool diver_login(const char* mobile, const char* pass);
char* gen_bus_new_id(const Destination dest);


#ifdef __cplusplus
}
#endif

#endif // DRIVER_MANAGER_H
