#ifndef PASSENGER_MANAGER_H
#define PASSENGER_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// Passenger Data
typedef struct {
    int id;
    char name[50];
    char mobile[15];
    char email[20];
    char password[20];
    char lastTrip[50];

}Passenger;

// Passenger Linked List
typedef struct PassengerNode{
    Passenger p_data;
    struct PassengerNode* next;

}PassengerNode;

// To be Global
extern PassengerNode* all_passengers_head;

// User APIs
void init_passenger_list();
int add_passenger_to_list(Passenger p);
Passenger* find_passenger_by_email(const char* email);

bool register_passenger(const char* name, const char* email, const char* pass, const char* mobile);
bool passenger_login(const char* email, const char* pass);
int book_seat(const char *bus_id, const char* seat_number, const char* email);
bool cancel_booking(const char *bus_id, const char *email);
void print_passenger_history(const char* email);

#ifdef __cplusplus
}
#endif

#endif // PASSENGER_MANAGER_H
