#include <stdio.h>
#include "irrigator_task.h"

// Definisce ogni quanto tempo (in millisecondi) l'IrrigatorTask si attiva
#define IRRIGATOR_TASK_DELAY_MS 4000

// Task che decide se attivare l'irrigazione in base ai dati dei sensori
void IrrigatorTask(void *pvParameters) {
    // Cast del parametro generico a struttura specifica con i parametri della task
    IrrigatorParams *params = (IrrigatorParams *) pvParameters;
    SensorData received; // Variabile per memorizzare i dati ricevuti dai sensori

    while(1) {
        // Riceve i dati dei sensori dalla coda (bloccante finché non arrivano dati, aspetta tutto il tempo necessario)
        if(xQueueReceive(params->sensorQueue, &received, portMAX_DELAY) == pdPASS) {
            // Logica di attivazione dell'irrigazione:
            // accende se NON piove e se:
            // - umidità è sotto al 40% OPPURE
            // - la temperatura è sopra i 30°C
            int shouldIrrigate = (!received.raining && (received.humidity<60.0f || received.temperature>30.0f));

            if (shouldIrrigate) {
                // Prova a entrare in mutua esclusionex
                //if (xSemaphoreTake(params->irrigationMutex, 0) == pdPASS) {
                    if (!(*params->irrigationActive)) {
                        *params->irrigationActive = true;
                        printf("[%s] Irrigating...\n", params->name);
                        vTaskDelay(pdMS_TO_TICKS(2000));
                        *params->irrigationActive = false;
                        printf("[%s] Done irrigating.\n", params->name);
                    }
                    //xSemaphoreGive(params->irrigationMutex);
               // } else {
                 //   printf("[%s] Un altro irrigatore è già attivo.\n", params->name);
               // }
            } else {
                printf("[%s] No sprinkler needed.\n", params->name);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(IRRIGATOR_TASK_DELAY_MS));
    }
}