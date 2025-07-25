#include <stdio.h>
#include "irrigator_task.h"

// Definisce ogni quanto tempo (in millisecondi) l'IrrigatorTask si attiva
#define IRRIGATOR_TASK_DELAY_MS 3000

// Task che decide se attivare l'irrigazione in base ai dati dei sensori
void IrrigatorTask(void *pvParameters) {
    // Cast del parametro generico a struttura specifica con i parametri della task
    IrrigatorParams *params = (IrrigatorParams *) pvParameters;
    SensorData received; // Variabile per memorizzare i dati ricevuti dai sensori

    while(1) {
        // Riceve i dati dei sensori dalla coda (bloccante finché non arrivano dati, aspetta tutto il tempo necessario).
        //La coda è thread safe quindi già qui si decide l'unico thread che riusirà a leggere il dato
        if(xQueueReceive(params->sensorQueue, &received, portMAX_DELAY) == pdPASS) {
            // Logica di attivazione dell'irrigazione:
            // accende se NON piove e se:
            // - umidità è sotto al 40% OPPURE
            // - la temperatura è sopra i 30°C
            int shouldIrrigate = (!received.raining && (received.humidity<60.0f || received.temperature>30.0f));

            if (shouldIrrigate) {
                // Prova a entrare in mutua esclusione
                if (xSemaphoreTake(params->irrigationMutex, pdMS_TO_TICKS(50)) == pdPASS) {
                    if (!(*params->irrigationActive)) {
                        *params->irrigationActive = true;
                        xSemaphoreGive(params->irrigationMutex);

                        xQueueSend(params->rosNotifyQueue, &params->name, 0);
                        printf("[%s] Irrigating...\n", params->name);
                        vTaskDelay(pdMS_TO_TICKS(2000));

                        xSemaphoreTake(params->irrigationMutex,portMAX_DELAY);
                        *params->irrigationActive = false;
                        xSemaphoreGive(params->irrigationMutex);

                        xQueueSend(params->rosNotifyQueue, &params->name, 0);
                        printf("[%s] Done irrigating.\n", params->name);
                    }
                    else {
                        xSemaphoreGive(params->irrigationMutex);
                        printf("[%s] Altro irrigatore attivo\n",params->name);
                    }
                } else {
                    printf("[%s] mutex occupato.\n", params->name);
                }
            } else {
                printf("[%s] No sprinkler needed.\n", params->name);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(IRRIGATOR_TASK_DELAY_MS));
    }
}