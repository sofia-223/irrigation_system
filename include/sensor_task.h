#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "data_structure.h"
#include <stdbool.h>


void SensorTask(void *pvParameters);

#endif // SENSOR_TASK_H
