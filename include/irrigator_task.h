#ifndef IRRIGATOR_TASK_H
#define IRRIGATOR_TASK_H

#include "FreeRTOS.h"                   //per la definizione di base del sistema FreeRTOS e l'utilizzo di macro come pdMS_TO_TICKS()
#include "task.h"                       //per la creazione e le operazioni sui task (per es il delay)
#include "queue.h"                      //per la comunicazione tramite code tra i task
#include "semphr.h"                     //per utilizzare i semafori
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
