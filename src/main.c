#include <stdio.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "sensor_task.h"
#include "logic_task.h"
#include "ros_task.h"
#include "data_structure.h"

int main(void) {
    
    // crea una coda per comunicare i dati dei sensori tra i task (fino a 10 elementi)
    QueueHandle_t sensorQueue = xQueueCreate(10, sizeof(SensorData));
    
    // crea un mutex per proteggere l'accesso alla variabile 'irrigationActive'
    SemaphoreHandle_t irrigationMutex = xSemaphoreCreateMutex();
    
    // variabile condivisa che rappresenta se l'irrigazione è attiva o no
    bool irrigationActive = false;

     // controllo che queue e mutex siano stati creati correttamente
    if(sensorQueue == NULL || irrigationMutex == NULL) {
        printf("Failed to create queue or mutex\n");
        return 1;
    }

    // prepara i parametri per la LogicTask (serve una struct con tutti i riferimenti)
    LogicParams logicParams = {
        .sensorQueue = sensorQueue,
        .irrigationMutex = irrigationMutex,
        .irrigationActive = &irrigationActive
    };

    ROSTaskParams rosParams = {
        .irrigationMutex = irrigationMutex,
        .irrigationActive = &irrigationActive
    };

    
    xTaskCreate(SensorTask, "SensorTask", 512, (void *)sensorQueue, 1, NULL); //creo task che simula sensori
    xTaskCreate(LogicTask, "LogicTask", 512, (void *)&logicParams, 1, NULL); //creo task della logica
    xTaskCreate(ROSTask, "ROSTask", 512, (void *)&rosParams, 1, NULL); //task pubblicazione ROS2

    vTaskStartScheduler();

    // se arrivi qui significa che scheduler è terminato (errore)
    while(1) {}

    return 0;
}
