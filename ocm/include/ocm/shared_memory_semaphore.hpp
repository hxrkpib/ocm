#pragma once

#include <fcntl.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <lcm/lcm-cpp.hpp>
#include <string>

namespace ocm {
/**
 * @brief 信号量包装器，用于管理进程间同步。
 *
 * `SharedMemorySemaphore` 类提供了对命名 POSIX 信号量的接口，
 * 允许多个进程同步访问共享资源。
 */
class SharedMemorySemaphore {
 public:
  /**
   * @brief 使用给定的名称和初始值构造 SharedMemorySemaphore。
   *
   * 通过调用 `Init` 方法初始化信号量。
   *
   * @param name 信号量的名称标识符。
   * @param value 信号量的初始值。
   */
  SharedMemorySemaphore(const std::string& name, unsigned int value);

  /**
   * @brief 析构函数。
   *
   * 默认析构函数确保信号量的正确清理。
   */
  ~SharedMemorySemaphore();

  /**
   * @brief 初始化信号量。
   *
   * 打开一个已存在的命名信号量，如果不存在则创建一个新的信号量。
   *
   * @param sem_name 信号量的名称标识符。
   * @param value 信号量的初始值。
   *
   * @throws std::runtime_error 如果信号量初始化失败。
   */
  void Init(const std::string& sem_name, unsigned int value);

  /**
   * @brief 增加信号量的值。
   *
   * 等同于 "post" 操作，将信号量的计数增加一。
   *
   * @throws std::runtime_error 如果增加操作失败。
   */
  void Increment();

  /**
   * @brief 如果信号量当前值为零，则增加信号量的值。
   *
   * 检查信号量的当前值，只有在其为零时才增加。
   *
   * @throws std::runtime_error 如果增加操作失败。
   */
  void IncrementWhenZero();

  /**
   * @brief 按指定数量增加信号量的值。
   *
   * 执行多个 "post" 操作以增加信号量的计数。
   *
   * @param value 要增加的信号量数量。
   *
   * @throws std::runtime_error 如果任何增加操作失败。
   */
  void Increment(unsigned int value);

  /**
   * @brief 减少信号量的值。
   *
   * 等同于 "wait" 操作，将信号量的计数减少一。
   *
   * @throws std::runtime_error 如果减少操作失败。
   */
  void Decrement();

  /**
   * @brief 尝试在不阻塞的情况下减少信号量的值。
   *
   * 尝试执行 "wait" 操作。如果信号量的值大于零，
   * 则减小其值并返回 `true`。否则，返回 `false`。
   *
   * @return 如果信号量成功减小，则返回 `true`；否则，返回 `false`。
   */
  bool TryDecrement();

  /**
   * @brief 尝试在超时的情况下减少信号量的值。
   *
   * 等待信号量可用，直到指定的超时时间。
   *
   * @param seconds 等待的秒数。
   * @param nanoseconds 等待的纳秒数。
   * @return 如果在超时内成功减小信号量，则返回 `true`；否则，返回 `false`。
   *
   * @throws std::runtime_error 如果获取当前时间失败。
   */
  bool DecrementTimeout(uint64_t milliseconds);

  /**
   * @brief 获取信号量的当前值。
   *
   * @return 信号量的当前计数。
   *
   * @throws std::runtime_error 如果获取信号量的值失败。
   */
  int GetValue() const;

  /**
   * @brief 销毁信号量。
   *
   * 关闭并取消链接信号量，将其从系统中移除。
   *
   * @throws std::runtime_error 如果关闭或取消链接信号量失败。
   */
  void Destroy();

 private:
  sem_t* sem_ = nullptr; /**< 指向 POSIX 信号量的指针。 */
  std::string name_;     /**< 信号量的名称标识符。 */
};

}  // namespace ocm
