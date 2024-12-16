// Start of Selection

#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace ocm {

/**
 * @enum NodeState
 * @brief 表示节点的状态。
 */
enum class NodeState : uint8_t {
  INIT = 0, /**< 初始化状态 */
  RUNNING,  /**< 运行状态 */
  STANDBY   /**< 待命状态 */
};

/**
 * @enum TaskState
 * @brief 表示任务的状态。
 */
enum class TaskState : uint8_t {
  INIT = 0, /**< 初始化状态 */
  RUNNING,  /**< 运行状态 */
  STANDBY   /**< 待命状态 */
};

/**
 * @enum TimerType
 * @brief 表示定时器的类型。
 */
enum class TimerType : uint8_t {
  INTERNAL_TIMER = 0, /**< 内部定时器 */
  EXTERNAL_TIMER,     /**< 外部定时器 */
  TRIGGER             /**< 触发器 */
};

/**
 * @brief 将定时器类型的字符串表示映射到对应的 `TimerType` 枚举值。
 *
 * 此映射用于将基于字符串的定时器类型标识符转换为各自的 `TimerType` 枚举。
 */
const std::unordered_map<std::string, TimerType> timer_type_map = {
    {"INTERNAL_TIMER", TimerType::INTERNAL_TIMER},
    {"EXTERNAL_TIMER", TimerType::EXTERNAL_TIMER},
    {"TRIGGER", TimerType::TRIGGER},
};

/**
 * @namespace ocm
 * @brief OpenRobot操作控制模块 (OCM) 的命名空间。
 */
}  // namespace ocm
