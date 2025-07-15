#ifndef LOGIC_TASK_H
#define LOGIC_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "data_structure.h"
#include <stdbool.h>


typedef struct {
    QueueHandle_t sensorQueue;
    SemaphoreHandle_t irrigationMutex;
    bool* irrigationActive;
} LogicParams;

void LogicTask(void *pvParameters);

#endif // LOGIC_TASK_H
