#include <stdio.h>
#include "logic_task.h"

#define LOGIC_TASK_DELAY_MS 1500


void LogicTask(void *pvParameters) {
    LogicParams *params = (LogicParams *) pvParameters;
    SensorData received;

    while(1) {
        if(xQueueReceive(params->sensorQueue, &received, portMAX_DELAY) == pdPASS) {
            int shouldIrrigate = (!received.raining && (received.humidity<40.0f || received.temperature>30.0f));

            xSemaphoreTake(params->irrigationMutex, portMAX_DELAY);
            *(params->irrigationActive) = shouldIrrigate;
            xSemaphoreGive(params->irrigationMutex);

            printf("[Logic] Irrigation: %s\n", shouldIrrigate ? "ON" : "OFF");
        }
        vTaskDelay(pdMS_TO_TICKS(LOGIC_TASK_DELAY_MS));
    }
}
