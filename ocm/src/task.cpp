

#include "task/task.hpp"

namespace ocm {

Task::Task(const TaskSetting& task_setting, const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>>& node_list, bool all_priority_enable,
           bool all_cpu_affinity_enable)
    : TaskBase(task_setting.task_name, task_setting.timer_setting.timer_type, static_cast<double>(task_setting.launch_setting.delay),
               all_priority_enable, all_cpu_affinity_enable),
      task_setting_(task_setting),
      node_list_(node_list) {
  SetPeriod(task_setting_.timer_setting.period);  // 根据配置设置任务的执行周期

  for (const auto& node : task_setting_.node_list) {
    node_output_flag_[node.node_name] = node.output_enable;  // 设置节点输出标志
    node_init_flag_[node.node_name] = false;                 // 初始化节点初始化标志为假
  }
}

void Task::Init() {
  for (auto& node : node_init_flag_) {
    node.second = true;  // 将所有节点的初始化标志设置为真
  }
}

std::set<std::string> Task::Init(const std::set<std::string>& init_node_list) {
  std::set<std::string> init_node_list_result;
  for (auto& node : node_init_flag_) {
    if (init_node_list.find(node.first) != init_node_list.end()) {
      node.second = true;                        // 设置节点初始化标志为真
      init_node_list_result.insert(node.first);  // 将成功初始化的节点名称添加到结果集中
    }
  }
  return init_node_list_result;  // 返回成功初始化的节点名称集合
}

void Task::Run() {
  for (auto& node : *node_list_) {
    const auto& node_name = node->GetNodeName();
    if (!node->GetIsConstruct()) {
      node->Construct();           // 构造节点
      node->SetIsConstruct(true);  // 设置节点构造标志
    }
    if (node_init_flag_.at(node_name)) {
      node->Init();                           // 初始化节点
      node_init_flag_.at(node_name) = false;  // 重置节点初始化标志
    }
    node->Execute();  // 执行节点
    if (node_output_flag_[node_name]) {
      node->Output();  // 输出节点数据
    }
    node->SetState(NodeState::RUNNING);  // 设置节点状态为运行中
  }
}

const TaskSetting& Task::GetTaskSetting() const { return task_setting_; }  // 返回任务的配置设置

}  // namespace ocm
