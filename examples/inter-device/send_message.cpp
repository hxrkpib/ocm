#include <lcm/lcm-cpp.hpp>

#include "exlcm/example_t.hpp"

int main(int argc, char **argv) {
  // 创建LCM实例
  lcm::LCM lcm;

  // 检查LCM是否正常工作
  if (!lcm.good()) return 1;

  // 创建要发送的消息数据
  exlcm::example_t my_data;
  my_data.timestamp = 0;  // 设置时间戳

  // 设置位置信息
  my_data.position[0] = 1;
  my_data.position[1] = 2;
  my_data.position[2] = 3;

  // 设置朝向信息（四元数表示）
  my_data.orientation[0] = 1;
  my_data.orientation[1] = 0;
  my_data.orientation[2] = 0;
  my_data.orientation[3] = 0;

  // 设置距离数据
  my_data.num_ranges = 15;                                             // 距离数据的数量
  my_data.ranges.resize(my_data.num_ranges);                           // 调整大小
  for (int i = 0; i < my_data.num_ranges; i++) my_data.ranges[i] = i;  // 填充距离数据

  // 设置名称和启用状态
  my_data.name = "example string";  // 设置名称
  my_data.enabled = true;           // 设置启用状态

  // 发布消息到"EXAMPLE"频道
  lcm.publish("EXAMPLE", &my_data);

  return 0;  // 程序结束
}
