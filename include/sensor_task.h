#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "data_structure.h"
#include <stdbool.h>


void TemperatureTask(void *pvParameters);
void HumidityTask(void *pvParameters);
void RainTask(void *pvParameters);
//void SensorTask(void *pvParameters);
void AggregatorTask(void *pvParameters);

#endif // SENSOR_TASK_H
