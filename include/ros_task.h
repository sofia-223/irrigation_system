#ifndef ROS_TASK_H
#define ROS_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdbool.h>


typedef struct {
    SemaphoreHandle_t irrigationMutex;
    bool* irrigationActive;
    QueueHandle_t rosNotifyQueue;
} ROSTaskParams;

void ROSTask(void *pvParameters);

#endif // ROS_TASK_H
