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
    QueueHandle_t sensorQueue = xQueueCreate(10, sizeof(SensorData));
    SemaphoreHandle_t irrigationMutex = xSemaphoreCreateMutex();
    bool irrigationActive = false;

    if(sensorQueue == NULL || irrigationMutex == NULL) {
        printf("Failed to create queue or mutex\n");
        return 1;
    }

    // Passaggio parametri ai task
    LogicParams logicParams = {
        .sensorQueue = sensorQueue,
        .irrigationMutex = irrigationMutex,
        .irrigationActive = &irrigationActive
    };

    ROSTaskParams rosParams = {
        .irrigationMutex = irrigationMutex,
        .irrigationActive = &irrigationActive
    };

    xTaskCreate(SensorTask, "SensorTask", 512, (void *)sensorQueue, 1, NULL);
    xTaskCreate(LogicTask, "LogicTask", 512, (void *)&logicParams, 1, NULL);
    xTaskCreate(ROSTask, "ROSTask", 512, (void *)&rosParams, 1, NULL);

    vTaskStartScheduler();

    // Se arrivi qui significa che scheduler è terminato (errore)
    while(1) {}

    return 0;
}
