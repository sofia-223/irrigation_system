#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "sensor_task.h"

#define SENSOR_TASK_DELAY_MS 1000
#define AGGREGATOR_TASK_DELAY_MS 500

void TemperatureTask(void *pvParameters){
    AggregatorParams *params = (AggregatorParams *) pvParameters;
    
    while(1) {
        float newTemp=(rand() % 310) / 10.0f + 10;

        // Protezione mutex per scrivere temperatura
        xSemaphoreTake(params->sensorDataMutex, portMAX_DELAY);
        params->sensorData->temperature = newTemp;
        *(params->tempUpdated) = true;
        xSemaphoreGive(params->sensorDataMutex);

        printf("[TempTask] Temperature updated: %.1f°C\n", newTemp);

        vTaskDelay(pdMS_TO_TICKS(SENSOR_TASK_DELAY_MS));
    }
}

void HumidityTask(void *pvParameters){
    AggregatorParams *params = (AggregatorParams *) pvParameters;
    
    while(1) {
        float newHumidity = (rand() % 1000) / 10.0f;  // 0.0 - 100.0 %

        xSemaphoreTake(params->sensorDataMutex, portMAX_DELAY);
        params->sensorData->humidity = newHumidity;
        *(params->humidityUpdated) = true;
        xSemaphoreGive(params->sensorDataMutex);

        printf("[HumidityTask] Humidity updated: %.1f%%\n", newHumidity);

        vTaskDelay(pdMS_TO_TICKS(SENSOR_TASK_DELAY_MS));
    }
}

void RainTask(void *pvParameters){

    AggregatorParams *params = (AggregatorParams *) pvParameters;

    while(1) {
        bool newRain = (rand() % 2) == 1;

        xSemaphoreTake(params->sensorDataMutex, portMAX_DELAY);
        params->sensorData->raining = newRain;
        *(params->rainUpdated) = true;
        xSemaphoreGive(params->sensorDataMutex);

        printf("[RainTask] Raining updated: %s\n", newRain ? "Yes" : "No");

        vTaskDelay(pdMS_TO_TICKS(SENSOR_TASK_DELAY_MS));
    }
}

void AggregatorTask(void *pvParameters) {
    AggregatorParams *params = (AggregatorParams *) pvParameters;
    SensorData dataCopy;

    while(1) {
        // Controlla se tutti i dati sono aggiornati
        if(*(params->tempUpdated) && *(params->humidityUpdated) && *(params->rainUpdated)) {
            // Copia protetta dati sensori
            xSemaphoreTake(params->sensorDataMutex, portMAX_DELAY);
            dataCopy = *(params->sensorData);
            // Reset flag aggiornamento
            *(params->tempUpdated) = false;
            *(params->humidityUpdated) = false;
            *(params->rainUpdated) = false;
            xSemaphoreGive(params->sensorDataMutex);

            xQueueReset(params->logicQueue);
            // Invia dati completi alla LogicTask tramite coda
            if(xQueueSend(params->logicQueue, &dataCopy, portMAX_DELAY) != pdPASS) {
                printf("[Aggregator] Failed to send data to logic queue\n");
            } else {
                printf("[Aggregator] Sent data -> Temp: %.1f°C, Humidity: %.1f%%, Raining: %s\n",
                    dataCopy.temperature,
                    dataCopy.humidity,
                    dataCopy.raining ? "Yes" : "No");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(AGGREGATOR_TASK_DELAY_MS));
    }
}
