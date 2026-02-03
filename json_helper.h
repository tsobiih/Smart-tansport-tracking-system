#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

void save_passengers_to_json();
void load_passengers_from_json();

void save_drivers_to_json();
void load_drivers_from_json();

void save_queues_to_json();
void load_queues_from_json();

#ifdef __cplusplus
}
#endif

#endif // JSON_HELPER_H
