#pragma once
#include <memory>

namespace ocm {

/**
 * @brief 原子指针包装器，用于线程安全的读写操作。
 *
 * `AtomicPtr` 类提供了一种线程安全的方式来管理指向类型 `T` 的常量对象的共享指针。它确保对指针的读写操作是原子的，防止多线程环境中的数据竞争。
 *
 * @tparam T 指针所指向对象的类型。
 */
template <typename T>
class AtomicPtr {
 public:
  /**
   * @brief 默认构造函数。
   *
   * 使用默认构造的 `std::shared_ptr<const T>` 初始化原子指针。
   */
  AtomicPtr() { data_ptr_.store(std::make_shared<const T>()); }

  /**
   * @brief 使用给定数据构造一个 AtomicPtr 实例。
   *
   * 使用指向 `data` 复制品的 `std::shared_ptr<const T>` 初始化原子指针。
   *
   * @param data 要存储在原子指针中的数据。
   */
  explicit AtomicPtr(const T& data) { data_ptr_.store(std::make_shared<const T>(data)); }

  /**
   * @brief 删除的拷贝构造函数。
   *
   * 防止拷贝 `AtomicPtr` 实例以保持唯一所有权语义。
   */
  AtomicPtr(const AtomicPtr&) { throw std::logic_error("[AtomicPtr] Data copy construction is not allowed!"); }

  /**
   * @brief 删除的拷贝赋值运算符。
   *
   * 防止将一个 `AtomicPtr` 赋值给另一个以保持唯一所有权语义。
   */
  AtomicPtr& operator=(const AtomicPtr&) = delete;

  /**
   * @brief 删除的移动构造函数。
   *
   * 防止移动 `AtomicPtr` 实例以保持唯一所有权语义。
   */
  AtomicPtr(AtomicPtr&&) = delete;

  /**
   * @brief 删除的移动赋值运算符。
   *
   * 防止移动赋值 `AtomicPtr` 实例以保持唯一所有权语义。
   */
  AtomicPtr& operator=(AtomicPtr&&) = delete;

  /**
   * @brief 析构函数。
   *
   * 默认析构函数确保正确清理共享指针。
   */
  ~AtomicPtr() = default;

  /**
   * @brief 赋予原子指针新的数据。
   *
   * 创建一个指向 `data` 复制品的新的 `std::shared_ptr<const T>` 并原子地存储它。
   *
   * @param data 要存储的新数据。
   */
  void operator=(const T& data) { data_ptr_.store(std::make_shared<const T>(data)); }

  /**
   * @brief 获取共享指针。
   *
   * 原子加载并返回当前的 `std::shared_ptr<const T>`。
   *
   * @return 一个指向类型 `T` 常量对象的共享指针。
   */
  std::shared_ptr<const T> GetPtr() const { return data_ptr_.load(); }

  /**
   * @brief 获取共享指针指向的值。
   *
   * 原子加载共享指针并返回它指向的对象的副本。
   *
   * @return 类型 `T` 对象的副本。
   */
  T GetValue() const { return *data_ptr_.load(); }

 private:
  std::atomic<std::shared_ptr<const T>> data_ptr_; /**< 指向类型 `T` 常量对象的原子共享指针 */
};

}  // namespace ocm
