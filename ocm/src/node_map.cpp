#include "node/node_map.hpp"

namespace ocm {
void NodeMap::AddNode(const std::string& node_name, std::shared_ptr<NodeBase> node_ptr) {
  // 检查节点名称是否已存在于节点映射中
  if (node_map_.find(node_name) == node_map_.end()) {
    // 将新节点添加到节点映射中
    node_map_[node_name] = node_ptr;
    // 记录节点添加的日志信息
    logger_->info("[NodeMap] Node '{}' added!", node_name);
  }
}
const std::shared_ptr<NodeBase>& NodeMap::GetNodePtr(const std::string& key) const {
  // 在节点映射中查找指定的节点
  auto it = node_map_.find(key);
  // 如果节点未找到，则抛出异常
  if (it == node_map_.end()) {
    throw std::runtime_error(std::format("[NodeMap] Node '{}' not found!", key));
  }
  // 返回找到的节点指针
  return it->second;
}
}  // namespace ocm
