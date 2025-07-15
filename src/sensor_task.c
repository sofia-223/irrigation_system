#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "sensor_task.h"

#define SENSOR_TASK_DELAY_MS 1000

static QueueHandle_t sensorQueue;

void SensorTask(void *pvParameters) {
    sensorQueue = (QueueHandle_t) pvParameters;

    // inizializza rand una volta
    srand((unsigned int)time(NULL));

    SensorData data;

    while(1) {
        data.temperature = (rand() % 310) / 10.0f + 10;   // 10 - 25 °C
        data.humidity = (rand() % 1000) / 10.0f;          // 0 - 100 %
        data.raining = rand() % 2;                        // 0 o 1

        xQueueSend(sensorQueue, &data, portMAX_DELAY);

        printf("[Sensor] Temp: %.1f°C | Humidity: %.1f%% | Raining: %s\n",
               data.temperature, data.humidity, data.raining ? "Yes" : "No");

        vTaskDelay(pdMS_TO_TICKS(SENSOR_TASK_DELAY_MS));
    }
}
