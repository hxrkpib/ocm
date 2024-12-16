#include <format>
#include <iostream>
#include "common/struct_type.hpp"
#include "task/task_base.hpp"
using namespace ocm;

class Task : public ocm::TaskBase {
 public:
  // 构造函数，初始化任务名称、定时器类型等
  Task() : ocm::TaskBase("internal_timer_test", ocm::TimerType::INTERNAL_TIMER, 0.0, false, false) {}

  // 重写 Run 方法，输出当前任务的循环持续时间
  void Run() override { std::cout << std::format("[internal_timer_test]{}", this->GetLoopDuration()) << std::endl; }
};

int main() {
  // 创建定时器任务实例
  Task timer_task;

  // 设置系统配置，包括任务优先级和 CPU 亲和性
  SystemSetting system_setting;
  system_setting.priority = 0;        // 设置任务优先级为 0
  system_setting.cpu_affinity = {0};  // 设置 CPU 亲和性为 CPU 0

  // 启动任务
  timer_task.TaskStart(system_setting);

  // 设置任务的执行周期为 1 秒
  timer_task.SetPeriod(1);

  // 程序运行 5 秒钟
  std::this_thread::sleep_for(std::chrono::seconds(5));

  // 销毁任务
  timer_task.TaskDestroy();

  return 0;
}
