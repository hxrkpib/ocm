#include <format>
#include <iostream>
#include "task/task_base.hpp"
using namespace ocm;

class Task : public ocm::TaskBase {
 public:
  // 构造函数，初始化任务名称、定时器类型等
  Task() : ocm::TaskBase("external_timer_test", ocm::TimerType::EXTERNAL_TIMER, 0.0, false, false) {}

  // 重写 Run 方法，输出当前任务的循环持续时间
  void Run() override { std::cout << std::format("[external_timer_test]{}", this->GetLoopDuration()) << std::endl; }
};

// 定义一个继承自TaskBase的任务类，用于定时任务
class TaskTimer : public ocm::TaskBase {
 public:
  // 构造函数，初始化任务名称、定时器类型、周期等
  TaskTimer()
      : ocm::TaskBase("external_timer_test_timer", ocm::TimerType::INTERNAL_TIMER, 0.0, false, false),
        sem_("external_timer_test", 0),                       // 创建信号量
        shm_("external_timer_test", true, sizeof(uint8_t)) {  // 创建共享内存
    shm_.Lock();                                              // 锁定共享内存
    *shm_.Get() = 1;                                          // 设置为1毫秒（任务定时器周期）
    shm_.UnLock();                                            // 解锁共享内存
  }

  // 默认析构函数
  ~TaskTimer() = default;

  // 重写Run方法，每次运行时递增信号量
  void Run() override {
    sem_.IncrementWhenZero();  // 如果信号量为0，则递增信号量
  }

 private:
  ocm::SharedMemorySemaphore sem_;      // 信号量
  ocm::SharedMemoryData<uint8_t> shm_;  // 共享内存数据
};

int main() {
  // 设置系统配置，任务优先级和 CPU 亲和性
  SystemSetting system_setting;
  system_setting.priority = 0;        // 设置任务优先级为0
  system_setting.cpu_affinity = {0};  // 设置 CPU 亲和性为 CPU 0

  // 创建定时任务 Timer
  TaskTimer timer_task;
  timer_task.SetPeriod(0.001);           // 设置任务周期为 1 毫秒
  timer_task.TaskStart(system_setting);  // 启动定时任务

  // 创建普通任务 Task
  Task task;
  task.SetPeriod(1);               // 设置任务周期为 1 秒
  task.TaskStart(system_setting);  // 启动任务

  // 程序运行 5 秒钟
  std::this_thread::sleep_for(std::chrono::seconds(5));

  // 销毁任务
  timer_task.TaskDestroy();  // 销毁定时任务
  task.TaskDestroy();        // 销毁普通任务

  return 0;
}
