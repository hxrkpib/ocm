#pragma once

#include <format>
#include <memory>
#include <string>
#include <unordered_map>
#include "log_anywhere/log_anywhere.hpp"
#include "node/node.hpp"

namespace ocm {

/**
 * @class NodeMap
 * @brief 管理一组节点，允许通过名称添加和检索节点。
 *
 * NodeMap类提供了存储和管理通过唯一字符串标识的节点的功能。
 * 它确保每个节点名称在映射中是唯一的，并提供添加新节点和检索现有节点的方法。
 * 尝试添加具有重复名称的节点或检索不存在的节点将导致异常。
 *
 * @note 此类被标记为 `final` 以防止继承。
 */
class NodeMap final {
 public:
  /**
   * @brief 默认构造函数。
   *
   * 初始化一个空的 `NodeMap`。
   */
  NodeMap() = default;

  /**
   * @brief 默认析构函数。
   *
   * 清理 `NodeMap`。由于所有管理的节点都通过 `std::shared_ptr` 处理，
   * 无需显式清理。
   */
  ~NodeMap() = default;

  /**
   * @brief 向映射中添加一个新节点。
   *
   * 此方法将一个具有指定名称和指针的新节点插入到 `NodeMap` 中。
   *
   * @param node_name 节点的唯一名称标识。
   * @param node_ptr 一个指向要添加的节点的 `std::shared_ptr`。
   */
  void AddNode(const std::string& node_name, std::shared_ptr<NodeBase> node_ptr);

  /**
   * @brief 通过名称检索节点指针。
   *
   * 此方法搜索具有指定名称的节点，并返回其 `std::shared_ptr<NodeBase>` 的引用。
   * 如果节点不存在，方法将抛出 `std::runtime_error`。
   *
   * @param key 要检索的节点的名称标识。
   * @return 节点对应的 `std::shared_ptr<NodeBase>` 的常量引用。
   *
   * @throws std::runtime_error 如果在映射中未找到具有指定名称的节点。
   */
  const std::shared_ptr<NodeBase>& GetNodePtr(const std::string& key) const;

 private:
  /**
   * @brief 存储节点名称及其对应指针的内部映射。
   *
   * 键是表示节点名称的唯一字符串，值是指向 `NodeBase` 实例的 `std::shared_ptr`。
   * 这个映射确保了节点的高效检索和管理。
   */
  std::unordered_map<std::string, std::shared_ptr<NodeBase>> node_map_;
  std::shared_ptr<spdlog::logger> logger_ = GetLogger();
};

}  // namespace ocm
