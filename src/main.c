#include <stdio.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "sensor_task.h"
#include "irrigator_task.h"
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

    // prepara i parametri per l'IrrigatorTask (serve una struct con tutti i riferimenti)
    IrrigatorParams irrigator1Params  = {
        .sensorQueue = sensorQueue,
        .irrigationMutex = irrigationMutex,
        .irrigationActive = &irrigationActive,
        .name = "Irrigator 1"
    };

    IrrigatorParams irrigator2Params = {
        .sensorQueue = sensorQueue,
        .irrigationMutex = irrigationMutex,
        .irrigationActive = &irrigationActive,
        .name = "Irrigator 2"
    };

    IrrigatorParams irrigator3Params = {
        .sensorQueue = sensorQueue,
        .irrigationMutex = irrigationMutex,
        .irrigationActive = &irrigationActive,
        .name = "Irrigator 3"
    };

    ROSTaskParams rosParams = {
        .irrigationMutex = irrigationMutex,
        .irrigationActive = &irrigationActive
    };

    
    xTaskCreate(SensorTask, "SensorTask", 512, (void *)sensorQueue, 1, NULL); //creo task che simula sensori
    xTaskCreate(IrrigatorTask, "Irrigator1Task", 512, (void *)&irrigator1Params, 1, NULL); //creo task irrigatore1
    xTaskCreate(IrrigatorTask, "Irrigator2Task", 512, (void *)&irrigator2Params, 1, NULL); //creo task irrigatore2
    xTaskCreate(IrrigatorTask, "Irrigator3Task", 512, (void *)&irrigator3Params, 1, NULL); //creo task irrigatore3
    xTaskCreate(ROSTask, "ROSTask", 512, (void *)&rosParams, 1, NULL); //task pubblicazione ROS2

    vTaskStartScheduler();

    // se arrivi qui significa che scheduler è terminato (errore)
    while(1) {}

    return 0;
}
