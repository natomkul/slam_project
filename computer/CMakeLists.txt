cmake_minimum_required(VERSION 3.10)

project(server)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================
# ROS2 dependencies
# =========================
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

# =========================
# sources
# =========================
set(SOURCES
    main.cpp
)

file(GLOB COMMUNICATION_SOURCES
    communication/*.cpp
    node/*.cpp
)

# =========================
# executable
# =========================
add_executable(server
    ${SOURCES}
    ${COMMUNICATION_SOURCES}
)

# =========================
# includes
# =========================
target_include_directories(server PRIVATE
    ${CMAKE_SOURCE_DIR}/communication
    ${CMAKE_SOURCE_DIR}/node
)

# =========================
# ROS2 dependencies link
# =========================
ament_target_dependencies(server
    rclcpp
    sensor_msgs
)

# =========================
# install
# =========================
install(TARGETS server
    DESTINATION lib/${PROJECT_NAME}
)

# =========================
# ament package
# =========================
ament_package()
