#pragma once

#include <atomic>
#include <boost/lockfree/queue.hpp>
#include <memory>
#include <semaphore>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "common/struct_type.hpp"

namespace ocm {

/**
 * @brief 配置DebugAnywhereTask的设置。
 */
struct DebugAnywhereConfig {
  /** @brief 调试接口的IP地址。 */
  std::string ip;

  /** @brief 调试接口的端口号。 */
  std::string port;

  /** @brief 多播数据包的生存时间（TTL）值。 */
  std::string ttl;

  /** @brief 启用或禁用调试功能的标志。 */
  bool enable;

  /** @brief 启用所有优先级设置的标志。 */
  bool all_priority_enable;

  /** @brief 启用所有CPU亲和性设置的标志。 */
  bool all_cpu_affinity_enable;

  /** @brief 与实时调度相关的系统设置。 */
  SystemSetting system_setting;

  /** @brief 启用白名单过滤的标志。 */
  bool white_list_enable;

  /** @brief 启用白名单时允许的频道集合。 */
  std::vector<std::string> white_list;

  /** @brief 内部数据队列的最大大小。 */
  size_t queue_size;
};

/**
 * @brief 存储与特定主题相关的调试数据的结构体。
 */
struct DebugDataStruct {
  /** @brief 调试数据的主题名称。 */
  std::string topic;

  /** @brief 表示调试数据的双精度值向量。 */
  std::vector<double> data;

  /**
   * @brief 使用给定的主题和数据构造一个DebugDataStruct。
   * @param t 主题名称。
   * @param v 数据值的向量。
   */
  DebugDataStruct(const std::string& t, const std::vector<double>& v);
};

/**
 * @brief 负责处理调试数据发布的任务。
 */
class DebugAnywhereTask {
 public:
  /**
   * @brief 使用指定的配置和队列大小构造一个DebugAnywhereTask。
   * @param config 任务的配置设置。
   */
  DebugAnywhereTask(const DebugAnywhereConfig& config);

  /**
   * @brief 停止任务并回收线程资源的析构函数。
   */
  ~DebugAnywhereTask();

  /**
   * @brief 处理和发布调试数据的主循环。
   */
  void Loop();

  /**
   * @brief 将调试数据发布到指定的频道。
   * @param channel 要发布到的频道名称。
   * @param data 要发布的数据值的向量。
   */
  void Publish(const std::string& channel, const std::vector<double>& data);

 private:
  /** @brief 运行主循环的线程。 */
  std::thread thread_;

  /** @brief IP地址的哈希表示。 */
  std::string ip_hash_;

  /** @brief 任务的配置设置。 */
  DebugAnywhereConfig config_;

  /** @brief 用于存储传入调试数据的无锁队列。 */
  boost::lockfree::queue<DebugDataStruct*> data_queue_;

  /** @brief 用于指示数据可用性的信号量。 */
  std::binary_semaphore sem_;

  /** @brief 原子标志，指示任务是否正在运行。 */
  std::atomic<bool> running_;
};

/**
 * @brief 提供在任何地方发布调试数据接口的单例类。
 */
class DebugAnywhere {
 public:
  // 删除拷贝构造函数和赋值运算符
  DebugAnywhere(const DebugAnywhere&) = delete;
  DebugAnywhere& operator=(const DebugAnywhere&) = delete;

  /**
   * @brief 获取DebugAnywhere的单例实例。
   * @return 单例实例的引用。
   */
  static DebugAnywhere& getInstance();

  /**
   * @brief 使用给定的配置和队列大小初始化DebugAnywhere单例。
   * @param config 调试任务的配置设置。
   * @param queue_size 内部数据队列的最大大小。
   */
  static void initialize(const DebugAnywhereConfig& config);

  /**
   * @brief 将调试数据发布到指定的频道。
   * @param channel 要发布到的频道名称。
   * @param data 要发布的数据值的向量。
   */
  void Publish(const std::string& channel, const std::vector<double>& data);

 private:
  /**
   * @brief 私有构造函数，防止直接实例化。
   */
  DebugAnywhere();

  /**
   * @brief 析构函数。
   */
  ~DebugAnywhere();

  /** @brief 指向DebugAnywhereTask实例的共享指针。 */
  std::shared_ptr<DebugAnywhereTask> task_;
};

}  // namespace ocm