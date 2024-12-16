#include "executer/executer.hpp"

#include <algorithm>
#include <chrono>
#include <thread>
#include "common/struct_type.hpp"
#include "executer/desired_group_data.hpp"

namespace ocm {

Executer::Executer(const ExecuterConfig& executer_config, const std::shared_ptr<NodeMap>& node_map, const std::string& desired_group_topic_name)
    : TaskBase(executer_config.executer_setting.package_name, executer_config.executer_setting.timer_setting.timer_type, 0.0,
               executer_config.executer_setting.all_priority_enable, executer_config.executer_setting.all_cpu_affinity_enable),
      node_map_(node_map),
      executer_config_(executer_config),
      desired_group_("empty_init"),
      current_group_("empty_init"),
      desired_group_history_("empty_init"),
      target_group_("empty_init"),
      is_transition_(false),
      all_node_exit_check_(false),
      all_node_enter_check_(false),
      task_stop_flag_(true),
      task_start_flag_(true),
      all_current_task_stop_(false),
      desired_group_topic_name_(desired_group_topic_name) {
  logger_ = GetLogger();                                                // 获取日志记录器
  desired_group_topic_lcm_ = std::make_shared<SharedMemoryTopicLcm>();  // 创建共享内存主题
  SetPeriod(executer_config_.executer_setting.timer_setting.period);    // 设置周期
  TaskStart(executer_config_.executer_setting.system_setting);          // 启动任务
}

void Executer::ExitAllTask() {
  // 停止所有常驻组任务
  for (auto& task : resident_group_task_list_) {
    task.second->TaskStop(executer_config_.executer_setting.idle_system_setting);  // 停止任务
    task.second->TaskDestroy();                                                    // 销毁任务
  }

  // 停止所有待命组任务
  for (auto& task : standby_group_task_list_) {
    task.second->TaskStop(executer_config_.executer_setting.idle_system_setting);  // 停止任务
    task.second->TaskDestroy();                                                    // 销毁任务
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待1秒
}

void Executer::CreateTask() {
  // 创建常驻组任务
  for (auto& task_setting : executer_config_.task_list.resident_group) {
    std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();

    // 获取节点列表
    for (auto& node_config : task_setting.second.node_list) {
      node_list->emplace_back(node_map_->GetNodePtr(node_config.node_name));  // 添加节点指针
    }

    // 创建任务并添加到常驻组任务列表
    resident_group_task_list_[task_setting.second.task_name] =
        std::make_shared<Task>(task_setting.second, node_list, executer_config_.executer_setting.all_priority_enable,
                               executer_config_.executer_setting.all_cpu_affinity_enable);

    logger_->info("[Executer] Task {} added.", task_setting.second.task_name);  // 记录任务添加信息
  }

  // 创建待命组任务
  for (auto& task_setting : executer_config_.task_list.standby_group) {
    std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();

    // 获取节点列表
    for (auto& node_config : task_setting.second.node_list) {
      node_list->emplace_back(node_map_->GetNodePtr(node_config.node_name));  // 添加节点指针
    }

    // 创建任务并添加到待命组任务列表
    standby_group_task_list_[task_setting.second.task_name] =
        std::make_shared<Task>(task_setting.second, node_list, executer_config_.executer_setting.all_priority_enable,
                               executer_config_.executer_setting.all_cpu_affinity_enable);

    logger_->info("[Executer] Task {} added.", task_setting.second.task_name);  // 记录任务添加信息
  }

  // 添加独占任务组
  for (auto& exclusive_task_group : executer_config_.exclusive_task_group) {
    logger_->info("[Executer] Exclusive group {} added.", exclusive_task_group.second.group_name);  // 记录独占组添加信息
    exclusive_group_set_.insert(exclusive_task_group.second.group_name);                            // 添加到独占组集合
  }
}

void Executer::InitTask() {
  std::vector<std::pair<bool, std::shared_ptr<Task>>> task_list_wait_to_start;  // 等待启动的任务列表
  std::set<std::string> task_set_wait_to_start;                                 // 等待启动的任务集合

  // 初始化等待启动的任务列表
  for (auto& task_ptr : resident_group_task_list_) {
    task_list_wait_to_start.emplace_back(std::make_pair(false, task_ptr.second));  // 添加任务指针
    task_set_wait_to_start.insert(task_ptr.second->GetTaskName());                 // 添加任务名称
  }

  // 等待所有任务启动
  while (!task_set_wait_to_start.empty()) {
    for (auto& task : task_list_wait_to_start) {
      if (!task.first) {                                                                         // 如果任务尚未启动
        bool is_pre_node_empty = task.second->GetTaskSetting().launch_setting.pre_node.empty();  // 检查前置节点是否为空
        const auto& pre_node_list = task.second->GetTaskSetting().launch_setting.pre_node;       // 获取前置节点列表

        // 检查前置节点是否准备就绪
        bool is_pre_node_ready = std::all_of(pre_node_list.begin(), pre_node_list.end(), [this](const std::string& pre_node_name) {
          return node_map_->GetNodePtr(pre_node_name)->GetState() == NodeState::RUNNING;  // 检查节点状态
        });

        // 如果前置节点为空或准备就绪，启动任务
        if (is_pre_node_empty || is_pre_node_ready) {
          task.first = true;                                                       // 标记任务为已启动
          task.second->Init();                                                     // 初始化任务
          task.second->TaskStart(task.second->GetTaskSetting().system_setting);    // 启动任务
          task_set_wait_to_start.erase(task.second->GetTaskName());                // 从等待集合中移除任务
          logger_->info("[Executer] Task {} start.", task.second->GetTaskName());  // 记录任务启动信息
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));  // 等待1毫秒
  }
}

void Executer::Run() {
  // 订阅期望组数据
  desired_group_topic_lcm_->SubscribeNoWait<DesiredGroupData>(
      desired_group_topic_name_ + "_lcm", desired_group_topic_name_ + "_lcm",
      [this](const DesiredGroupData& desired_group) { desired_group_ = desired_group.desired_group; });  // 更新期望组
  TransitionCheck();                                                                                     // 检查状态转换

  if (is_transition_) {
    Transition();  // 执行状态转换
  }
}

void Executer::TransitionCheck() {
  if (!is_transition_) {                                    // 如果当前不在转换状态
    const auto& desired_group = desired_group_.GetValue();  // 获取期望组
    const auto& current_group = current_group_.GetValue();  // 获取当前组

    if (desired_group != current_group) {                                            // 如果期望组与当前组不同
      if (exclusive_group_set_.find(desired_group) != exclusive_group_set_.end()) {  // 检查期望组是否为独占组
        target_task_set_.clear();                                                    // 清空目标任务集合
        current_task_set_.clear();                                                   // 清空当前任务集合
        target_node_set_.clear();                                                    // 清空目标节点集合
        current_node_set_.clear();                                                   // 清空当前节点集合
        enter_node_set_.clear();                                                     // 清空进入节点集合
        exit_node_set_.clear();                                                      // 清空退出节点集合

        const auto& exclusive_group_config = executer_config_.exclusive_task_group;  // 获取独占组配置
        if (exclusive_group_config.find(desired_group) != executer_config_.exclusive_task_group.end()) {
          for (auto& task_config : exclusive_group_config.at(desired_group).task_list) {
            const auto& task_name = task_config.second.task_name;
            if (standby_group_task_list_.find(task_name) != standby_group_task_list_.end()) {
              target_task_set_.insert(standby_group_task_list_.at(task_name));  // 添加目标任务
            }
            if (executer_config_.task_list.standby_group.find(task_name) != executer_config_.task_list.standby_group.end()) {
              for (auto& node : executer_config_.task_list.standby_group.at(task_name).node_list) {
                target_node_set_.insert(node.node_name);  // 添加目标节点
              }
            }
          }
        }

        if (current_group != "empty_init") {  // 如果当前组不为空初始化
          if (exclusive_group_config.find(current_group) != exclusive_group_config.end()) {
            for (auto& task_config : exclusive_group_config.at(current_group).task_list) {
              const auto& task_name = task_config.second.task_name;  // 获取任务名称
              if (standby_group_task_list_.find(task_name) != standby_group_task_list_.end()) {
                current_task_set_.insert(standby_group_task_list_.at(task_name));  // 添加当前任务
              }
              if (executer_config_.task_list.standby_group.find(task_name) != executer_config_.task_list.standby_group.end()) {
                for (auto& node : executer_config_.task_list.standby_group.at(task_name).node_list) {
                  current_node_set_.insert(node.node_name);  // 添加当前节点
                }
              }
            }
          }
        }

        // 计算退出节点和进入节点
        std::set_difference(current_node_set_.begin(), current_node_set_.end(), target_node_set_.begin(), target_node_set_.end(),
                            std::inserter(exit_node_set_, exit_node_set_.begin()));  // 计算退出节点

        std::set_difference(target_node_set_.begin(), target_node_set_.end(), current_node_set_.begin(), current_node_set_.end(),
                            std::inserter(enter_node_set_, enter_node_set_.begin()));  // 计算进入节点

        all_node_exit_check_ = false;    // 重置退出节点检查标志
        all_node_enter_check_ = false;   // 重置进入节点检查标志
        is_transition_ = true;           // 设置为转换状态
        task_stop_flag_ = true;          // 设置任务停止标志
        task_start_flag_ = true;         // 设置任务启动标志
        all_current_task_stop_ = false;  // 重置当前任务停止标志
        target_group_ = desired_group;   // 设置目标组

        logger_->info("[Executer] Transition from group {} to group {}", ColorPrint(current_group, ColorEnum::YELLOW),
                      ColorPrint(desired_group, ColorEnum::YELLOW));  // 记录转换信息
      } else {
        if (desired_group_history_ != desired_group) {  // 如果历史期望组与当前期望组不同
          desired_group_history_ = desired_group;       // 更新历史期望组
          logger_->error("[Executer] Target group {} is not an exclusive group.", ColorPrint(desired_group, ColorEnum::RED));  // 记录错误信息
        }
      }
    }
  }
}

void Executer::Transition() {
  if (all_node_exit_check_ && all_node_enter_check_) {  // 如果所有节点退出和进入检查通过
    if (task_stop_flag_) {                              // 如果任务停止标志为真
      task_stop_flag_ = false;                          // 重置任务停止标志
      for (auto& task : current_task_set_) {
        task->TaskStop(executer_config_.executer_setting.idle_system_setting);  // 停止当前任务
      }
    }

    if (all_current_task_stop_) {  // 如果所有当前任务已停止
      for (auto& node : exit_node_set_) {
        node_map_->GetNodePtr(node)->AfterExit();                   // 执行退出后操作
        node_map_->GetNodePtr(node)->SetState(NodeState::STANDBY);  // 设置节点状态为待命
      }
      std::vector<std::pair<bool, std::shared_ptr<Task>>> task_list_wait_to_start;  // 等待启动的任务列表
      std::set<std::string> task_set_wait_to_start;                                 // 等待启动的任务集合
      for (auto& task_ptr : target_task_set_) {
        task_list_wait_to_start.emplace_back(std::make_pair(false, task_ptr));  // 添加任务指针
        task_set_wait_to_start.insert(task_ptr->GetTaskName());                 // 添加任务名称
      }
      std::set<std::string> all_init_node_set_log;  // 记录所有初始化节点集合

      // 等待所有目标任务启动
      while (!task_set_wait_to_start.empty()) {
        for (auto& task : task_list_wait_to_start) {
          const auto& task_name = task.second->GetTaskName();  // 获取任务名称
          if (!task.first) {
            GroupTaskSetting task_setting;
            const auto& exclusive_task_group = executer_config_.exclusive_task_group;  // 获取独占组配置
            if (exclusive_task_group.find(target_group_) != exclusive_task_group.end()) {
              if (exclusive_task_group.at(target_group_).task_list.find(task_name) != exclusive_task_group.at(target_group_).task_list.end()) {
                task_setting = exclusive_task_group.at(target_group_).task_list.at(task_name);  // 获取任务设置
              }
            }
            bool is_pre_node_empty = task_setting.pre_node.empty();  // 检查前置节点是否为空
            const auto& pre_node_list = task_setting.pre_node;       // 获取前置节点列表

            // 获取强制初始化节点集合
            std::set<std::string> force_init_node_set(task_setting.force_init_node.begin(), task_setting.force_init_node.end());
            std::set<std::string> all_init_node_set;  // 所有初始化节点集合
            std::set_union(force_init_node_set.begin(), force_init_node_set.end(), enter_node_set_.begin(), enter_node_set_.end(),
                           std::inserter(all_init_node_set, all_init_node_set.begin()));  // 合并初始化节点集合

            // 检查前置节点是否准备就绪
            bool is_pre_node_ready = std::all_of(pre_node_list.begin(), pre_node_list.end(), [this](const std::string& pre_node_name) {
              return node_map_->GetNodePtr(pre_node_name)->GetState() == NodeState::RUNNING;  // 检查节点状态
            });

            // 如果前置节点为空或准备就绪，启动任务
            if (is_pre_node_empty || is_pre_node_ready) {
              task.first = true;                                                         // 标记任务为已启动
              auto init_node_set = task.second->Init(all_init_node_set);                 // 初始化任务并获取初始化节点集合
              task.second->TaskStart(task.second->GetTaskSetting().system_setting);      // 启动任务
              all_init_node_set_log.insert(init_node_set.begin(), init_node_set.end());  // 记录初始化节点
              task_set_wait_to_start.erase(task.second->GetTaskName());                  // 从等待集合中移除任务
              logger_->info("[Executer] Task {} start.", task.second->GetTaskName());    // 记录任务启动信息
            }
          }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));  // 等待1毫秒
      }
      std::set<std::string> running_node_set;  // 运行节点集合
      for (auto& task : target_task_set_) {
        for (auto& node : task->GetTaskSetting().node_list) {
          running_node_set.insert(node.node_name);  // 添加运行节点
        }
      }
      logger_->info(
          "[Executer] Transition from {} to group {} finished.\n      Node State:\n                 - Exit node: {} \n                 - Enter node: {} \n                 - Init node: {}\n                 - Running node: {}\n",
          ColorPrint(current_group_.GetValue(), ColorEnum::YELLOW), ColorPrint(target_group_, ColorEnum::YELLOW),
          ColorPrint(JointStrSet(exit_node_set_, ","), ColorEnum::BLUE), ColorPrint(JointStrSet(enter_node_set_, ","), ColorEnum::GREEN),
          ColorPrint(JointStrSet(all_init_node_set_log, ","), ColorEnum::YELLOW),
          ColorPrint(JointStrSet(running_node_set, ","), ColorEnum::GREEN));  // 记录转换完成信息

      current_group_ = target_group_;  // 更新当前组
      is_transition_ = false;          // 重置转换状态
    } else {
      all_current_task_stop_ = std::all_of(current_task_set_.begin(), current_task_set_.end(),
                                           [](const auto& task) { return task->GetState() == TaskState::STANDBY; });  // 检查所有当前任务是否已停止
    }
  } else {
    // 检查所有节点退出和进入状态
    all_node_exit_check_ = std::all_of(exit_node_set_.begin(), exit_node_set_.end(),
                                       [this](const auto& node) { return node_map_->GetNodePtr(node)->TryExit(); });  // 检查退出节点
    all_node_enter_check_ = std::all_of(enter_node_set_.begin(), enter_node_set_.end(),
                                        [this](const auto& node) { return node_map_->GetNodePtr(node)->TryEnter(); });  // 检查进入节点
  }
}

}  // namespace ocm
