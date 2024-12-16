
#include "task/task_base.hpp"
#include "task/rt/sched_rt.hpp"
#include "task/timer.hpp"

namespace ocm {

SleepInternalTimer::SleepInternalTimer() {
  SetPeriod(0.01);  // 使用默认的0.01秒周期初始化内部计时器
}

void SleepInternalTimer::Sleep(double duration) {
  timer_loop_.SleepUntilNextLoop();  // 调用内部 TimerLoop 实例的 SleepUntilNextLoop
  if (duration > GetPeriod()) {
    timer_loop_.ResetClock();  // 如果指定的持续时间超过当前周期，则重置定时器时钟
  }
}

void SleepInternalTimer::SetPeriod(double period) {
  timer_loop_.SetPeriod(period);  // 将周期设置委托给内部的 TimerLoop 实例
}

double SleepInternalTimer::GetPeriod() const {
  return timer_loop_.GetPeriod();  // 从内部的 TimerLoop 实例中获取周期
}

void SleepInternalTimer::Continue() {
  timer_loop_.ResetClock();  // 调用内部 TimerLoop 实例的 ResetClock
}

SleepExternalTimer::SleepExternalTimer(const std::string& sem_name, const std::string& shm_name)
    : sem_(sem_name, 0), shm_(shm_name, false, sizeof(uint8_t)) {
  shm_.Lock();               // 锁定共享内存
  dt_ = *shm_.Get();         // 从共享内存中读取初始定时器增量
  shm_.UnLock();             // 解锁共享内存
  interval_count_.store(0);  // 初始化间隔计数
}

void SleepExternalTimer::Sleep(double duration) {
  while (interval_count_ < interval_time_ && duration < GetPeriod()) {
    sem_.Decrement();              // 等待信号量并递减
    interval_count_.fetch_add(1);  // 增加间隔计数
  }
  interval_count_.store(0);  // 重置间隔计数
}

void SleepExternalTimer::SetPeriod(double period) {
  if (dt_ < 1) {
    dt_ = 1;  // 确保定时器增量至少为1毫秒
  }
  interval_time_.store(static_cast<int>(period * 1000 / dt_));  // 根据周期和定时器增量计算所需的间隔数
  interval_count_.store(0);                                     // 重置间隔计数
}

double SleepExternalTimer::GetPeriod() const {
  return static_cast<double>(interval_time_ * dt_);  // 通过将间隔时间乘以定时器增量来计算周期
}

void SleepExternalTimer::Continue() {
  interval_time_.store(0);  // 将间隔时间设置为零
  sem_.Increment();         // 增加信号量以释放任何等待的线程
}

SleepTrigger::SleepTrigger(const std::string& sem_name) : sem_(sem_name, 0) {}

void SleepTrigger::Sleep(double duration) {
  sem_.Decrement();  // 等待信号量被递减
}

void SleepTrigger::Continue() {
  sem_.Increment();  // 增加信号量，释放任何等待的线程
}

TaskBase::TaskBase(const std::string& thread_name, TimerType type, double sleep_duration, bool all_priority_enable, bool all_cpu_affinity_enable)
    : start_sem_(0), sleep_duration_(sleep_duration), all_priority_enable_(all_priority_enable), all_cpu_affinity_enable_(all_cpu_affinity_enable) {
  logger_ = GetLogger();  // 获取日志记录器

  if (type == TimerType::INTERNAL_TIMER) {
    timer_ = std::make_unique<SleepInternalTimer>();  // 根据定时器类型初始化适当的休眠机制
  } else if (type == TimerType::EXTERNAL_TIMER) {
    timer_ = std::make_unique<SleepExternalTimer>(thread_name, thread_name);
  } else if (type == TimerType::TRIGGER) {
    timer_ = std::make_unique<SleepTrigger>(thread_name);
  }

  thread_name_ = thread_name;     // 设置线程名称
  TaskCreate();                   // 创建任务线程
  run_duration_.store(0.0);       // 初始化运行持续时间
  loop_duration_.store(0.0);      // 初始化循环持续时间
  run_flag_.store(false);         // 初始化运行标志
  loop_run_.store(false);         // 初始化循环运行标志
  state_.store(TaskState::INIT);  // 初始化任务状态
}

TaskBase::~TaskBase() = default;

void TaskBase::TaskCreate() {
  thread_alive_.store(true);                                               // 设置线程存活标志
  thread_ = std::thread([this] { Loop(); });                               // 创建任务线程
  logger_->info("[TASK] {} task thread has been created!", thread_name_);  // 记录任务线程创建信息
}

void TaskBase::TaskStart(const SystemSetting& system_setting) {
  system_setting_start_ = system_setting;                              // 设置启动系统设置
  run_flag_.store(true);                                               // 设置运行标志为真
  loop_run_.store(true);                                               // 设置循环运行标志为真
  start_sem_.release();                                                // 释放启动信号量
  logger_->info("[TASK] {} task thread ready to run!", thread_name_);  // 记录任务启动信息
}

void TaskBase::TaskStop(const SystemSetting& system_setting) {
  system_setting_stop_ = system_setting;                                // 设置停止系统设置
  run_flag_.store(false);                                               // 设置运行标志为假
  loop_run_.store(false);                                               // 设置循环运行标志为假
  timer_->Continue();                                                   // 信号定时器继续
  logger_->info("[TASK] {} task thread ready to stop!", thread_name_);  // 记录任务停止信息
}

void TaskBase::TaskDestroy() {
  thread_alive_.store(false);  // 设置线程存活标志为假
  loop_run_.store(false);      // 设置循环运行标志为假
  run_flag_.store(true);       // 设置运行标志为真
  start_sem_.release();        // 释放启动信号量
  timer_->Continue();          // 信号定时器继续

  if (thread_.joinable()) {
    thread_.join();                                                                   // 如果线程可连接，则连接线程
    logger_->info("[TASK] {} task thread has been safely destroyed!", thread_name_);  // 记录任务线程销毁信息
  } else {
    logger_->info("[TASK] {} task thread was already detached or finished!", thread_name_);  // 记录任务线程已分离或完成信息
  }
}

void TaskBase::Loop() {
  ocm::rt::set_thread_name(thread_name_);  // 设置线程名称
  TimerOnce loop_timer;                    // 创建循环计时器
  TimerOnce run_timer;                     // 创建运行计时器

  while (thread_alive_.load()) {
    SetRtConfig(system_setting_stop_);   // 设置实时配置
    state_.store(TaskState::STANDBY);    // 设置任务状态为待命
    start_sem_.acquire();                // 获取启动信号量
    SetRtConfig(system_setting_start_);  // 设置实时配置

    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int64_t>(sleep_duration_ * 1000)));  // 进入运行循环前的初始休眠

    while (loop_run_.load()) {
      timer_->Sleep(GetRunDuration());           // 调用休眠机制
      loop_duration_.store(loop_timer.getMs());  // 获取循环持续时间
      run_timer.start();                         // 启动运行计时器

      if (run_flag_.load()) {
        Run();                             // 执行任务
        state_.store(TaskState::RUNNING);  // 设置任务状态为运行
      }

      run_duration_.store(run_timer.getMs());  // 获取运行持续时间
    }
  }
}

double TaskBase::GetRunDuration() const {
  return run_duration_.load();  // 获取上次运行的持续时间
}

double TaskBase::GetLoopDuration() const {
  return loop_duration_.load();  // 获取上次循环的持续时间
}

void TaskBase::SetPeriod(double period) {
  timer_->SetPeriod(period);  // 将周期设置委托给休眠机制
}

std::string TaskBase::GetTaskName() const {
  return thread_name_;  // 获取任务的名称
}

TaskState TaskBase::GetState() const {
  return state_.load();  // 获取任务的当前状态
}

void TaskBase::SetRtConfig(const SystemSetting& system_setting) {
  pid_t pid = gettid();  // 获取线程ID

  if (system_setting.priority != 0 && all_priority_enable_) {
    ocm::rt::set_thread_priority(pid, system_setting.priority, SCHED_FIFO);  // 设置线程优先级
  }

  if (system_setting.cpu_affinity.size() > 0 && all_cpu_affinity_enable_) {
    ocm::rt::set_thread_cpu_affinity(pid, system_setting.cpu_affinity);  // 设置线程CPU亲和性
  }
}

}  // namespace ocm
