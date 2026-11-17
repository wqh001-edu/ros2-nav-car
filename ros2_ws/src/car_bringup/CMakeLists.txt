cmake_minimum_required(VERSION 3.8)
project(car_bringup)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(sensor_msgs REQUIRED)

add_executable(base_driver src/base_driver.cpp)
ament_target_dependencies(base_driver
  rclcpp
  geometry_msgs
  nav_msgs
  tf2
  tf2_ros
  sensor_msgs
)

install(TARGETS
  base_driver
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch udev
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
