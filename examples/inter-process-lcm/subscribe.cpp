#include <iostream>
#include "MyData.hpp"
#include "ocm/shared_memory_topic_lcm.hpp"

using namespace ocm;

int main() {
  // 创建共享内存主题对象
  SharedMemoryTopicLcm topic;

  // 创建数据对象，必须为LCM消息
  MyData data;
  int i;

  // 持续订阅"topic1"频道上的数据，使用"shm1"作为共享内存名称
  while (true) {
    // 订阅数据，并设置回调函数来处理收到的消息
    topic.Subscribe<MyData>("topic1", "shm1", [](const MyData& msg) {
      // 当接收到数据时，输出数据的count值
      std::cout << "Received data: " << msg.count << std::endl;
      for (int i = 0; i < 10; i++) {
        std::cout << msg.values[i] << " ";
      }
      std::cout << std::endl;
    });
  }

  return 0;  // 程序结束
}
