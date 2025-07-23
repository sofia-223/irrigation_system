#ifndef IRRIGATOR_TASK_H
#define IRRIGATOR_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "data_structure.h"
#include <stdbool.h>


typedef struct {
    QueueHandle_t sensorQueue;                      //È la coda dove IrrigatorTask legge i dati generati da SensorTask
    QueueHandle_t rosNotifyQueue;                   //È la coda dove IrrigatorTask comunica lo stato dell'irrigatore al ROS task
    SemaphoreHandle_t irrigationMutex;              //mutex
    bool* irrigationActive;                         //È la variabile condivisa che dice se l’irrigazione è attiva
    const char* name;                               //nome dell'irrigatore
} IrrigatorParams;

void IrrigatorTask(void *pvParameters);

#endif // IRRIGATOR_TASK_H
