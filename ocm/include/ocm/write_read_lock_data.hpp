
#pragma once

#include <fcntl.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <lcm/lcm-cpp.hpp>
#include <memory>
#include <shared_mutex>
#include <stdexcept>

namespace ocm {
/**
 * @brief 读写锁保护的数据包装器。
 *
 * `RWLockData` 类提供了一种线程安全的方式来管理指向类型 `T` 对象的共享指针。
 * 它允许多个并发的读者或独占的写作者，确保多线程环境中的数据完整性。
 *
 * @tparam T 要管理的对象的类型。
 */
template <typename T>
class RWLockData {
 public:
  /**
   * @brief 默认构造函数。
   *
   * 使用默认构造的 `T` 类型对象初始化共享指针。
   */
  RWLockData() { data_ptr_ = std::make_shared<T>(); }

  /**
   * @brief 使用给定数据构造 RWLockData。
   *
   * 使用 `data` 的副本初始化共享指针。
   *
   * @param data 要存储的数据。
   */
  explicit RWLockData(const T& data) { data_ptr_ = std::make_shared<T>(data); }

  /**
   * @brief 删除的拷贝构造函数。
   *
   * 防止复制 `RWLockData` 实例以保持唯一所有权语义。
   */
  RWLockData(const RWLockData&) { throw std::logic_error("[RWLockData] 数据拷贝构造不被允许！"); }

  /**
   * @brief 删除的拷贝赋值运算符。
   *
   * 防止将一个 `RWLockData` 赋值给另一个以保持唯一所有权语义。
   */
  RWLockData& operator=(const RWLockData&) = delete;

  /**
   * @brief 删除的移动构造函数。
   *
   * 防止移动 `RWLockData` 实例以保持唯一所有权语义。
   */
  RWLockData(RWLockData&&) = delete;

  /**
   * @brief 删除的移动赋值运算符。
   *
   * 防止移动赋值 `RWLockData` 实例以保持唯一所有权语义。
   */
  RWLockData& operator=(RWLockData&&) = delete;

  /**
   * @brief 析构函数。
   *
   * 默认析构函数确保共享指针的正确清理。
   */
  ~RWLockData() = default;

  /**
   * @brief 获取共享（读）锁。
   *
   * 允许多个线程同时读取数据。
   */
  void LockRead() { mutex_.lock_shared(); }

  /**
   * @brief 尝试获取共享（读）锁而不阻塞。
   *
   * @return 如果成功获取锁则返回 `true`；否则返回 `false`。
   */
  bool TryLockRead() { return mutex_.try_lock_shared(); }

  /**
   * @brief 释放共享（读）锁。
   */
  void UnlockRead() { mutex_.unlock_shared(); }

  /**
   * @brief 获取独占（写）锁。
   *
   * 确保对数据的独占访问，阻塞其他读者和写者。
   */
  void LockWrite() { mutex_.lock(); }

  /**
   * @brief 尝试获取独占（写）锁而不阻塞。
   *
   * @return 如果成功获取锁则返回 `true`；否则返回 `false`。
   */
  bool TryLockWrite() { return mutex_.try_lock(); }

  /**
   * @brief 释放独占（写）锁。
   */
  void UnlockWrite() { mutex_.unlock(); }

  /**
   * @brief 将新数据赋值给共享指针。
   *
   * 创建一个指向 `data` 副本的新 `std::shared_ptr<T>` 并原子性地赋值。
   *
   * @param data 要存储的新数据。
   */
  void operator=(const T& data) { data_ptr_ = std::make_shared<T>(data); }

  /**
   * @brief 获取共享指针。
   *
   * @return 指向 `T` 类型对象的共享指针。
   */
  std::shared_ptr<T> GetPtr() { return data_ptr_; }

  /**
   * @brief 获取共享指针指向的值。
   *
   * @return `T` 类型对象的副本。
   */
  T GetValue() { return *data_ptr_; }

 private:
  std::shared_ptr<T> data_ptr_; /**< 指向 `T` 类型管理对象的共享指针。 */
  std::shared_mutex mutex_;     /**< 用于管理读写锁的共享互斥锁。 */
};

}  // namespace ocm
