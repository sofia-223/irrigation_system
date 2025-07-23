#include <stdio.h>
#include "ros_task.h"
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/bool.h>

static rcl_publisher_t publisher;
static std_msgs__msg__Bool msg;
static bool *irrigationActivePtr;
static SemaphoreHandle_t irrigationMutex;
static QueueHandle_t rosNotifyQueue;

static void publish_irrigation_state(const char* source) {
    if (xSemaphoreTake(irrigationMutex, portMAX_DELAY) == pdPASS) {
        msg.data = *irrigationActivePtr;
        xSemaphoreGive(irrigationMutex);
    } else {
        printf("[micro-ROS] Could not take mutex to read irrigation state\n");
        return;
    }
    rcl_publish(&publisher, &msg, NULL);
    //printf("[micro-ROS][%s] Published irrigation state: %s\n",source, msg.data ? "ON" : "OFF");
}


//funzione chiamata periodicamente dal timer per pubblicare lo stato dell'irrigazione
void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
    (void) last_call_time;
    if (timer == NULL) {
        return;
    }
    publish_irrigation_state("Timer");
}

>>>>>>> microros
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
    rclc_support_init(&support, 0, NULL, &allocator);
    
    // crea un nodo ROS chiamato irrigation_node
    rcl_node_t node;
    rclc_node_init_default(&node, "irrigation_node", "", &support);

    // crea il publisher per il topic "irrigation_state" che pubblica messaggi true o false a seconda dello stato attuale di irrigazione
    rclc_publisher_init_default(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
        "/irrigation_state"
    );

    //inizializza msg
    std_msgs__msg__Bool__init(&msg);

    //crea un timer che chiama la funzione ogni 3 sec 
    rcl_timer_t timer;
    rclc_timer_init_default(
        &timer,
        &support.context,
        RCL_MS_TO_NS(7000),
        timer_callback
    );

    //crea executor che gestice il timer 
    rclc_executor_t executor;
    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_timer(&executor, &timer);


    while (1) {
        //esegue callbeck timer 
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));

        if(xQueueReceive(rosNotifyQueue,&irrigationName,0)==pdPASS){
            //se arriva una notifica sulla coda pubblica immediatamente
            publish_irrigation_state("Event");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }

}
