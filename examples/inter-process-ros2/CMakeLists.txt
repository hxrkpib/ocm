cmake_minimum_required(VERSION 3.5)
project(ros2_test)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 默认使用 C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

# 查找必需的包
set(CMAKE_PREFIX_PATH "/opt/openrobotlib/ocm" "/opt/openrobotlib/third_party" "/opt/ros/jazzy")
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rosidl_typesupport_cpp REQUIRED)
find_package(rosidl_typesupport_fastrtps_cpp REQUIRED)


add_subdirectory(ros2_msg)
add_subdirectory(main)
