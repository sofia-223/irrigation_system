#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdbool.h>
#include <logic_task.h>


typedef struct {
    float humidity;
    float temperature;
    bool raining;
} SensorData;

typedef struct {
    SensorData *sensorData;
    SemaphoreHandle_t sensorDataMutex;

    QueueHandle_t logicQueue;

    // flag aggiornamento dati sensori
    bool *tempUpdated;
    bool *humidityUpdated;
    bool *rainUpdated;
} AggregatorParams;


#endif // DATA_STRUCTURES_H
