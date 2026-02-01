#include "passenger_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAX_HISTORY 20
typedef struct {
    char bus_id[20];
    int seat_number;
} Booking;
struct Passenger {
    char name[50];
    char email[50];
    char password[20];
    char mobile[15];
    Booking history[MAX_HISTORY];
    int history_count;
    struct Passenger *next;
};
static Passenger *head = NULL;
void init_passenger_list()
{ head = NULL;
}
int add_passenger_to_list(Passenger p)
{
  Passenger *new_node = (Passenger *)malloc(sizeof(Passenger));
    if (!new_node)
        return -1;
    *new_node = p;
    new_node->next = NULL;

    if (head == NULL)
    {
        head = new_node;
    }
    else
    {
        Passenger *temp = head;
        while (temp->next)
            temp = temp->next;
        temp->next = new_node;
    }
    return 0;
}
Passenger *find_passenger_by_email(const char *email)
{Passenger *temp = head;
    while (temp)
    {
        if (strcmp(temp->email, email) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}
bool register_passenger(const char *name, const char *email, const char *pass, const char *mobile)
{if (find_passenger_by_email(email))
        return false;
    Passenger p;
    strcpy(p.name, name);
    strcpy(p.email, email);
    strcpy(p.password, pass);
    strcpy(p.mobile, mobile);
    p.history_count = 0;
    p.next = NULL;
    return add_passenger_to_list(p) == 0;
}
bool passenger_login(const char *email, const char *pass)
{Passenger *p = find_passenger_by_email(email);
    if (!p)
        return false;
    return strcmp(p->password, pass) == 0;
}
int book_seat(const char *bus_id, int seat_number)
{Passenger *p = head;
    while (p && p->next)
        p = p->next;
    if (!p || p->history_count >= MAX_HISTORY)
        return -1;
    strcpy(p->history[p->history_count].bus_id, bus_id);
    p->history[p->history_count].seat_number = seat_number;
    p->history_count++;
    return 0;
}
bool cancel_booking(const char *bus_id, int seat_num, const char *email)
{ Passenger *p = find_passenger_by_email(email);
    if (!p)
        return false;
    for (int i = 0; i < p->history_count; i++)
    {
        if (strcmp(p->history[i].bus_id, bus_id) == 0 &&
            p->history[i].seat_number == seat_num)
        {
            for (int j = i; j < p->history_count - 1; j++)
                p->history[j] = p->history[j + 1];
            p->history_count--;
            return true;
        }
    }
    return false;
}
void print_passenger_history(const char *email)
{ Passenger *p = find_passenger_by_email(email);
    if (!p)
    {
        printf("Passenger not found\n");
        return;
    }
    printf("Passenger: %s\n", p->name);
    for (int i = 0; i < p->history_count; i++)
    {
        printf("Bus: %s | Seat: %d\n",
               p->history[i].bus_id,
               p->history[i].seat_number);
    }
}
