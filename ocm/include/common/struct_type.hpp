#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "common/enum.hpp"

namespace ocm {

/**
 * @struct NodeConfig
 * @brief 节点的配置设置。
 *
 * 该结构体包含单个节点的配置详情，包括其名称和输出启用状态。
 */
struct NodeConfig {
  std::string node_name; /**< 节点的名称标识符。 */
  bool output_enable;    /**< 标志，指示节点的输出是否被启用。 */
};

/**
 * @struct TimerSetting
 * @brief 定时器的配置设置。
 *
 * 该结构体定义了定时器的设置，包括其类型和周期时长。
 */
struct TimerSetting {
  TimerType timer_type; /**< 定时器的类型，由 `TimerType` 枚举定义。 */
  double period;        /**< 定时器的周期，单位为秒。 */
};

/**
 * @struct SystemSetting
 * @brief 系统配置设置。
 *
 * 该结构体包含与系统相关的设置，如优先级和CPU亲和性。
 */
struct SystemSetting {
  int priority;                  /**< 系统的优先级级别。 */
  std::vector<int> cpu_affinity; /**< 系统所关联的CPU核心列表。 */
};

/**
 * @struct LaunchSetting
 * @brief 任务启动行为的配置设置。
 *
 * 该结构体定义了与任务启动行为相关的设置，包括预初始化节点和启动延迟。
 */
struct LaunchSetting {
  std::vector<std::string> pre_node; /**< 启动任务前需初始化的节点名称列表。 */
  double delay;                      /**< 启动任务前的延迟时间（秒）。 */
};

/**
 * @struct TaskSetting
 * @brief 单个任务的配置设置。
 *
 * 该结构体封装了配置任务所需的所有设置，包括相关节点、定时器设置、系统设置和启动行为。
 */
struct TaskSetting {
  std::string task_name;             /**< 任务的名称标识符。 */
  std::vector<NodeConfig> node_list; /**< 与任务关联的节点列表。 */
  TimerSetting timer_setting;        /**< 任务的定时器设置。 */
  SystemSetting system_setting;      /**< 任务的系统设置。 */
  LaunchSetting launch_setting;      /**< 任务的启动设置。 */
};

/**
 * @struct GroupTaskSetting
 * @brief 组任务的配置设置。
 *
 * 该结构体定义了特定于组任务的设置，包括强制初始化节点和预初始化节点。
 */
struct GroupTaskSetting {
  std::string task_name;                    /**< 组任务的名称标识符。 */
  std::vector<std::string> force_init_node; /**< 组任务需强制初始化的节点名称列表。 */
  std::vector<std::string> pre_node;        /**< 启动组任务前需初始化的节点名称列表。 */
};

/**
 * @struct GroupSetting
 * @brief 任务组的配置设置。
 *
 * 该结构体封装了一组任务的设置，包括组名称和组内的任务列表。
 */
struct GroupSetting {
  std::string group_name;                                      /**< 组的名称标识符。 */
  std::unordered_map<std::string, GroupTaskSetting> task_list; /**< 任务名称与其对应组任务设置的映射。 */
};

/**
 * @struct TaskList
 * @brief 按组类型分类的任务列表。
 *
 * 该结构体保存了组织成常驻组和待命组的任务设置。
 */
struct TaskList {
  std::unordered_map<std::string, TaskSetting> resident_group; /**< 常驻组任务名称与其设置的映射。 */
  std::unordered_map<std::string, TaskSetting> standby_group;  /**< 待命组任务名称与其设置的映射。 */
};

/**
 * @struct ExecuterSetting
 * @brief 执行器的配置设置。
 *
 * 该结构体定义了配置执行器所需的设置，包括包名称、定时器设置、系统设置以及优先级和CPU亲和性的标志。
 */
struct ExecuterSetting {
  std::string package_name;          /**< 与执行器关联的包名称。 */
  TimerSetting timer_setting;        /**< 执行器的定时器设置。 */
  SystemSetting system_setting;      /**< 执行器的系统设置。 */
  SystemSetting idle_system_setting; /**< 执行器空闲时的系统设置。 */
  bool all_priority_enable;          /**< 标志，指示是否启用所有优先级。 */
  bool all_cpu_affinity_enable;      /**< 标志，指示是否启用所有CPU亲和性。 */
};

/**
 * @struct ExecuterConfig
 * @brief 执行器的全面配置。
 *
 * 该结构体封装了执行器的完整配置，包括执行器设置、任务列表和独占任务组。
 */
struct ExecuterConfig {
  ExecuterSetting executer_setting;                                   /**< 执行器的设置。 */
  TaskList task_list;                                                 /**< 按组类型分类的任务列表。 */
  std::unordered_map<std::string, GroupSetting> exclusive_task_group; /**< 组名称与其对应的独占任务组设置的映射。 */
};

}  // namespace ocm