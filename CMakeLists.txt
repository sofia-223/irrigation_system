# Define the executable for student_app1
add_executable(irrigation_system 
    src/main.c
    src/sensor_task.c
    src/irrigator_task.c
    src/ros_task.c
)

# Include directories
target_include_directories(irrigation_system PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/apps/common  # Include common_utils headers
    ${CMAKE_SOURCE_DIR}/libmicroros/include  # Per includere Micro-ROS headers
)

# Link against common dependencies
target_link_libraries(irrigation_system FreeRTOS pthread microros)

# Optional: Link against shared utilities if needed
target_link_libraries(irrigation_system common_utils)


