#ifndef PASSENGER_MANAGER_H
#define PASSENGER_MANAGER_H
#include <stdbool.h>
/*
    Passenger struct
    (Passenger نفسه هو Node في Linked List)
*/
typedef struct Passenger {
    char name[50];
    char email[50];
    char password[20];
    char mobile[15];
    /* Booking history */
    struct {
        char bus_id[20];
        int seat_number;
    } history[20];

    int history_count;

    struct Passenger *next;
} Passenger;

/* Passenger Manager APIs */

void init_passenger_list();

int add_passenger_to_list(Passenger p);

Passenger *find_passenger_by_email(const char *email);

bool register_passenger(const char *name,
                        const char *email,
                        const char *pass,
                        const char *mobile);

bool passenger_login(const char *email, const char *pass);

int book_seat(const char *bus_id, int seat_number);

bool cancel_booking(const char *bus_id,
                    int seat_num,
                    const char *email);

void print_passenger_history(const char *email);

#endif /* PASSENGER_MANAGER_H */
