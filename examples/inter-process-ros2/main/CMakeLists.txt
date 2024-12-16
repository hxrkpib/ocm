
# # 生成接口（消息）
# rosidl_generate_interfaces(${PROJECT_NAME}
#   "msg/Test.msg"
#   DEPENDENCIES std_msgs
# )
find_package(OCM REQUIRED)
# 创建可执行文件
add_executable(main src/main.cpp)

# 添加依赖并链接库
# ament_target_dependencies(ros2_test
#   rclcpp
#   std_msgs
# )

# # 将生成的消息类型链接到可执行文件
# rosidl_target_interfaces(ros2_test
#   ${PROJECT_NAME} "rosidl_typesupport_cpp"
# )

# 链接类型支持库
target_link_libraries(main
  rclcpp::rclcpp
  rosidl_typesupport_cpp::rosidl_typesupport_cpp
  rosidl_typesupport_fastrtps_cpp::rosidl_typesupport_fastrtps_cpp
  ros2_msg::ros2_msg__rosidl_generator_cpp
  ros2_msg::ros2_msg__rosidl_typesupport_cpp
  ros2_msg::ros2_msg__rosidl_typesupport_fastrtps_cpp
  ros2_msg::ros2_msg__rosidl_typesupport_introspection_cpp
  OCM::OCM
)

# 安装可执行文件
install(TARGETS main
  DESTINATION lib/${PROJECT_NAME}
)

# # 导出包信息
# ament_package()
