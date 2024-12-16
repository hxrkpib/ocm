#include "MyData.hpp"
#include "ocm/shared_memory_topic_lcm.hpp"
using namespace ocm;

int main() {
  // 创建共享内存主题对象
  SharedMemoryTopicLcm topic;

  // 创建数据对象，并设置初始值，必须为LCM消息
  MyData data;
  data.count = 1;  // 设置计数值

  // 填充数据数组
  for (int i = 0; i < 10; i++) {
    data.values[i] = i;  // 将数组填充为0到9的值
  }

  // 发布数据到"topic1"频道，使用"shm1"作为共享内存名称
  topic.Publish("topic1", "shm1", &data);

  return 0;  // 程序结束
}
