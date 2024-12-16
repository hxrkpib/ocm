#pragma once

#include <stdint.h>
#include <time.h>

/*!
 * @file timer.hpp
 * @brief 包含用于时间测量和循环管理的TimerOnce和TimerLoop类的声明。
 */

namespace ocm {

/**
 * @class TimerOnce
 * @brief 使用CLOCK_MONOTONIC时钟测量经过的时间。
 *
 * `TimerOnce`类提供了以毫秒、纳秒和秒为单位测量时间间隔的功能。
 * 它还允许以毫秒为单位获取当前时间。
 */
class TimerOnce {
 public:
  /**
   * @brief 构造并启动定时器。
   *
   * 通过获取当前时间来初始化定时器。
   */
  explicit TimerOnce();

  /**
   * @brief 启动或重启定时器。
   *
   * 将当前时间捕捉为开始时间。
   */
  void start();

  /**
   * @brief 获取自上次启动或调用getNs以来经过的毫秒数。
   *
   * @return 以双精度浮点数表示的经过毫秒数。
   */
  double getMs();

  /**
   * @brief 获取自上次启动或调用getNs以来经过的纳秒数。
   *
   * @return 以64位整数表示的经过纳秒数。
   */
  int64_t getNs();

  /**
   * @brief 获取自上次启动或调用getNs以来经过的秒数。
   *
   * @return 以双精度浮点数表示的经过秒数。
   */
  double getSeconds();

  /**
   * @brief 获取自纪元以来的当前时间，以毫秒为单位。
   *
   * @return 以双精度浮点数表示的当前时间（毫秒）。
   */
  double getNowTime() const;

 private:
  struct timespec _startTime; /**< 存储用于计算经过时间的开始时间 */
  /**< 存储用于计算经过时间的开始时间 */
};

/**
 * @class TimerLoop
 * @brief 使用绝对唤醒时间管理循环定时。
 *
 * `TimerLoop`类提供了设置循环周期和休眠直到下一个循环迭代的功能。
 * 它确保循环根据指定的周期以一致的间隔运行。
 */
class TimerLoop {
 public:
  /**
   * @brief 将内部时钟重置为当前时间。
   *
   * 将当前时间捕捉为唤醒绝对时间并重置内部计数器。
   */
  void ResetClock();

  /**
   * @brief 设置循环周期并初始化唤醒时间。
   *
   * @param period 循环的周期（秒）。
   *
   * 该方法计算以毫秒和纳秒为单位的周期，重置内部时钟，并安排第一次唤醒时间。
   */
  void SetPeriod(double period);

  /**
   * @brief 获取当前循环周期。
   *
   * @return 以双精度浮点数表示的循环周期（毫秒）。
   */
  double GetPeriod() const;

  /**
   * @brief 根据唤醒绝对时间休眠直到下一个循环迭代。
   *
   * 该方法使线程休眠直到预定的唤醒时间，然后安排下一个唤醒时间。
   */
  void SleepUntilNextLoop();

 private:
  /**
   * @brief 将循环周期添加到当前唤醒时间，处理纳秒溢出。
   *
   * 通过添加循环周期更新唤醒绝对时间，确保纳秒不超过十亿（每秒的纳秒数）。
   */
  void AddPeriod();

  timespec wake_abs_time_; /**< 下一个循环迭代的绝对唤醒时间 */
  /**< 下一个循环迭代的绝对唤醒时间 */
  long time_ns_; /**< 唤醒时间的当前纳秒部分 */
  /**< 唤醒时间的当前纳秒部分 */
  long time_s_; /**< 唤醒时间的当前秒部分 */
  /**< 唤醒时间的当前秒部分 */
  double period_ms_; /**< 循环周期，以毫秒为单位 */
  /**< 循环周期，以毫秒为单位 */
  long period_ns_; /**< 循环周期，以纳秒为单位 */
  /**< 循环周期，以纳秒为单位 */

  // Constants for nanosecond calculations
  static constexpr long NS_CARRY = 999999999; /**< 纳秒进位阈值 */
  /**< 纳秒进位阈值 */
  static constexpr long NS_TO_S = 1000000000; /**< 一秒中的纳秒数 */
  /**< 一秒中的纳秒数 */
};

}  // namespace ocm
