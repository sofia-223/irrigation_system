#include <stdio.h>
#include "ros_task.h"
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/bool.h>

static rcl_publisher_t publisher;
static std_msgs__msg__Bool msg;
static bool *irrigationActivePtr;

void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
    (void) last_call_time;
    if (timer == NULL) {
        return;
    }
    // aggiorna e pubblica il messaggio
    msg.data = *irrigationActivePtr;
    rcl_publish(&publisher, &msg, NULL);
    printf("[micro-ROS] Published irrigation state: %s\n", msg.data ? "ON" : "OFF");
}

void ROSTask(void *pvParameters) {

    ROSTaskParams *params = (ROSTaskParams *) pvParameters;
    irrigationActivePtr = params->irrigationActive;

    // 1. INIT micro-ROS allocator
    rcl_allocator_t allocator = rcl_get_default_allocator();

    // 2. INIT support structure
    rclc_support_t support;
    rclc_support_init(&support, 0, NULL, &allocator);
    
    // 3. CREATE node
    rcl_node_t node;
    rclc_node_init_default(&node, "irrigation_node", "", &support);

    // 4. CREATE publisher
    rclc_publisher_init_default(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
        "/irrigation_state"
    );

    std_msgs__msg__Bool__init(&msg);

    rcl_timer_t timer;
    rclc_timer_init_default(
        &timer,
        &support.context,
        RCL_MS_TO_NS(3000),
        timer_callback
    );

    rclc_executor_t executor;
    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_timer(&executor, &timer);


    while (1) {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
        vTaskDelay(pdMS_TO_TICKS(100));
    }

}
