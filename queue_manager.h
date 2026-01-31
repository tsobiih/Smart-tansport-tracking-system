#ifndef QUEUE_MANAGER_H
#define QUEUE_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "driver_manager.h"

typedef struct QueueNode{
    DriverNode* head;
    DriverNode* tail;
    int count;
}QueueNode;

extern QueueNode station_queues[DEST_COUNT];

// Queue APIs
void init_queues();

// Add a driver to queue
int enqueue_driver(Driver d);

// Remove a driver from queue
int dequeue_driver(Driver d);

// Show the queue of a destination
DriverNode* get_queue_head(Destination dest);

#ifdef __cplusplus
}
#endif

#endif // QUEUE_MANAGER_H
