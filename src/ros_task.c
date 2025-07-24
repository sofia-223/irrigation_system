#include <stdio.h>
#include "ros_task.h"
#include <rcl/rcl.h>                    //libreria base di ros2, per creare i nodi (rcl_node_t) e i public/subscriber (rcl_publisher_t)
#include <rclc/rclc.h>                  //per gestire tutte le funzioni rclc
#include <rclc/executor.h>              //serve per la gestione degli eventi (rcl_timer_t, rclc_executor_t)
#include <std_msgs/msg/bool.h>          //per creare e gestire i messaggi booleani del task ros

static rcl_publisher_t publisher;           //nodo che pubblica il messaggio su ROS
static std_msgs__msg__Bool msg;             //messaggio da inviare
static bool *irrigationActivePtr;           //puntatore alla variabile condivisa che mi dice se l'irrigatore è attivo o meno
static SemaphoreHandle_t irrigationMutex;
static QueueHandle_t rosNotifyQueue;

//se una chiamata ROS fallisce, stampa errore e termina il task
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
//se una chiamata ROS fallisce, stampa errore ma continua
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}


//Funzione per pubblicare lo stato
static void publish_irrigation_state(const char* source) {
    if (xSemaphoreTake(irrigationMutex, portMAX_DELAY) == pdPASS) {
        msg.data = *irrigationActivePtr;
        xSemaphoreGive(irrigationMutex);
    } else {
        printf("[micro-ROS] Semaforo occupato\n");
        return;
    }
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    //printf("[micro-ROS][%s] Published irrigation state: %s\n",source, msg.data ? "ON" : "OFF");
}


//funzione chiamata periodicamente dal timer per pubblicare lo stato dell'irrigazione
void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
	RCLC_UNUSED(last_call_time);        //per evitare warning del compilatore sul parametro non usato
    if (timer == NULL) {
        return;
    }
    publish_irrigation_state("Timer");
}

void ROSTask(void *pvParameters) {

    ROSTaskParams *params = (ROSTaskParams *) pvParameters;
    const char *irrigatorName;

    irrigationActivePtr = params->irrigationActive;
    irrigationMutex = params->irrigationMutex;
    rosNotifyQueue=params->rosNotifyQueue;

    // inizializza allocatore 
    rcl_allocator_t allocator = rcl_get_default_allocator();

    // inizializza struttura di supporto
    rclc_support_t support;
    RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
    
    // crea un nodo ROS chiamato irrigation_node
    rcl_node_t node;
    RCCHECK(rclc_node_init_default(&node, "irrigation_node", "", &support));

    // crea il publisher per il topic "irrigation_state" che pubblica messaggi true o false a seconda dello stato attuale di irrigazione
    RCCHECK(rclc_publisher_init_default(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
        "/irrigation_state"
    ));

    //inizializza msg
    std_msgs__msg__Bool__init(&msg);

    //crea un timer che chiama la funzione ogni 7 sec 
    rcl_timer_t timer;
    RCCHECK(rclc_timer_init_default(
        &timer,
        &support,
        RCL_MS_TO_NS(7000), //timer timeout
        timer_callback
    ));

    //crea executor che gestice il timer 
    rclc_executor_t executor;
    RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
    RCCHECK(rclc_executor_add_timer(&executor, &timer));


    while (1) {
        //esegue callbeck timer 
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));

        if(xQueueReceive(rosNotifyQueue,&irrigatorName,0)==pdPASS){
            //se arriva una notifica sulla coda pubblica immediatamente
            publish_irrigation_state("Event");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // libera risorse
	RCCHECK(rcl_publisher_fini(&publisher, &node));
	RCCHECK(rcl_node_fini(&node));

  	vTaskDelete(NULL);
}
