#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include "sensor_task.h"
#include "logic_task.h"
#include "ros_task.h"
#include "data_structure.h"

SensorData sensorData;
SemaphoreHandle_t sensorDataMutex;
SemaphoreHandle_t irrigationMutex;
bool irrigationActive=false; // variabile condivisa che rappresenta se l'irrigazione è attiva o no

bool humidityUpdated=false;
bool rainUpdated=false;
bool tempUpdated=false; 

int main(void) {
    
    srand((unsigned int)time(NULL));
    // crea una coda per comunicare i dati dei sensori tra i task (fino a 10 elementi)
    QueueHandle_t logicQueue = xQueueCreate(1, sizeof(SensorData));
    
    sensorDataMutex=xSemaphoreCreateMutex();
    // crea un mutex per proteggere l'accesso alla variabile 'irrigationActive'
    irrigationMutex = xSemaphoreCreateMutex();
    
    // variabile condivisa che rappresenta se l'irrigazione è attiva o no
    

     // controllo che queue e mutex siano stati creati correttamente
    if(sensorDataMutex == NULL || irrigationMutex == NULL || logicQueue==NULL) {
        printf("Failed to create mutex or queue\n");
        return 1;
    }

    AggregatorParams aggregatorParams = {
        .sensorData = &sensorData,
        .sensorDataMutex = sensorDataMutex,
        .logicQueue = logicQueue,
        .tempUpdated = &tempUpdated,
        .humidityUpdated = &humidityUpdated,
        .rainUpdated = &rainUpdated
    };

    sensorData.temperature = 0.0f;
    sensorData.humidity = 0.0f;
    sensorData.raining = false;

    // prepara i parametri per la LogicTask (serve una struct con tutti i riferimenti)
    LogicParams logicParams = {
        .logicQueue=logicQueue,
        .irrigationMutex = irrigationMutex,
        .irrigationActive = &irrigationActive
    };

    ROSTaskParams rosParams = {
        .irrigationMutex = irrigationMutex,
        .irrigationActive = &irrigationActive
    };

    xTaskCreate(TemperatureTask, "TemperatureTask", 512, &aggregatorParams, 1, NULL);
    xTaskCreate(HumidityTask, "HumidityTask", 512, &aggregatorParams, 1, NULL);
    xTaskCreate(RainTask, "RainTask", 512, &aggregatorParams, 1, NULL);


    xTaskCreate(AggregatorTask, "AggregatorTask", 512, &aggregatorParams, 2, NULL);
    
    xTaskCreate(LogicTask, "LogicTask", 512, (void *)&logicParams, 1, NULL); //creo task della logica
    xTaskCreate(ROSTask, "ROSTask", 512, (void *)&rosParams, 1, NULL); //task pubblicazione ROS2

    vTaskStartScheduler();

    // se arrivi qui significa che scheduler è terminato (errore)
    while(1) {}

    return 0;
}
