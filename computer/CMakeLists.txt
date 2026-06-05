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
find_package(nav_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(geometry_msgs REQUIRED)
# =========================
# sources
# =========================
set(SOURCES
    main.cpp
)

file(GLOB COMMUNICATION_SOURCES
    communication/*.cpp
    node/*.cpp
    motion/*.cpp
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
    ${CMAKE_SOURCE_DIR}/motion
)

# =========================
# ROS2 dependencies link
# =========================
ament_target_dependencies(server
    rclcpp
    sensor_msgs
    nav_msgs
    tf2_ros
    geometry_msgs
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
