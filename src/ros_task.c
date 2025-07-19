#include <stdio.h>
#include "ros_task.h"

// Stub di esempio senza Micro-ROS per ora
// Qui poi metteremo l’integrazione Micro-ROS

void ROSTask(void *pvParameters) {
    ROSTaskParams *params = (ROSTaskParams *) pvParameters;

    while(1) {
        // Simuliamo la pubblicazione dello stato di irrigazione con print 
        xSemaphoreTake(params->irrigationMutex, portMAX_DELAY);
        int state = *(params->irrigationActive);
        xSemaphoreGive(params->irrigationMutex);

        printf("[ROS] Publishing irrigation state: %s\n", state ? "ON" : "OFF");

        // Simuliamo il delay del task
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
