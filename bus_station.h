#ifndef BUS_STATION_H
#define BUS_STATION_H

// To declare that we will use C language
#ifdef __cplusplus
extern "C" {
#endif

// functions should be written below







// Trip Data
typedef struct{
    char bus_id[20];
    char deiver_name[50];
    char departure_time[20];
    int seats_available;
}TripSummary;




#ifdef __cplusplus
}
#endif

#endif // BUS_STATION_H
