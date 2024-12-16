#pragma once

#include <log_anywhere/log_anywhere.hpp>
#include <memory>
#include <set>
#include <string>
#include "common/struct_type.hpp"
#include "node/node_map.hpp"
#include "ocm/atomic_ptr.hpp"
#include "ocm/shared_memory_topic_lcm.hpp"
#include "task/task.hpp"

namespace ocm {

/**
 * @class Executer
 * @brief 管理OpenRobot操作控制模块（OCM）内任务组的执行和切换。
 *
 * Executer 类继承自 TaskBase，负责根据提供的配置创建、初始化以及在不同任务组之间进行切换。
 * 它确保任务能够适当地启动和停止，处理依赖关系以及排他性任务组。
 */
class Executer : public TaskBase {
 public:
  /**
   * @brief Executer 的构造函数。
   *
   * 初始化 Executer，设置其配置，建立日志记录，并初始化内部状态变量。
   *
   * @param executer_config Executer 的配置设置。
   * @param node_map 用于检索节点指针的 NodeMap 的共享指针。
   * @param desired_group_topic_name 期望的任务组主题名称。
   */
  Executer(const ExecuterConfig& executer_config, const std::shared_ptr<NodeMap>& node_map, const std::string& desired_group_topic_name);

  /**
   * @brief 默认析构函数。
   *
   * 清理 Executer 使用的资源。
   */
  ~Executer() = default;

  /**
   * @brief 根据配置为常驻和待命组创建任务。
   *
   * 从 NodeMap 中检索节点指针，并为配置中的每个任务初始化 Task 实例。
   * 同时填充排他性任务组集合。
   */
  void CreateTask();

  /**
   * @brief 初始化常驻组中的所有任务，确保前置节点已准备就绪。
   *
   * 根据其前置节点依赖关系，等待所有任务准备好启动。
   */
  void InitTask();

  /**
   * @brief 执行 Executer 的主运行循环。
   *
   * 检查任务组之间的切换并相应地处理。
   */
  void Run() override;

  /**
   * @brief 优雅地退出所有任务。
   *
   * 停止并销毁常驻和待命组中的所有任务，确保干净的关闭。
   */
  void ExitAllTask();

 private:
  /**
   * @brief 检查是否需要在任务组之间进行切换。
   *
   * 如果需要切换，它将准备必要的集合和标志以处理切换。
   */
  void TransitionCheck();

  /**
   * @brief 处理任务组之间的切换。
   *
   * 停止当前任务，启动目标任务，并更新当前任务组。
   */
  void Transition();

  // 原子指针用于在多线程环境中安全管理期望和当前的任务组

  /**
   * @brief 原子指针，用于存储当前任务组的名称。
   *
   * 用于在多线程环境中安全地更新和访问当前组。
   */
  AtomicPtr<std::string> current_group_;

  /**
   * @brief 原子指针，用于存储期望任务组的名称。
   *
   * 用于在多线程环境中安全地更新和访问期望组。
   */
  AtomicPtr<std::string> desired_group_;

  /**
   * @brief 用于记录信息和错误的日志记录器。
   *
   * 利用 spdlog 库记录 Executer 内的各种事件和状态。
   */
  std::shared_ptr<spdlog::logger> logger_;

  /**
   * @brief 用于存储常驻和待命组任务的映射。
   *
   * 这些无序映射将任务名称与各自的 Task 实例关联起来。
   */
  std::unordered_map<std::string, std::shared_ptr<Task>> resident_group_task_list_;
  std::unordered_map<std::string, std::shared_ptr<Task>> standby_group_task_list_;

  /**
   * @brief 用于存储任务系统设置的映射。
   *
   * 将任务名称与其相应的 SystemSetting 配置关联起来。
   */
  std::unordered_map<std::string, SystemSetting> system_setting_map_;

  /**
   * @brief 排他性任务组的集合。
   *
   * 包含排他性任务组的名称，这意味着一次只能有一个任务组处于活动状态。
   */
  std::set<std::string> exclusive_group_set_;

  /**
   * @brief 用于检索节点指针的 NodeMap 的共享指针。
   *
   * 方便访问任务所需的节点实例。
   */
  std::shared_ptr<NodeMap> node_map_;

  /**
   * @brief Executer 的配置设置。
   *
   * 保存配置和管理 Executer 所需的所有必要设置。
   */
  ExecuterConfig executer_config_;

  /**
   * @brief 用于管理任务和节点切换的集合。
   *
   * 这些集合在切换期间跟踪目标和当前的任务及节点。
   */
  std::set<std::shared_ptr<Task>> target_task_set_;
  std::set<std::shared_ptr<Task>> current_task_set_;
  std::set<std::string> target_node_set_;
  std::set<std::string> current_node_set_;
  std::set<std::string> enter_node_set_;
  std::set<std::string> exit_node_set_;

  /**
   * @brief 用于跟踪节点检查和任务切换状态的标志。
   *
   * 这些布尔标志指示切换的进展和状态。
   */
  bool all_node_exit_check_;
  bool all_node_enter_check_;
  bool task_stop_flag_;
  bool task_start_flag_;
  bool all_current_task_stop_;

  /**
   * @brief 用于跟踪目标和期望任务组的字符串。
   *
   * 用于比较和管理不同任务组之间的切换。
   */
  std::string target_group_;
  std::string desired_group_history_;

  /**
   * @brief 指示是否正在进行切换的标志。
   *
   * 确保一次只发生一次切换。
   */
  bool is_transition_;

  /**
   * @brief 期望任务组的共享内存主题。
   */
  std::shared_ptr<SharedMemoryTopicLcm> desired_group_topic_lcm_;

  /**
   * @brief 期望任务组主题的名称。
   */
  std::string desired_group_topic_name_;
};

}  // namespace ocm
