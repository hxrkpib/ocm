cmake_minimum_required(VERSION 3.14)

project(NodeMap LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 追加新的路径到 CMAKE_PREFIX_PATH
set(CMAKE_PREFIX_PATH "/opt/openrobotlib/ocm" "/opt/openrobotlib/third_party" "/opt/ros/jazzy")
# 输出 CMAKE_PREFIX_PATH 进行调试
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

# 查找 OCM 包
find_package(OCM REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rosidl_typesupport_cpp REQUIRED)
find_package(rosidl_typesupport_fastrtps_cpp REQUIRED)

# 添加可执行文件
add_executable(NodeMap node_map.cpp)

# 链接 OCM 库
target_link_libraries(NodeMap PUBLIC OCM::OCM rclcpp::rclcpp
  rosidl_typesupport_cpp::rosidl_typesupport_cpp
  rosidl_typesupport_fastrtps_cpp::rosidl_typesupport_fastrtps_cpp)
