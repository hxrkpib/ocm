
#include "task/timer.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>

namespace ocm {

TimerOnce::TimerOnce() { start(); }

void TimerOnce::start() {
  // 获取当前时间并记录为开始时间
  if (clock_gettime(CLOCK_MONOTONIC, &_startTime) != 0) {
    std::cerr << "Failed to get start time: " << strerror(errno) << std::endl;
  }
}

double TimerOnce::getMs() {
  // 返回自上次调用以来经过的时间（毫秒）
  return static_cast<double>(getNs()) / 1.e6;
}

int64_t TimerOnce::getNs() {
  struct timespec now;
  // 获取当前时间
  if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
    std::cerr << "Failed to get current time: " << strerror(errno) << std::endl;
    return 0;
  }

  // 计算经过的纳秒
  int64_t ns = static_cast<int64_t>(now.tv_nsec - _startTime.tv_nsec) + static_cast<int64_t>(now.tv_sec - _startTime.tv_sec) * 1000000000LL;

  // 更新开始时间为当前时间以便下次测量
  _startTime.tv_sec = now.tv_sec;
  _startTime.tv_nsec = now.tv_nsec;

  return ns;
}

double TimerOnce::getSeconds() {
  // 返回自上次调用以来经过的时间（秒）
  return static_cast<double>(getNs()) / 1.e9;
}

double TimerOnce::getNowTime() const {
  struct timespec now;
  // 获取当前时间
  if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
    std::cerr << "Failed to get current time: " << strerror(errno) << std::endl;
    return 0.0;
  }
  // 返回自纪元以来的当前时间（毫秒）
  return static_cast<double>(now.tv_nsec + 1000000000LL * now.tv_sec) / 1.e6;
}

void TimerLoop::ResetClock() {
  // 获取当前时间并重置内部时钟
  if (clock_gettime(CLOCK_MONOTONIC, &wake_abs_time_) != 0) {
    std::cerr << "Failed to reset clock: " << strerror(errno) << std::endl;
    return;
  }
  time_ns_ = wake_abs_time_.tv_nsec;
  time_s_ = wake_abs_time_.tv_sec;
}

void TimerLoop::SetPeriod(double period) {
  // 将周期转换为毫秒和纳秒
  period_ms_ = period * 1000.0;
  period_ns_ = static_cast<long>(period * 1e9);
  ResetClock();  // 重置时钟
  AddPeriod();   // 安排下一个唤醒时间
}

double TimerLoop::GetPeriod() const {
  // 返回当前循环周期
  return period_ms_;
}

void TimerLoop::SleepUntilNextLoop() {
  // 根据唤醒绝对时间休眠直到下一个循环
  if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wake_abs_time_, nullptr) != 0) {
    std::cerr << "Failed to sleep until next loop: " << strerror(errno) << std::endl;
  }
  AddPeriod();  // 安排下一个唤醒时间
}

void TimerLoop::AddPeriod() {
  // 将循环周期添加到当前唤醒时间
  time_ns_ += period_ns_;
  if (time_ns_ > NS_CARRY) {
    time_s_ += time_ns_ / NS_TO_S;  // 处理纳秒溢出
    time_ns_ %= NS_TO_S;
  }
  wake_abs_time_.tv_sec = time_s_;
  wake_abs_time_.tv_nsec = time_ns_;
}

}  // namespace ocm
