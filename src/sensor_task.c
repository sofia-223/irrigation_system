#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "sensor_task.h"

#define SENSOR_TASK_DELAY_MS 1000

static QueueHandle_t sensorQueue; //coda per i dati

void SensorTask(void *pvParameters) {
    sensorQueue = (QueueHandle_t) pvParameters;

    // inizializza rand
    srand((unsigned int)time(NULL));

    SensorData data; //struttura in cui memorizzo i dati "letti" dal sensore

    while(1) {
        data.temperature = (rand() % 310) / 10.0f + 10;   // tempreatura causale tra 10 - 40 °C
        data.humidity = (rand() % 1000) / 10.0f;          // umidità casuale 0 - 100 %
        data.raining = rand() % 2;                        // pioggia: 0 o 1

        xQueueSend(sensorQueue, &data, portMAX_DELAY);//invia struttura alla cosa, bloccando finchè non riesce
        
        //DEBUG: stampa i dati 
        printf("[Sensor] Temp: %.1f°C | Humidity: %.1f%% | Raining: %s\n",
               data.temperature, data.humidity, data.raining ? "Yes" : "No");

        vTaskDelay(pdMS_TO_TICKS(SENSOR_TASK_DELAY_MS));
    }
}
