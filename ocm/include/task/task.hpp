
#pragma once

#include <sys/timerfd.h>
#include "common/struct_type.hpp"
#include "log_anywhere/log_anywhere.hpp"
#include "node/node.hpp"
#include "task/task_base.hpp"

namespace ocm {

/**
 * @class Task
 * @brief 表示 OpenRobot OCM（操作控制模块）中的任务。
 *
 * @details
 * 继承自 TaskBase，并管理与任务关联的节点的初始化和执行。
 * 每个 Task 处理一组节点，根据配置设置初始化它们，并在运行时执行各自的进程。
 */
class Task : public TaskBase {
 public:
  /**
   * @brief Task 的构造函数。
   *
   * @details
   * 初始化 TaskBase，包括任务名称、定时器类型、延迟，并设置节点标志。
   *
   * @param task_setting 任务的配置设置，包括任务名称、定时器设置和节点配置。
   * @param node_list 关联任务的节点指针向量的共享指针。
   * @param all_priority_enable 标志，指示是否为任务启用所有优先级设置。
   * @param all_cpu_affinity_enable 标志，指示是否为任务启用所有 CPU 亲和性设置。
   */
  Task(const TaskSetting& task_setting, const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>>& node_list, bool all_priority_enable,
       bool all_cpu_affinity_enable);

  /**
   * @brief 默认析构函数。
   *
   * @details
   * 清理 Task 分配的所有资源。
   */
  ~Task() = default;

  /**
   * @brief 初始化与任务关联的所有节点。
   *
   * @details
   * 将所有节点的初始化标志设置为 true 并调用 InitNode 执行初始化。
   * 该方法确保在任务开始执行之前所有节点都已正确设置。
   */
  void Init();

  /**
   * @brief 初始化与任务关联的特定子集节点。
   *
   * @details
   * 根据提供的列表选择性地初始化节点。
   * 仅初始化 `init_node_list` 中存在的节点。
   *
   * @param init_node_list 要初始化的节点名称集合。
   * @return 成功初始化的节点名称集合。
   */
  std::set<std::string> Init(const std::set<std::string>& init_node_list);

  /**
   * @brief 通过运行并可选择性地输出每个节点来执行任务。
   *
   * @details
   * 该方法重写了 TaskBase 的 Run 方法，并根据任务的定时器定期调用。
   * 它遍历所有关联的节点，运行它们并在启用时处理它们的输出。
   */
  void Run() override;

  /**
   * @brief 获取任务的配置设置。
   *
   * @details
   * 提供对任务配置的访问，包括任务名称、定时器设置和节点配置。
   *
   * @return 对 TaskSetting 结构的常量引用。
   */
  const TaskSetting& GetTaskSetting() const;

 private:
  /**
   * @brief 根据节点的初始化标志初始化节点。
   *
   * @details
   * 该方法遍历节点列表，初始化每个初始化标志设置为 true 的节点。
   * 它调用每个节点的 Init 和 RunOnce 方法，并在启用时处理它们的输出。
   *
   * @param node_init_flag 包含节点名称及其初始化状态的映射。
   */
  void InitNode(const std::unordered_map<std::string, bool>& node_init_flag);

  /**
   * @brief 用于跟踪每个节点是否应输出数据的映射。
   *
   * @details
   * 键是节点的名称，值指示该节点是否启用了输出。
   */
  std::unordered_map<std::string, bool> node_output_flag_;

  /**
   * @brief 任务的配置设置。
   *
   * @details
   * 包含所有相关的设置，如任务名称、定时器设置和节点配置。
   */
  TaskSetting task_setting_;

  /**
   * @brief 用于跟踪每个节点是否已初始化的映射。
   *
   * @details
   * 键是节点的名称，值指示该节点是否已初始化。
   */
  std::unordered_map<std::string, bool> node_init_flag_;

  /**
   * @brief 关联任务的节点列表的共享指针。
   *
   * @details
   * 列表中的每个节点代表任务管理和执行的不同组件或进程。
   */
  std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list_;
};

}  // namespace ocm
