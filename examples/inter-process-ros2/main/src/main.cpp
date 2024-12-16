#include <iostream>
#include "ocm/shared_memory_topic_ros2.hpp"
#include "ros2_msg/msg/my_message.hpp"

int main(int argc, char** argv) {
  ocm::SharedMemoryTopicRos2 topic;
  ros2_msg::msg::MyMessage msg;
  msg.data = "Hello, ROS 2!";
  topic.Publish("test", "test", msg);

  topic.Subscribe<ros2_msg::msg::MyMessage>("test", "test",
                                            [](ros2_msg::msg::MyMessage msg) { std::cout << "Received message: " << msg.data << std::endl; });

  return 0;
}
