/*
 * Automatically generate files, manual modification is strictly prohibited!
 */
#pragma once
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

namespace ocm {
namespace auto_TaskConfig {
namespace auto_ExecuterSetting {
namespace auto_TimerSetting {
class TimerSetting {
 public:
  TimerSetting() = default;
  ~TimerSetting() = default;

  TimerSetting(const TimerSetting&) = default;
  TimerSetting& operator=(const TimerSetting&) = default;

  TimerSetting(TimerSetting&&) = default;
  TimerSetting& operator=(TimerSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["timer_type"]) timer_type_ = auto_yaml_node["timer_type"].as<std::string>();
    if (auto_yaml_node["period"]) period_ = auto_yaml_node["period"].as<double>();
  }

  std::string TimerType() const { return timer_type_; }

  double Period() const { return period_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "TimerSetting:" << std::endl;
    std::cout << indent << "    timer_type_: " << timer_type_ << std::endl;
    std::cout << indent << "    period_: " << period_ << std::endl;
  }

 private:
  std::string timer_type_;
  double period_;
};

}  // namespace auto_TimerSetting
}  // namespace auto_ExecuterSetting
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_ExecuterSetting {
namespace auto_SystemSetting {
class SystemSetting {
 public:
  SystemSetting() = default;
  ~SystemSetting() = default;

  SystemSetting(const SystemSetting&) = default;
  SystemSetting& operator=(const SystemSetting&) = default;

  SystemSetting(SystemSetting&&) = default;
  SystemSetting& operator=(SystemSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["priority"]) priority_ = auto_yaml_node["priority"].as<double>();
    if (auto_yaml_node["executer_cpu_affinity"]) {
      executer_cpu_affinity_.clear();
      for (auto& item : auto_yaml_node["executer_cpu_affinity"]) {
        executer_cpu_affinity_.push_back(item.as<double>());
      }
    }
  }

  double Priority() const { return priority_; }

  std::vector<double> ExecuterCpuAffinity() const { return executer_cpu_affinity_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "SystemSetting:" << std::endl;
    std::cout << indent << "    priority_: " << priority_ << std::endl;
    std::cout << indent << "    executer_cpu_affinity_: [" << std::endl;
    for (const auto& item : executer_cpu_affinity_) {
      std::cout << indent << "        " << item << std::endl;
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  double priority_;
  std::vector<double> executer_cpu_affinity_;
};

}  // namespace auto_SystemSetting
}  // namespace auto_ExecuterSetting
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_ExecuterSetting {
namespace auto_IdleSystemSetting {
class IdleSystemSetting {
 public:
  IdleSystemSetting() = default;
  ~IdleSystemSetting() = default;

  IdleSystemSetting(const IdleSystemSetting&) = default;
  IdleSystemSetting& operator=(const IdleSystemSetting&) = default;

  IdleSystemSetting(IdleSystemSetting&&) = default;
  IdleSystemSetting& operator=(IdleSystemSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["priority"]) priority_ = auto_yaml_node["priority"].as<double>();
    if (auto_yaml_node["cpu_affinity"]) {
      cpu_affinity_.clear();
      for (auto& item : auto_yaml_node["cpu_affinity"]) {
        cpu_affinity_.push_back(item.as<double>());
      }
    }
  }

  double Priority() const { return priority_; }

  std::vector<double> CpuAffinity() const { return cpu_affinity_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "IdleSystemSetting:" << std::endl;
    std::cout << indent << "    priority_: " << priority_ << std::endl;
    std::cout << indent << "    cpu_affinity_: [" << std::endl;
    for (const auto& item : cpu_affinity_) {
      std::cout << indent << "        " << item << std::endl;
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  double priority_;
  std::vector<double> cpu_affinity_;
};

}  // namespace auto_IdleSystemSetting
}  // namespace auto_ExecuterSetting
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_ExecuterSetting {
class ExecuterSetting {
 public:
  ExecuterSetting() = default;
  ~ExecuterSetting() = default;

  ExecuterSetting(const ExecuterSetting&) = default;
  ExecuterSetting& operator=(const ExecuterSetting&) = default;

  ExecuterSetting(ExecuterSetting&&) = default;
  ExecuterSetting& operator=(ExecuterSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["timer_setting"]) timer_setting_.update_from_yaml(auto_yaml_node["timer_setting"]);
    if (auto_yaml_node["system_setting"]) system_setting_.update_from_yaml(auto_yaml_node["system_setting"]);
    if (auto_yaml_node["idle_system_setting"]) idle_system_setting_.update_from_yaml(auto_yaml_node["idle_system_setting"]);
    if (auto_yaml_node["all_priority_enable"]) all_priority_enable_ = auto_yaml_node["all_priority_enable"].as<bool>();
    if (auto_yaml_node["all_cpu_affinity_enable"]) all_cpu_affinity_enable_ = auto_yaml_node["all_cpu_affinity_enable"].as<bool>();
  }

  const auto_TaskConfig::auto_ExecuterSetting::auto_TimerSetting::TimerSetting& TimerSetting() const { return timer_setting_; }

  const auto_TaskConfig::auto_ExecuterSetting::auto_SystemSetting::SystemSetting& SystemSetting() const { return system_setting_; }

  const auto_TaskConfig::auto_ExecuterSetting::auto_IdleSystemSetting::IdleSystemSetting& IdleSystemSetting() const { return idle_system_setting_; }

  bool AllPriorityEnable() const { return all_priority_enable_; }

  bool AllCpuAffinityEnable() const { return all_cpu_affinity_enable_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "ExecuterSetting:" << std::endl;
    std::cout << indent << "    timer_setting_:" << std::endl;
    timer_setting_.print(indent_level + 1);
    std::cout << indent << "    system_setting_:" << std::endl;
    system_setting_.print(indent_level + 1);
    std::cout << indent << "    idle_system_setting_:" << std::endl;
    idle_system_setting_.print(indent_level + 1);
    std::cout << indent << "    all_priority_enable_: " << all_priority_enable_ << std::endl;
    std::cout << indent << "    all_cpu_affinity_enable_: " << all_cpu_affinity_enable_ << std::endl;
  }

 private:
  auto_TaskConfig::auto_ExecuterSetting::auto_TimerSetting::TimerSetting timer_setting_;
  auto_TaskConfig::auto_ExecuterSetting::auto_SystemSetting::SystemSetting system_setting_;
  auto_TaskConfig::auto_ExecuterSetting::auto_IdleSystemSetting::IdleSystemSetting idle_system_setting_;
  bool all_priority_enable_;
  bool all_cpu_affinity_enable_;
};

}  // namespace auto_ExecuterSetting
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_NodeList {
class NodeList {
 public:
  NodeList() = default;
  ~NodeList() = default;

  NodeList(const NodeList&) = default;
  NodeList& operator=(const NodeList&) = default;

  NodeList(NodeList&&) = default;
  NodeList& operator=(NodeList&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["node_name"]) node_name_ = auto_yaml_node["node_name"].as<std::string>();
    if (auto_yaml_node["output_enable"]) output_enable_ = auto_yaml_node["output_enable"].as<bool>();
  }

  std::string NodeName() const { return node_name_; }

  bool OutputEnable() const { return output_enable_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "NodeList:" << std::endl;
    std::cout << indent << "    node_name_: " << node_name_ << std::endl;
    std::cout << indent << "    output_enable_: " << output_enable_ << std::endl;
  }

 private:
  std::string node_name_;
  bool output_enable_;
};

}  // namespace auto_NodeList
}  // namespace auto_ResidentGroup
}  // namespace auto_TaskList
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_TimerSetting {
class TimerSetting {
 public:
  TimerSetting() = default;
  ~TimerSetting() = default;

  TimerSetting(const TimerSetting&) = default;
  TimerSetting& operator=(const TimerSetting&) = default;

  TimerSetting(TimerSetting&&) = default;
  TimerSetting& operator=(TimerSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["timer_type"]) timer_type_ = auto_yaml_node["timer_type"].as<std::string>();
    if (auto_yaml_node["period"]) period_ = auto_yaml_node["period"].as<double>();
  }

  std::string TimerType() const { return timer_type_; }

  double Period() const { return period_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "TimerSetting:" << std::endl;
    std::cout << indent << "    timer_type_: " << timer_type_ << std::endl;
    std::cout << indent << "    period_: " << period_ << std::endl;
  }

 private:
  std::string timer_type_;
  double period_;
};

}  // namespace auto_TimerSetting
}  // namespace auto_ResidentGroup
}  // namespace auto_TaskList
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_SystemSetting {
class SystemSetting {
 public:
  SystemSetting() = default;
  ~SystemSetting() = default;

  SystemSetting(const SystemSetting&) = default;
  SystemSetting& operator=(const SystemSetting&) = default;

  SystemSetting(SystemSetting&&) = default;
  SystemSetting& operator=(SystemSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["priority"]) priority_ = auto_yaml_node["priority"].as<double>();
    if (auto_yaml_node["cpu_affinity"]) {
      cpu_affinity_.clear();
      for (auto& item : auto_yaml_node["cpu_affinity"]) {
        cpu_affinity_.push_back(item.as<double>());
      }
    }
  }

  double Priority() const { return priority_; }

  std::vector<double> CpuAffinity() const { return cpu_affinity_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "SystemSetting:" << std::endl;
    std::cout << indent << "    priority_: " << priority_ << std::endl;
    std::cout << indent << "    cpu_affinity_: [" << std::endl;
    for (const auto& item : cpu_affinity_) {
      std::cout << indent << "        " << item << std::endl;
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  double priority_;
  std::vector<double> cpu_affinity_;
};

}  // namespace auto_SystemSetting
}  // namespace auto_ResidentGroup
}  // namespace auto_TaskList
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_LaunchSetting {
class LaunchSetting {
 public:
  LaunchSetting() = default;
  ~LaunchSetting() = default;

  LaunchSetting(const LaunchSetting&) = default;
  LaunchSetting& operator=(const LaunchSetting&) = default;

  LaunchSetting(LaunchSetting&&) = default;
  LaunchSetting& operator=(LaunchSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["pre_node"]) {
      pre_node_.clear();
      for (auto& item : auto_yaml_node["pre_node"]) {
        pre_node_.push_back(item.as<std::string>());
      }
    }
    if (auto_yaml_node["delay"]) delay_ = auto_yaml_node["delay"].as<double>();
  }

  std::vector<std::string> PreNode() const { return pre_node_; }

  double Delay() const { return delay_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "LaunchSetting:" << std::endl;
    std::cout << indent << "    pre_node_: [" << std::endl;
    for (const auto& item : pre_node_) {
      std::cout << indent << "        " << item << std::endl;
    }
    std::cout << indent << "    ]" << std::endl;
    std::cout << indent << "    delay_: " << delay_ << std::endl;
  }

 private:
  std::vector<std::string> pre_node_;
  double delay_;
};

}  // namespace auto_LaunchSetting
}  // namespace auto_ResidentGroup
}  // namespace auto_TaskList
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_ResidentGroup {
class ResidentGroup {
 public:
  ResidentGroup() = default;
  ~ResidentGroup() = default;

  ResidentGroup(const ResidentGroup&) = default;
  ResidentGroup& operator=(const ResidentGroup&) = default;

  ResidentGroup(ResidentGroup&&) = default;
  ResidentGroup& operator=(ResidentGroup&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["task_name"]) task_name_ = auto_yaml_node["task_name"].as<std::string>();
    if (auto_yaml_node["node_list"]) {
      node_list_.clear();
      for (auto& item : auto_yaml_node["node_list"]) {
        auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_NodeList::NodeList elem;
        elem.update_from_yaml(item);
        node_list_.push_back(elem);
      }
    }
    if (auto_yaml_node["timer_setting"]) timer_setting_.update_from_yaml(auto_yaml_node["timer_setting"]);
    if (auto_yaml_node["system_setting"]) system_setting_.update_from_yaml(auto_yaml_node["system_setting"]);
    if (auto_yaml_node["launch_setting"]) launch_setting_.update_from_yaml(auto_yaml_node["launch_setting"]);
  }

  std::string TaskName() const { return task_name_; }

  std::vector<auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_NodeList::NodeList> NodeList() const { return node_list_; }

  const auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_TimerSetting::TimerSetting& TimerSetting() const { return timer_setting_; }

  const auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_SystemSetting::SystemSetting& SystemSetting() const { return system_setting_; }

  const auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_LaunchSetting::LaunchSetting& LaunchSetting() const { return launch_setting_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "ResidentGroup:" << std::endl;
    std::cout << indent << "    task_name_: " << task_name_ << std::endl;
    std::cout << indent << "    node_list_: [" << std::endl;
    for (const auto& item : node_list_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
    std::cout << indent << "    timer_setting_:" << std::endl;
    timer_setting_.print(indent_level + 1);
    std::cout << indent << "    system_setting_:" << std::endl;
    system_setting_.print(indent_level + 1);
    std::cout << indent << "    launch_setting_:" << std::endl;
    launch_setting_.print(indent_level + 1);
  }

 private:
  std::string task_name_;
  std::vector<auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_NodeList::NodeList> node_list_;
  auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_TimerSetting::TimerSetting timer_setting_;
  auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_SystemSetting::SystemSetting system_setting_;
  auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_LaunchSetting::LaunchSetting launch_setting_;
};

}  // namespace auto_ResidentGroup
}  // namespace auto_TaskList
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_StandbyGroup {
namespace auto_NodeList {
class NodeList {
 public:
  NodeList() = default;
  ~NodeList() = default;

  NodeList(const NodeList&) = default;
  NodeList& operator=(const NodeList&) = default;

  NodeList(NodeList&&) = default;
  NodeList& operator=(NodeList&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["node_name"]) node_name_ = auto_yaml_node["node_name"].as<std::string>();
    if (auto_yaml_node["output_enable"]) output_enable_ = auto_yaml_node["output_enable"].as<bool>();
  }

  std::string NodeName() const { return node_name_; }

  bool OutputEnable() const { return output_enable_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "NodeList:" << std::endl;
    std::cout << indent << "    node_name_: " << node_name_ << std::endl;
    std::cout << indent << "    output_enable_: " << output_enable_ << std::endl;
  }

 private:
  std::string node_name_;
  bool output_enable_;
};

}  // namespace auto_NodeList
}  // namespace auto_StandbyGroup
}  // namespace auto_TaskList
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_StandbyGroup {
namespace auto_TimerSetting {
class TimerSetting {
 public:
  TimerSetting() = default;
  ~TimerSetting() = default;

  TimerSetting(const TimerSetting&) = default;
  TimerSetting& operator=(const TimerSetting&) = default;

  TimerSetting(TimerSetting&&) = default;
  TimerSetting& operator=(TimerSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["timer_type"]) timer_type_ = auto_yaml_node["timer_type"].as<std::string>();
    if (auto_yaml_node["period"]) period_ = auto_yaml_node["period"].as<double>();
  }

  std::string TimerType() const { return timer_type_; }

  double Period() const { return period_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "TimerSetting:" << std::endl;
    std::cout << indent << "    timer_type_: " << timer_type_ << std::endl;
    std::cout << indent << "    period_: " << period_ << std::endl;
  }

 private:
  std::string timer_type_;
  double period_;
};

}  // namespace auto_TimerSetting
}  // namespace auto_StandbyGroup
}  // namespace auto_TaskList
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_StandbyGroup {
namespace auto_SystemSetting {
class SystemSetting {
 public:
  SystemSetting() = default;
  ~SystemSetting() = default;

  SystemSetting(const SystemSetting&) = default;
  SystemSetting& operator=(const SystemSetting&) = default;

  SystemSetting(SystemSetting&&) = default;
  SystemSetting& operator=(SystemSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["priority"]) priority_ = auto_yaml_node["priority"].as<double>();
    if (auto_yaml_node["cpu_affinity"]) {
      cpu_affinity_.clear();
      for (auto& item : auto_yaml_node["cpu_affinity"]) {
        cpu_affinity_.push_back(item.as<double>());
      }
    }
  }

  double Priority() const { return priority_; }

  std::vector<double> CpuAffinity() const { return cpu_affinity_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "SystemSetting:" << std::endl;
    std::cout << indent << "    priority_: " << priority_ << std::endl;
    std::cout << indent << "    cpu_affinity_: [" << std::endl;
    for (const auto& item : cpu_affinity_) {
      std::cout << indent << "        " << item << std::endl;
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  double priority_;
  std::vector<double> cpu_affinity_;
};

}  // namespace auto_SystemSetting
}  // namespace auto_StandbyGroup
}  // namespace auto_TaskList
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_StandbyGroup {
class StandbyGroup {
 public:
  StandbyGroup() = default;
  ~StandbyGroup() = default;

  StandbyGroup(const StandbyGroup&) = default;
  StandbyGroup& operator=(const StandbyGroup&) = default;

  StandbyGroup(StandbyGroup&&) = default;
  StandbyGroup& operator=(StandbyGroup&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["task_name"]) task_name_ = auto_yaml_node["task_name"].as<std::string>();
    if (auto_yaml_node["node_list"]) {
      node_list_.clear();
      for (auto& item : auto_yaml_node["node_list"]) {
        auto_TaskConfig::auto_TaskList::auto_StandbyGroup::auto_NodeList::NodeList elem;
        elem.update_from_yaml(item);
        node_list_.push_back(elem);
      }
    }
    if (auto_yaml_node["timer_setting"]) timer_setting_.update_from_yaml(auto_yaml_node["timer_setting"]);
    if (auto_yaml_node["system_setting"]) system_setting_.update_from_yaml(auto_yaml_node["system_setting"]);
  }

  std::string TaskName() const { return task_name_; }

  std::vector<auto_TaskConfig::auto_TaskList::auto_StandbyGroup::auto_NodeList::NodeList> NodeList() const { return node_list_; }

  const auto_TaskConfig::auto_TaskList::auto_StandbyGroup::auto_TimerSetting::TimerSetting& TimerSetting() const { return timer_setting_; }

  const auto_TaskConfig::auto_TaskList::auto_StandbyGroup::auto_SystemSetting::SystemSetting& SystemSetting() const { return system_setting_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "StandbyGroup:" << std::endl;
    std::cout << indent << "    task_name_: " << task_name_ << std::endl;
    std::cout << indent << "    node_list_: [" << std::endl;
    for (const auto& item : node_list_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
    std::cout << indent << "    timer_setting_:" << std::endl;
    timer_setting_.print(indent_level + 1);
    std::cout << indent << "    system_setting_:" << std::endl;
    system_setting_.print(indent_level + 1);
  }

 private:
  std::string task_name_;
  std::vector<auto_TaskConfig::auto_TaskList::auto_StandbyGroup::auto_NodeList::NodeList> node_list_;
  auto_TaskConfig::auto_TaskList::auto_StandbyGroup::auto_TimerSetting::TimerSetting timer_setting_;
  auto_TaskConfig::auto_TaskList::auto_StandbyGroup::auto_SystemSetting::SystemSetting system_setting_;
};

}  // namespace auto_StandbyGroup
}  // namespace auto_TaskList
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_TaskList {
class TaskList {
 public:
  TaskList() = default;
  ~TaskList() = default;

  TaskList(const TaskList&) = default;
  TaskList& operator=(const TaskList&) = default;

  TaskList(TaskList&&) = default;
  TaskList& operator=(TaskList&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["resident_group"]) {
      resident_group_.clear();
      for (auto& item : auto_yaml_node["resident_group"]) {
        auto_TaskConfig::auto_TaskList::auto_ResidentGroup::ResidentGroup elem;
        elem.update_from_yaml(item);
        resident_group_.push_back(elem);
      }
    }
    if (auto_yaml_node["standby_group"]) {
      standby_group_.clear();
      for (auto& item : auto_yaml_node["standby_group"]) {
        auto_TaskConfig::auto_TaskList::auto_StandbyGroup::StandbyGroup elem;
        elem.update_from_yaml(item);
        standby_group_.push_back(elem);
      }
    }
  }

  std::vector<auto_TaskConfig::auto_TaskList::auto_ResidentGroup::ResidentGroup> ResidentGroup() const { return resident_group_; }

  std::vector<auto_TaskConfig::auto_TaskList::auto_StandbyGroup::StandbyGroup> StandbyGroup() const { return standby_group_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "TaskList:" << std::endl;
    std::cout << indent << "    resident_group_: [" << std::endl;
    for (const auto& item : resident_group_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
    std::cout << indent << "    standby_group_: [" << std::endl;
    for (const auto& item : standby_group_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  std::vector<auto_TaskConfig::auto_TaskList::auto_ResidentGroup::ResidentGroup> resident_group_;
  std::vector<auto_TaskConfig::auto_TaskList::auto_StandbyGroup::StandbyGroup> standby_group_;
};

}  // namespace auto_TaskList
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_ExclusiveTaskGroup {
namespace auto_TaskList {
class TaskList {
 public:
  TaskList() = default;
  ~TaskList() = default;

  TaskList(const TaskList&) = default;
  TaskList& operator=(const TaskList&) = default;

  TaskList(TaskList&&) = default;
  TaskList& operator=(TaskList&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["task_name"]) task_name_ = auto_yaml_node["task_name"].as<std::string>();
    if (auto_yaml_node["force_init_node"]) {
      force_init_node_.clear();
      for (auto& item : auto_yaml_node["force_init_node"]) {
        force_init_node_.push_back(item.as<std::string>());
      }
    }
    if (auto_yaml_node["pre_node"]) {
      pre_node_.clear();
      for (auto& item : auto_yaml_node["pre_node"]) {
        pre_node_.push_back(item.as<std::string>());
      }
    }
  }

  std::string TaskName() const { return task_name_; }

  std::vector<std::string> ForceInitNode() const { return force_init_node_; }

  std::vector<std::string> PreNode() const { return pre_node_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "TaskList:" << std::endl;
    std::cout << indent << "    task_name_: " << task_name_ << std::endl;
    std::cout << indent << "    force_init_node_: [" << std::endl;
    for (const auto& item : force_init_node_) {
      std::cout << indent << "        " << item << std::endl;
    }
    std::cout << indent << "    ]" << std::endl;
    std::cout << indent << "    pre_node_: [" << std::endl;
    for (const auto& item : pre_node_) {
      std::cout << indent << "        " << item << std::endl;
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  std::string task_name_;
  std::vector<std::string> force_init_node_;
  std::vector<std::string> pre_node_;
};

}  // namespace auto_TaskList
}  // namespace auto_ExclusiveTaskGroup
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
namespace auto_ExclusiveTaskGroup {
class ExclusiveTaskGroup {
 public:
  ExclusiveTaskGroup() = default;
  ~ExclusiveTaskGroup() = default;

  ExclusiveTaskGroup(const ExclusiveTaskGroup&) = default;
  ExclusiveTaskGroup& operator=(const ExclusiveTaskGroup&) = default;

  ExclusiveTaskGroup(ExclusiveTaskGroup&&) = default;
  ExclusiveTaskGroup& operator=(ExclusiveTaskGroup&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["group_name"]) group_name_ = auto_yaml_node["group_name"].as<std::string>();
    if (auto_yaml_node["task_list"]) {
      task_list_.clear();
      for (auto& item : auto_yaml_node["task_list"]) {
        auto_TaskConfig::auto_ExclusiveTaskGroup::auto_TaskList::TaskList elem;
        elem.update_from_yaml(item);
        task_list_.push_back(elem);
      }
    }
  }

  std::string GroupName() const { return group_name_; }

  std::vector<auto_TaskConfig::auto_ExclusiveTaskGroup::auto_TaskList::TaskList> TaskList() const { return task_list_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "ExclusiveTaskGroup:" << std::endl;
    std::cout << indent << "    group_name_: " << group_name_ << std::endl;
    std::cout << indent << "    task_list_: [" << std::endl;
    for (const auto& item : task_list_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  std::string group_name_;
  std::vector<auto_TaskConfig::auto_ExclusiveTaskGroup::auto_TaskList::TaskList> task_list_;
};

}  // namespace auto_ExclusiveTaskGroup
}  // namespace auto_TaskConfig

namespace auto_TaskConfig {
class TaskConfig {
 public:
  TaskConfig() = default;
  ~TaskConfig() = default;

  TaskConfig(const TaskConfig&) = default;
  TaskConfig& operator=(const TaskConfig&) = default;

  TaskConfig(TaskConfig&&) = default;
  TaskConfig& operator=(TaskConfig&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["executer_setting"]) executer_setting_.update_from_yaml(auto_yaml_node["executer_setting"]);
    if (auto_yaml_node["task_list"]) task_list_.update_from_yaml(auto_yaml_node["task_list"]);
    if (auto_yaml_node["exclusive_task_group"]) {
      exclusive_task_group_.clear();
      for (auto& item : auto_yaml_node["exclusive_task_group"]) {
        auto_TaskConfig::auto_ExclusiveTaskGroup::ExclusiveTaskGroup elem;
        elem.update_from_yaml(item);
        exclusive_task_group_.push_back(elem);
      }
    }
  }

  const auto_TaskConfig::auto_ExecuterSetting::ExecuterSetting& ExecuterSetting() const { return executer_setting_; }

  const auto_TaskConfig::auto_TaskList::TaskList& TaskList() const { return task_list_; }

  std::vector<auto_TaskConfig::auto_ExclusiveTaskGroup::ExclusiveTaskGroup> ExclusiveTaskGroup() const { return exclusive_task_group_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "TaskConfig:" << std::endl;
    std::cout << indent << "    executer_setting_:" << std::endl;
    executer_setting_.print(indent_level + 1);
    std::cout << indent << "    task_list_:" << std::endl;
    task_list_.print(indent_level + 1);
    std::cout << indent << "    exclusive_task_group_: [" << std::endl;
    for (const auto& item : exclusive_task_group_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  auto_TaskConfig::auto_ExecuterSetting::ExecuterSetting executer_setting_;
  auto_TaskConfig::auto_TaskList::TaskList task_list_;
  std::vector<auto_TaskConfig::auto_ExclusiveTaskGroup::ExclusiveTaskGroup> exclusive_task_group_;
};

}  // namespace auto_TaskConfig

class ConfigCollect {
 public:
  // 获取单例实例
  static ConfigCollect& getInstance() {
    static ConfigCollect instance;
    return instance;
  }

  // 删除拷贝构造函数和赋值操作符
  ConfigCollect(const ConfigCollect&) = delete;
  ConfigCollect& operator=(const ConfigCollect&) = delete;

  // 删除移动构造函数和赋值操作符
  ConfigCollect(ConfigCollect&&) = delete;
  ConfigCollect& operator=(ConfigCollect&&) = delete;

  // 根据名称更新对应的配置
  void update_from_yaml(const std::string& name, const std::string& base_path) {
    if (name.empty()) {
      return;
    }
    bool matched = false;
    if (name == "task_config") {
      update_from_yaml_task_config(base_path);
      matched = true;
    }
    if (!matched) {
      // 未找到匹配的配置项
      std::cerr << "[ConfigCollect] No matching configuration for '" << name << "'" << std::endl;
    }
  }

  // 更新所有配置
  void update_from_yaml_all(const std::string& base_path) { update_from_yaml_task_config(base_path); }

  // 打印所有配置
  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "task_config:" << std::endl;
    {
      std::shared_lock<std::shared_mutex> lock(m_task_config);
      task_config.print(indent_level + 1);
    }
  }

  // 获取各个配置的实例
  auto_TaskConfig::TaskConfig& get_task_config() {
    std::shared_lock<std::shared_mutex> lock(m_task_config);
    return task_config;
  }

 private:
  // 私有构造函数，防止外部实例化
  ConfigCollect() = default;

  auto_TaskConfig::TaskConfig task_config;
  mutable std::shared_mutex m_task_config;

  // 更新 auto_TaskConfig::TaskConfig 配置
  void update_from_yaml_task_config(const std::string& base_path) {
    YAML::Node auto_yaml_node;
    // 加载 YAML 文件
    auto_yaml_node = YAML::LoadFile(base_path + "/task_config.yaml");
    {
      std::unique_lock<std::shared_mutex> lock(m_task_config);
      task_config.update_from_yaml(auto_yaml_node);
    }
  }
};

}  // namespace ocm
