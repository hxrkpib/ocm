
#pragma once

#include <atomic>
#include <string>
#include "common/enum.hpp"
#include "debug_anywhere/debug_anywhere.hpp"
#include "log_anywhere/log_anywhere.hpp"

namespace ocm {

/**
 * @class NodeBase
 * @brief 抽象基类，表示OpenRobot OCM中的通用节点。
 *
 * `NodeBase` 类作为OpenRobot操作控制模块（OCM）中所有节点类型的接口。它定义了每个节点必须实现的基本生命周期方法，如初始化、执行和状态转换。
 * 该类确保所有派生节点遵循一致的接口，从而实现OCM框架内的无缝集成和管理。
 *
 * @note 此类是抽象的，不能被直接实例化。必须由具体的节点实现继承，并为纯虚方法提供定义。
 */
class NodeBase {
 public:
  /**
   * @brief 使用给定的节点名称构造一个新的 NodeBase 实例。
   *
   * 初始化节点的名称并将其初始状态设置为 `NodeState::INIT`。
   *
   * @param node_name 节点的唯一名称标识符。
   */
  NodeBase(const std::string& node_name);

  /**
   * @brief 删除的拷贝赋值操作符。
   *
   * 防止拷贝 `NodeBase` 实例以保持唯一所有权语义。
   */
  NodeBase& operator=(const NodeBase&) = delete;

  /**
   * @brief 删除的移动构造函数。
   *
   * 防止移动 `NodeBase` 实例以保持唯一所有权语义。
   */
  NodeBase(NodeBase&&) = delete;

  /**
   * @brief 虚析构函数。
   *
   * 确保通过基类指针删除实例时，派生类的析构函数被正确调用。
   */
  virtual ~NodeBase() = default;

  /**
   * @brief 构造节点。
   *
   * 纯虚方法，必须由派生类重写以执行节点的任何必要构造步骤。
   *
   * @note 此方法在节点开始执行之前调用。
   */
  virtual void Construct() = 0;

  /**
   * @brief 初始化节点。
   *
   * 纯虚方法，必须由派生类重写以执行节点的任何必要初始化步骤。
   *
   * @note 此方法在节点开始执行之前调用。
   */
  virtual void Init() = 0;

  /**
   * @brief 执行节点的主要功能。
   *
   * 纯虚方法，必须由派生类重写以定义节点的主要执行逻辑。
   *
   * @note 此方法旨在定期调用或响应事件调用。
   */
  virtual void Execute() = 0;

  /**
   * @brief 输出节点的状态或数据。
   *
   * 纯虚方法，必须由派生类重写以处理任何输出操作，例如向其他组件发送数据或记录日志。
   */
  virtual void Output() = 0;

  /**
   * @brief 尝试进入节点的活动状态。
   *
   * 纯虚方法，必须由派生类重写以定义节点可以过渡到活动状态的条件。
   *
   * @return 如果节点成功进入活动状态，则返回 `true`；否则，返回 `false`。
   */
  virtual bool TryEnter() = 0;

  /**
   * @brief 尝试退出节点的活动状态。
   *
   * 纯虚方法，必须由派生类重写以定义节点可以过渡出活动状态的条件。
   *
   * @return 如果节点成功退出活动状态，则返回 `true`；否则，返回 `false`。
   */
  virtual bool TryExit() = 0;

  /**
   * @brief 虚拟方法，可以由派生类重写以处理节点退出活动状态后的任何后退出操作。
   */
  virtual void AfterExit() = 0;

  /**
   * @brief 设置节点的状态。
   *
   * @param state 要为节点设置的新状态。
   */
  void SetState(NodeState state);

  /**
   * @brief 获取节点的当前状态。
   *
   * @return 节点当前的 `NodeState`。
   */
  NodeState GetState() const;

  /**
   * @brief 获取节点的名称。
   *
   * @return 节点名称字符串的常量引用。
   */
  const std::string& GetNodeName() const;

  /**
   * @brief 获取节点是否构造。
   *
   * @return 节点是否构造的布尔值。
   */
  bool GetIsConstruct() const;

  /**
   * @brief 设置节点是否构造。
   *
   * @param is_construct 节点是否构造的布尔值。
   */
  void SetIsConstruct(bool is_construct);

  std::shared_ptr<spdlog::logger> log_anywhere_ = GetLogger();  // 获取日志实例
  DebugAnywhere& debug_anywhere_ = DebugAnywhere::getInstance();

 private:
  bool is_construct_ = false;
  std::string node_name_;        /**< 节点的唯一名称标识符 */
  std::atomic<NodeState> state_; /**< 节点的当前状态，通过原子操作管理以确保线程安全 */
};

}  // namespace ocm
