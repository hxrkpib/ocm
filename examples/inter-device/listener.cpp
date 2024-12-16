#include <stdio.h>

#include <lcm/lcm-cpp.hpp>

#include "exlcm/example_t.hpp"

// 定义一个消息处理类
class Handler {
 public:
  ~Handler() {}

  // 处理接收到的消息
  void handleMessage(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const exlcm::example_t *msg) {
    int i;
    // 打印接收到的消息信息
    printf("Received message on channel \"%s\":\n", chan.c_str());
    printf("  timestamp   = %lld\n", (long long)msg->timestamp);  // 打印时间戳
    printf("  position    = (%f, %f, %f)\n", msg->position[0], msg->position[1],
           msg->position[2]);                                                                                                          // 打印位置信息
    printf("  orientation = (%f, %f, %f, %f)\n", msg->orientation[0], msg->orientation[1], msg->orientation[2], msg->orientation[3]);  // 打印朝向信息
    printf("  ranges:");
    // 打印距离数据
    for (i = 0; i < msg->num_ranges; i++) printf(" %d", msg->ranges[i]);
    printf("\n");
    printf("  name        = '%s'\n", msg->name.c_str());  // 打印名称
    printf("  enabled     = %d\n", msg->enabled);         // 打印启用状态
  }
};

int main(int argc, char **argv) {
  // 创建LCM实例
  lcm::LCM lcm;

  // 检查LCM是否正常工作
  if (!lcm.good()) return 1;

  // 创建Handler实例用于处理消息
  Handler handlerObject;
  // 订阅"EXAMPLE"频道上的消息，并指定处理方法
  lcm.subscribe("EXAMPLE", &Handler::handleMessage, &handlerObject);

  // 持续监听消息
  while (0 == lcm.handle()) {
    // 此处不做任何操作，等待消息
  }

  return 0;  // 程序结束
}
