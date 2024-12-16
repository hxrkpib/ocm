#pragma once

#include <sys/timerfd.h>
#include <atomic>
#include <cstdint>
#include <semaphore>
#include <thread>
#include "common/enum.hpp"
#include "common/struct_type.hpp"
#include "log_anywhere/log_anywhere.hpp"
#include "ocm/shard_memory_data.hpp"
#include "ocm/shared_memory_semaphore.hpp"
#include "task/timer.hpp"

namespace ocm {

/**
 * @brief 抽象的睡眠机制基类。
 *
 * `SleepBase`类为任务中使用的各种睡眠策略定义了一个通用接口。
 *
 * 它允许派生类根据内部或外部定时器实现特定的睡眠行为。
 */
class SleepBase {
 public:
  /**
   * @brief 默认构造函数。
   */
  SleepBase() = default;

  /**
   * @brief 虚析构函数。
   */
  virtual ~SleepBase() = default;

  /**
   * @brief 使线程在指定的持续时间内进入睡眠。
   *
   * @param duration 睡眠的持续时间，以秒为单位。默认为0。
   */
  virtual void Sleep(double duration = 0) = 0;

  /**
   * @brief 设置睡眠周期。
   *
   * @param period 睡眠机制的周期，以秒为单位。
   */
  virtual void SetPeriod(double period) {}

  /**
   * @brief 获取当前的睡眠周期。
   *
   * @return 睡眠周期，以秒为单位。
   */
  virtual double GetPeriod() const { return 0; }

  /**
   * @brief 继续或恢复睡眠机制。
   */
  virtual void Continue() = 0;
};

/**
 * @brief 使用内部定时器的睡眠机制。
 *
 * `SleepInternalTimer`类使用内部时间循环实现了`SleepBase`接口。
 *
 * 它依赖于`TimerLoop`类来管理睡眠间隔。
 */
class SleepInternalTimer : public SleepBase {
 public:
  /**
   * @brief 构造一个`SleepInternalTimer`实例。
   *
   * 初始化内部定时器，使用默认周期。
   */
  SleepInternalTimer();

  /**
   * @brief 析构函数。
   */
  ~SleepInternalTimer() = default;

  /**
   * @brief 使线程睡眠直到下一个循环迭代。
   *
   * @param duration 睡眠的持续时间，以秒为单位。默认为0。
   */
  void Sleep(double duration = 0) override;

  /**
   * @brief 设置内部定时器的睡眠周期。
   *
   * @param period 周期，以秒为单位。
   */
  void SetPeriod(double period) override;

  /**
   * @brief 获取内部定时器的当前睡眠周期。
   *
   * @return 睡眠周期，以秒为单位。
   */
  double GetPeriod() const override;

  /**
   * @brief 继续或重置内部定时器时钟。
   */
  void Continue() override;

 private:
  TimerLoop timer_loop_; /**< 内部定时器循环，用于管理睡眠间隔 */
};

/**
 * @brief 使用外部定时器的睡眠机制。
 *
 * `SleepExternalTimer`类使用信号量和共享内存等外部同步原语实现了`SleepBase`接口。
 *
 * 它允许外部实体触发睡眠的继续。
 */
class SleepExternalTimer : public SleepBase {
 public:
  /**
   * @brief 构造一个`SleepExternalTimer`实例。
   *
   * 初始化用于外部定时器同步的信号量和共享内存。
   *
   * @param sem_name 信号量名称。
   * @param shm_name 共享内存段名称。
   */
  SleepExternalTimer(const std::string& sem_name, const std::string& shm_name);

  /**
   * @brief 析构函数。
   */
  ~SleepExternalTimer() = default;

  /**
   * @brief 使线程睡眠，直到外部定时器发出继续信号。
   *
   * @param duration 睡眠的持续时间，以秒为单位。默认为0。
   */
  void Sleep(double duration = 0) override;

  /**
   * @brief 根据外部定时器的间隔设置睡眠周期。
   *
   * @param period 周期，以秒为单位。
   */
  void SetPeriod(double period) override;

  /**
   * @brief 获取基于外部定时器间隔的当前睡眠周期。
   *
   * @return 睡眠周期，以秒为单位。
   */
  double GetPeriod() const override;

  /**
   * @brief 继续或重置外部定时器。
   */
  void Continue() override;

 private:
  uint8_t dt_;                     /**< 定时器间隔增量 */
  std::atomic_int interval_time_;  /**< 等待的间隔数量 */
  std::atomic_int interval_count_; /**< 已等待间隔的计数器 */
  SharedMemorySemaphore sem_;      /**< 用于外部同步的信号量 */
  SharedMemoryData<uint8_t> shm_;  /**< 定时器数据的共享内存段 */
};

/**
 * @brief 由信号量触发的睡眠机制。
 *
 * `SleepTrigger`类使用信号量控制睡眠，实现在`SleepBase`接口。
 *
 * 它允许其他进程或线程发出信号，指示何时应继续睡眠。
 */
class SleepTrigger : public SleepBase {
 public:
  /**
   * @brief 构造一个`SleepTrigger`实例。
   *
   * 初始化用于睡眠同步的信号量。
   *
   * @param sem_name 信号量名称。
   */
  SleepTrigger(const std::string& sem_name);

  /**
   * @brief 析构函数。
   */
  ~SleepTrigger() = default;

  /**
   * @brief 使线程睡眠，直到信号量被递减。
   *
   * @param duration 睡眠的持续时间，以秒为单位。默认为0。
   */
  void Sleep(double duration = 0) override;

  /**
   * @brief 继续或发送信号量信号，允许睡眠继续。
   */
  void Continue() override;

 private:
  SharedMemorySemaphore sem_; /**< 用于睡眠同步的信号量 */
};

/**
 * @brief 抽象的任务基类。
 *
 * `TaskBase`类为各种任务提供了通用的接口和功能。
 *
 * 它管理任务的生命周期，包括启动、停止和销毁任务线程。
 */
class TaskBase {
 public:
  /**
   * @brief 构造一个`TaskBase`实例。
   *
   * 使用指定的参数初始化任务并创建任务线程。
   *
   * @param thread_name 任务线程名称。
   * @param type 要使用的定时器类型（`TimerType::INTERNAL_TIMER`，`TimerType::EXTERNAL_TIMER`，`TimerType::TRIGGER`）。
   * @param sleep_duration 睡眠机制的持续时间，以秒为单位。
   * @param all_priority_enable 启用所有优先级设置的标志。
   * @param all_cpu_affinity_enable 启用所有CPU亲和性设置的标志。
   */
  TaskBase(const std::string& thread_name, TimerType type, double sleep_duration, bool all_priority_enable, bool all_cpu_affinity_enable);

  /**
   * @brief 虚析构函数。
   */
  virtual ~TaskBase();

  /**
   * @brief 运行任务。
   *
   * 这是一个纯虚函数，必须由派生类实现以定义任务的特定行为。
   */
  virtual void Run() = 0;

  /**
   * @brief 获取上次运行的持续时间。
   *
   * @return 上次运行的持续时间，以秒为单位。
   */
  double GetRunDuration() const;

  /**
   * @brief 获取上次循环迭代的持续时间。
   *
   * @return 上次循环的持续时间，以秒为单位。
   */
  double GetLoopDuration() const;

  /**
   * @brief 设置任务睡眠机制的周期。
   *
   * @param period 周期，以秒为单位。
   */
  void SetPeriod(double period);

  /**
   * @brief 获取任务的名称。
   *
   * @return 任务名称。
   */
  std::string GetTaskName() const;

  /**
   * @brief 获取任务的当前状态。
   *
   * @return 当前的`TaskState`。
   */
  TaskState GetState() const;

  /**
   * @brief 使用指定的系统设置启动任务。
   *
   * @param system_setting 启动任务的系统设置。
   */
  void TaskStart(const SystemSetting& system_setting);

  /**
   * @brief 使用指定的系统设置停止任务。
   *
   * @param system_setting 停止任务的系统设置。
   */
  void TaskStop(const SystemSetting& system_setting);

  /**
   * @brief 销毁任务，终止线程并清理资源。
   */
  void TaskDestroy();

  bool set_rt_flag_; /**< 标志，指示是否应用了实时设置 */

 private:
  /**
   * @brief 创建任务线程。
   *
   * 初始化线程并启动主循环。
   */
  void TaskCreate();

  /**
   * @brief 任务线程的主循环。
   *
   * 管理任务的执行周期，包括睡眠和运行。
   */
  void Loop();

  /**
   * @brief 设置线程的实时配置。
   *
   * 应用诸如优先级和CPU亲和性等系统设置。
   *
   * @param system_setting 要应用的系统设置。
   */
  void SetRtConfig(const SystemSetting& system_setting);

  std::atomic_bool thread_alive_;     /**< 标志，指示线程是否存活 */
  std::atomic_bool loop_run_;         /**< 标志，指示循环是否应继续运行 */
  std::binary_semaphore start_sem_;   /**< 用于信号任务开始的信号量 */
  int task_id_;                       /**< 任务的标识符 */
  std::string thread_name_;           /**< 任务线程的名称 */
  std::thread thread_;                /**< 任务线程 */
  std::atomic<double> run_duration_;  /**< 上次运行的持续时间 */
  std::atomic<double> loop_duration_; /**< 上次循环的持续时间 */
  std::atomic_bool run_flag_;         /**< 标志，指示任务是否应运行 */

  std::unique_ptr<SleepBase> timer_; /**< 任务使用的睡眠机制 */
  double sleep_duration_;            /**< 睡眠机制的持续时间，以秒为单位 */

  std::atomic<TaskState> state_;       /**< 任务的当前状态 */
  SystemSetting system_setting_start_; /**< 任务开始时的系统设置 */
  SystemSetting system_setting_stop_;  /**< 任务停止时的系统设置 */

  bool all_priority_enable_;     /**< 启用所有优先级设置的标志 */
  bool all_cpu_affinity_enable_; /**< 启用所有CPU亲和性设置的标志 */

  std::shared_ptr<spdlog::logger> logger_; /**< 任务日志记录的记录器 */
};

}  // namespace ocm
