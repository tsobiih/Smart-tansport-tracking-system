#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "driver_manager.h"
#include "json_helper.h"
#include "queue_manager.h"

DriverNode* all_drivers_head = NULL;
static uint32_t bus_counter;

void init_drivers_list()
{
  all_drivers_head = NULL;
  char* _bus_id = load_drivers_from_json();

  // BNH-103
  char* start = strchr(_bus_id,'-')+1;
  uint8_t len = strlen(start);

  char number[10];
  strncpy(number, start, len);
  number[len] = '\0';

  bus_counter = atoi(number);
}

int add_driver_to_list(Driver d)
{
  DriverNode* newNode =(DriverNode*) malloc(sizeof(DriverNode));
  if(newNode == NULL) return 0;

  newNode->data = d;
  newNode->next = all_drivers_head;
  all_drivers_head = newNode;
  return 1;
}

Driver* find_driver_by_mobile(const char* mobile)
{
  DriverNode* driver = all_drivers_head;

  while (driver->next != NULL)
  {
    if(strcmp(driver->data.mobile, mobile) == 0) return &driver->data;
  }
  printf("There is no any drivers linked with this number.");
  return NULL;
}

Driver* find_driver_by_id(const char* bus_id)
{
  DriverNode* driver = all_drivers_head;

  while (driver->next != NULL)
  {
    if(strcmp(driver->data.bus_id, bus_id) == 0) return &driver->data;
  }
  printf("There is no any drivers linked with this bus ID.");
  return NULL;
}

// not important now
void print_all_drivers()
{

}

char* gen_bus_new_id(const Destination dest)
{
  static char buffer[10];
  bus_counter++;

  char num[5];
  sprintf(num,"%d",bus_counter);

  switch (dest) {
    case CAIRO_SHOUBRA:
        strcpy(buffer,"SHO-");
        break;
    case CAIRO_METRO:
        strcpy(buffer,"MET-");
        break;
    case CAIRO_RAMSES:
        strcpy(buffer,"RAM-");
        break;
    case ALEX:
        strcpy(buffer,"ALX-");
        break;
    case MENOUFIA_SHEPIN:
        strcpy(buffer,"SHP-");
        break;
    case MENOUFIA_QWESNA:
        strcpy(buffer,"QWE-");
        break;
    case SHARQIA_ZAQAZIQ:
        strcpy(buffer,"ZAQ-");
        break;
    case ELMANSOURA:
        strcpy(buffer,"MNS-");
        break;
    case TANTA:
        strcpy(buffer,"TNT-");
        break;
    default:
        printf("Invalid destination.");
        return NULL;
  }
  strcat(buffer,num);
  return buffer;
}

bool register_driver(const char* name, const char* mobile, Destination dest, const char* gen_id_buffer, const char* pass)
{
  Driver newDriver;

  strcpy(newDriver.bus_id, gen_bus_new_id(dest));
  newDriver.dest = dest;
  strcpy(newDriver.mobile,mobile);
  strcpy(newDriver.name,name);
  strcpy(newDriver.password,pass);
  newDriver.status = STATUS_NOT_ACTIVE;

  if(add_driver_to_list(newDriver))
  {
    save_drivers_to_json();
    return true;
  }
  else return false;

}

bool change_driver_status(const char* bus_id, DriverStatus new_state)
{
  Driver* c_driver = find_driver_by_id(bus_id);

  if(c_driver == NULL) return false;

  switch (new_state) {
    case STATUS_NOT_ACTIVE:
        c_driver->status = STATUS_NOT_ACTIVE;
        // TODO: dequeue the bus
          dequeue_driver(*c_driver);
        break;
    case STATUS_READY:
        c_driver->status = STATUS_READY;
        // TODO: enqueue the bus
        enqueue_driver(*c_driver);
        break;
    case STATUS_ON_TRIP:
        c_driver->status = STATUS_ON_TRIP;
        // TODO: dequeue the bus
        dequeue_driver(*c_driver);
        break;
    default:
        printf("Invalid Status.");
        return false;
  }
  return true;
}


bool diver_login(const char* mobile, const char* pass)
{
  Driver* c_driver = find_driver_by_mobile(mobile);

  if( c_driver == NULL)  return false;                   // no user found
  if(strcmp(c_driver->password,pass) == 0) return true;  // correct password
  else return false;                                        // incorrect password
}

char *dest_id_to_dist_name(const int id)
{
  static char buffer[20];

  switch (id) {
    case CAIRO_SHOUBRA:
        strcpy(buffer,"CAIRO_SHOUBRA");
        break;
    case CAIRO_METRO:
        strcpy(buffer,"CAIRO_METRO");
        break;
    case CAIRO_RAMSES:
        strcpy(buffer,"CAIRO_RAMSES");
        break;
    case ALEX:
        strcpy(buffer,"ALEX");
        break;
    case MENOUFIA_SHEPIN:
        strcpy(buffer,"MENOUFIA_SHEPIN");
        break;
    case MENOUFIA_QWESNA:
        strcpy(buffer,"MENOUFIA_QWESNA");
        break;
    case SHARQIA_ZAQAZIQ:
        strcpy(buffer,"SHARQIA_ZAQAZIQ");
        break;
    case ELMANSOURA:
        strcpy(buffer,"ELMANSOURA");
        break;
    case TANTA:
        strcpy(buffer,"TANTA");
        break;
    default:
        printf("Invalid ID.");
        return NULL;
  }

  return buffer;
}


