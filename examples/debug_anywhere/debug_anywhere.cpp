#include "debug_anywhere/debug_anywhere.hpp"
#include <thread>
#include "common/struct_type.hpp"

using namespace ocm;

int main() {
  // 获取DebugAnywhere实例
  auto& debug_anywhere = DebugAnywhere::getInstance();

  // 初始化DebugAnywhere配置
  debug_anywhere.initialize(DebugAnywhereConfig{
      "192.168.1.100",         // 设置目标IP地址
      "10000",                 // 设置端口号
      "1",                     // 设置设备ID
      true,                    // 启用日志记录
      true,                    // 启用数据发布
      true,                    // 启用数据接收
      SystemSetting{80, {3}},  // 设置系统配置，包括优先级和CPU亲和性
      true,                    // 启用白名单
      {"topic2"},              // 订阅的主题列表
      10                       // 设置DebugAnywhere的队列大小为10
  });

  // 循环发布数据
  for (int i = 0; i < 10; i++) {
    std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0};  // 创建数据向量
    debug_anywhere.Publish("topic1", data);                // 发布数据到"topic1"
    debug_anywhere.Publish("topic2", data);                // 发布数据到"topic2"
    std::this_thread::sleep_for(std::chrono::seconds(1));  // 每隔1秒发布一次
  }

  return 0;  // 程序结束
}
