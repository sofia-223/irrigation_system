#include <stdio.h>
#include "ros_task.h"

void ROSTask(void *pvParameters) {
    ROSTaskParams *params = (ROSTaskParams *) pvParameters;
     const char *irrigatorName;

    while(1) {
        // Simuliamo la pubblicazione dello stato di irrigazione con print 
        if (xQueueReceive(params->rosNotifyQueue, &irrigatorName, portMAX_DELAY) == pdPASS) {
        //xSemaphoreTake(params->irrigationMutex, portMAX_DELAY);
            const char* state = *(params->irrigationActive) ? "ON" : "OFF";
            printf("[ROS] %s changed irrigation state to: %s\n", irrigatorName, state);
        //xSemaphoreGive(params->irrigationMutex);
        }
        // Simuliamo il delay del task
        //vTaskDelay(pdMS_TO_TICKS(3000));
    }
}