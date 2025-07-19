#include <stdio.h>
#include "logic_task.h"

// Definisce ogni quanto tempo (in millisecondi) la LogicTask si attiva
#define LOGIC_TASK_DELAY_MS 1500

// Task che decide se attivare l'irrigazione in base ai dati dei sensori
void LogicTask(void *pvParameters) {
    // Cast del parametro generico a struttura specifica con i parametri della task
    LogicParams *params = (LogicParams *) pvParameters;
    SensorData received; // Variabile per memorizzare i dati ricevuti dai sensori

    while(1) {
        // Riceve i dati dei sensori dalla coda (bloccante finché non arrivano dati)
        if(xQueueReceive(params->logicQueue, &received, portMAX_DELAY) == pdPASS) {
             // Logica di attivazione dell'irrigazione:
            // accende se NON piove e se:
            // - umidità è sotto al 40% OPPURE
            // - la temperatura è sopra i 30°C
            int shouldIrrigate = (!received.raining && (received.humidity<40.0f || received.temperature>30.0f));

            // Aggiorna la variabile condivisa con mutex per evitare accessi concorrenti
            xSemaphoreTake(params->irrigationMutex, portMAX_DELAY);
            *(params->irrigationActive) = shouldIrrigate;
            xSemaphoreGive(params->irrigationMutex);
            
             // Stampa lo stato dell'irrigazione deciso dalla logica
            printf("[Logic] Received: Temp=%.1f, Humidity=%.1f, Raining=%d, Irrigation: %s\n", received.temperature, received.humidity, received.raining,shouldIrrigate ? "ON" : "OFF");
            
        }
        vTaskDelay(pdMS_TO_TICKS(LOGIC_TASK_DELAY_MS));
    }
}
