
project(ros2_msg)
# 指定消息文件
set(msg_files
  "msg/MyMessage.msg"
)

# 生成接口
rosidl_generate_interfaces(ros2_msg
  ${msg_files}
  DEPENDENCIES std_msgs  # 如果你的消息依赖于其他消息包，可以在这里添加
)


if(NOT TARGET ros2_msg::ros2_msg__rosidl_typesupport_cpp)
  add_library(ros2_msg::ros2_msg__rosidl_typesupport_cpp ALIAS ros2_msg__rosidl_typesupport_cpp)
endif()

if(NOT TARGET ros2_msg::ros2_msg__rosidl_typesupport_fastrtps_cpp)
  add_library(ros2_msg::ros2_msg__rosidl_typesupport_fastrtps_cpp ALIAS ros2_msg__rosidl_typesupport_fastrtps_cpp)
endif()


ament_export_dependencies(rosidl_default_runtime)
ament_package()
