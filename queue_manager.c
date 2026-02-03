#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "queue_manager.h"

QueueNode station_queues[DEST_COUNT];

void init_queues()
{
  for (uint8_t dest = 0; dest < DEST_COUNT; ++dest)
  {
    station_queues[dest].head = NULL;
    station_queues[dest].tail = NULL;
    station_queues[dest].count = 0;
  }
}

int enqueue_driver(Driver d)
{
  DriverNode* newNode = (DriverNode*) malloc(sizeof(DriverNode));
  if(newNode == NULL) return 0;

  newNode->data = d;
  newNode->next = NULL;

  // to edit in the original queue
  QueueNode* q = &station_queues[d.dest];

  if(q->head == NULL)
  {
    q->head = newNode;
    q->tail = newNode;
  }
  else
  {
    q->tail->next = newNode;
    q->tail = newNode;
  }

  q->count++;
  return 1;
}

int dequeue_driver(Driver d)
{
  int dest_id = (int)d.dest;

  QueueNode* q = &station_queues[dest_id];

  if (q->head == NULL) return 0;

  DriverNode* current = q->head;
  DriverNode* prev = NULL;

  while (current != NULL)
  {
    if(strcmp(current->data.mobile, d.mobile) == 0)
    {
      // HE IS THE FIRST ONE
      if(prev == NULL)
      {
        q->head = current->next;

        // HE IS THE ONLY ONE
        if(q->head == NULL)
        {
          q->tail = NULL;
        }
      }
      else
      {
        prev->next = current->next;
        if (current->next == NULL)
        {
          q->tail = prev;
        }
      }
      free(current);
      q->count--;

      return 1;
    }
    prev = current;
    current = current->next;
  }
  return 0;
}

// future work
DriverNode *get_queue_head(Destination dest)
{

}
