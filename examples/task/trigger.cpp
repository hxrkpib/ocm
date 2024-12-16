#include <common/enum.hpp>
#include <format>
#include <iostream>
#include "task/task_base.hpp"
using namespace ocm;

class Task : public ocm::TaskBase {
 public:
  // 构造函数，初始化任务名称、定时器类型等参数
  Task() : ocm::TaskBase("trigger_test", ocm::TimerType::TRIGGER, 0.0, false, false) {}

  // 重写 Run 方法，输出当前任务的循环持续时间
  void Run() override { std::cout << std::format("[trigger_test]{}", this->GetLoopDuration()) << std::endl; }
};

int main() {
  // 创建一个定时器任务实例
  Task timer_task;

  // 创建一个共享内存信号量，用于同步任务
  ocm::SharedMemorySemaphore sem("trigger_test", 0);

  // 设置系统设置，包括任务优先级和 CPU 亲和性
  SystemSetting system_setting;
  system_setting.priority = 0;        // 设置任务优先级为 0
  system_setting.cpu_affinity = {0};  // 设置 CPU 亲和性为 CPU 0

  // 启动任务
  timer_task.TaskStart(system_setting);

  // 模拟执行任务，循环 10 次，每次休眠 1 秒
  for (int i = 0; i < 10; ++i) {
    std::this_thread::sleep_for(std::chrono::seconds(1));  // 休眠 1 秒
    sem.Increment();                                       // 增加信号量
  }

  // 销毁任务
  timer_task.TaskDestroy();

  return 0;
}
