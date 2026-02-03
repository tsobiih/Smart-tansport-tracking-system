#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "passenger_manager.h"
#include "json_helper.h"

PassengerNode* all_passengers_head = NULL;
static uint32_t id_counter;

void init_passenger_list()
{
  all_passengers_head = NULL;
  id_counter = load_passengers_from_json();
}

int add_passenger_to_list(Passenger p)
{
  PassengerNode* newNode = (PassengerNode*) malloc(sizeof(PassengerNode));
  if (newNode == NULL) return 0;

  newNode->p_data = p;
  newNode->next = all_passengers_head;
  all_passengers_head = newNode;
  return 1;
}

Passenger *find_passenger_by_email(const char *email)
{
  PassengerNode* current_pe = all_passengers_head;

  while (current_pe->next != NULL)
  {
    if (strcmp(current_pe->p_data.email, email) == 0)
    {
      return &(current_pe->p_data);
    }
    current_pe = current_pe->next;
  }
  printf("There is no any user linked with this email.");
  return NULL;
}

bool register_passenger(const char *name, const char *email, const char *pass, const char *mobile)
{
  Passenger newPassenger;
  newPassenger.id = id_counter++;
  strcpy(newPassenger.email,email);
  strcpy(newPassenger.mobile,mobile);
  strcpy(newPassenger.name,name);
  strcpy(newPassenger.password,pass);
  strcpy(newPassenger.lastTrip,"");

  if(add_passenger_to_list(newPassenger))
  {
    // update database file.json
    save_passengers_to_json();
    return true;
  }
  else return false;

}

bool passenger_login(const char *email, const char *pass)
{
  Passenger* c_passenger = find_passenger_by_email(email);

  if( c_passenger == NULL)  return false;                   // no user found
  if(strcmp(c_passenger->password,pass) == 0) return true;  // correct password
  else return false;                                        // incorrect password
}

int book_seat(const char *bus_id, const char* seat_number, const char* email)
{
  char buffer[50];
  Passenger* passenger = find_passenger_by_email(email);
  if( passenger == NULL)  return false;                   // no user found

  strcpy(buffer,bus_id);
  strcat(buffer,",");
  strcat(buffer,seat_number);
  return true;
}

bool cancel_booking(const char *bus_id, const char *email)
{
  char buffer[10];
  Passenger* passenger = find_passenger_by_email(email);
  if( passenger == NULL)  return false;                   // no user found

  strncpy(buffer,passenger->lastTrip,7);
  buffer[7] = '\0';

  // check if it is the same bus id
  if(strcmp(buffer,bus_id) == 0)
  {
    passenger->lastTrip[0] = '\0';
    printf("Trip cancled successfully.");
    return true;
  }
  return false;
}


// will be done soon
void print_passenger_history(const char *email)
{

}
