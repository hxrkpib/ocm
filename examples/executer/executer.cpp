#include "executer/executer.hpp"
#include <common/struct_type.hpp>
#include <memory>
#include <ocm/shared_memory_topic_lcm.hpp>
#include "common/enum.hpp"
#include "executer/desired_group_data.hpp"
#include "log_anywhere/log_anywhere.hpp"
#include "node/node_map.hpp"
#include "node_test.hpp"
#include "yaml_template/task/yaml_load_generated_classes.hpp"

using namespace ocm;

// 定义一个继承自TaskBase的任务类，用于定时任务
class TaskTimer : public ocm::TaskBase {
 public:
  // 构造函数，初始化任务名称、定时器类型、周期等
  TaskTimer() : ocm::TaskBase("openrobot_task_timer", ocm::TimerType::INTERNAL_TIMER, 0.0, false, false) {
    // 定义信号量名称列表
    std::vector<std::string> sem_name_list = {"executer", "resident_task_1", "standby_task_1", "standby_task_2", "standby_task_3"};
    // 创建信号量和共享内存数据
    for (const auto& sem_name : sem_name_list) {
      sem_.emplace_back(SharedMemorySemaphore(sem_name, 0));
      shm_.emplace_back(SharedMemoryData<uint8_t>(sem_name, true, sizeof(uint8_t)));
    }
    // 初始化共享内存数据
    for (auto& shm : shm_) {
      shm.Lock();
      *shm.Get() = 1;  // 设置为1毫秒
      shm.UnLock();
    }
  }

  // 默认析构函数
  ~TaskTimer() = default;

  // 重写Run方法，每次运行时递增信号量
  void Run() override {
    for (auto& sem : sem_) {
      sem.IncrementWhenZero();
    }
  }

 private:
  // 信号量列表
  std::vector<ocm::SharedMemorySemaphore> sem_;
  // 共享内存数据列表
  std::vector<ocm::SharedMemoryData<uint8_t>> shm_;
};

int main() {
  // 配置日志
  ocm::LoggerConfig log_config;
  log_config.log_file = "my_logs/executer_test.log";  // 日志文件路径
  log_config.queue_size = 8192;                       // 可选，默认8192
  log_config.thread_count = 1;                        // 可选，默认1
  log_config.all_priority_enable = true;              // 启用所有线程优先级
  log_config.all_cpu_affinity_enable = true;          // 启用所有线程CPU亲和性
  log_config.system_setting.priority = 80;            // 设置系统优先级
  log_config.system_setting.cpu_affinity = {7};       // 设置系统CPU亲和性
  auto logger_generator = std::make_shared<ocm::LogAnywhere>(log_config);
  auto logger = GetLogger();

  // 创建并启动定时任务
  TaskTimer timer_task;
  timer_task.SetPeriod(0.001);  // 设置周期为1毫秒
  SystemSetting timer_system_setting;
  timer_system_setting.priority = 0;           // 设置优先级
  timer_system_setting.cpu_affinity = {0};     // 设置CPU亲和性
  timer_task.TaskStart(timer_system_setting);  // 启动任务

  // 获取配置并从YAML文件更新配置
  ConfigCollect& config = ConfigCollect::getInstance();
  config.update_from_yaml_all("/home/lizhen/works/code/OpenRobot/OCM/examples/executer/yaml_template/task");

  // 创建节点映射并添加各个节点
  std::shared_ptr<NodeMap> node_map = std::make_shared<NodeMap>();
  node_map->AddNode("NodeA", std::make_shared<NodeA>("NodeA"));
  node_map->AddNode("NodeB", std::make_shared<NodeB>("NodeB"));
  node_map->AddNode("NodeC", std::make_shared<NodeC>("NodeC"));
  node_map->AddNode("NodeD", std::make_shared<NodeD>("NodeD"));
  node_map->AddNode("NodeE", std::make_shared<NodeE>("NodeE"));

  // 配置执行器
  ExecuterConfig executer_config;

  // 获取执行器设置、任务列表和排他任务组
  const auto& executer_setting = config.get_task_config().ExecuterSetting();
  const auto& task_list = config.get_task_config().TaskList();
  const auto& exclusive_task_group = config.get_task_config().ExclusiveTaskGroup();

  // 配置执行器的定时器设置和系统设置
  executer_config.executer_setting.package_name = "executer";
  executer_config.executer_setting.timer_setting.timer_type = timer_type_map.at(executer_setting.TimerSetting().TimerType());
  executer_config.executer_setting.timer_setting.period = executer_setting.TimerSetting().Period();
  executer_config.executer_setting.system_setting.priority = static_cast<int>(executer_setting.SystemSetting().Priority());
  // executer_config.executer_setting.system_setting.cpu_affinity = executer_setting.SystemSetting().ExecuterCpuAffinity();

  // 配置常驻任务组
  for (const auto& task : task_list.ResidentGroup()) {
    TaskSetting task_setting;
    task_setting.task_name = task.TaskName();                                                    // 任务名称
    task_setting.timer_setting.timer_type = timer_type_map.at(task.TimerSetting().TimerType());  // 定时器类型
    task_setting.timer_setting.period = task.TimerSetting().Period();                            // 定周期
    task_setting.system_setting.priority = task.SystemSetting().Priority();                      // 系统优先级
    // task_setting.system_setting.cpu_affinity = task.SystemSetting().CpuAffinity();
    task_setting.launch_setting.pre_node = task.LaunchSetting().PreNode();  // 前置节点
    task_setting.launch_setting.delay = task.LaunchSetting().Delay();       // 延迟
    // 添加节点配置
    for (const auto& node : task.NodeList()) {
      NodeConfig node_config;
      node_config.node_name = node.NodeName();          // 节点名称
      node_config.output_enable = node.OutputEnable();  // 是否启用输出
      task_setting.node_list.push_back(node_config);
    }
    executer_config.task_list.resident_group[task.TaskName()] = task_setting;  // 添加到常驻任务组
  }

  // 配置备用任务组
  for (const auto& task : task_list.StandbyGroup()) {
    TaskSetting task_setting;
    task_setting.task_name = task.TaskName();                                                    // 任务名称
    task_setting.timer_setting.timer_type = timer_type_map.at(task.TimerSetting().TimerType());  // 定时器类型
    task_setting.timer_setting.period = task.TimerSetting().Period();                            // 定周期
    task_setting.system_setting.priority = task.SystemSetting().Priority();                      // 系统优先级
    // task_setting.system_setting.cpu_affinity = task.SystemSetting().CpuAffinity();
    // 添加节点配置
    for (const auto& node : task.NodeList()) {
      NodeConfig node_config;
      node_config.node_name = node.NodeName();          // 节点名称
      node_config.output_enable = node.OutputEnable();  // 是否启用输出
      task_setting.node_list.push_back(node_config);
    }
    executer_config.task_list.standby_group[task.TaskName()] = task_setting;  // 添加到备用任务组
  }

  // 配置排他任务组
  for (const auto& group : exclusive_task_group) {
    GroupSetting group_setting;
    group_setting.group_name = group.GroupName();  // 组名称
    // 配置组内的任务
    for (const auto& task : group.TaskList()) {
      GroupTaskSetting group_task_setting;
      group_task_setting.task_name = task.TaskName();                 // 任务名称
      group_task_setting.force_init_node = task.ForceInitNode();      // 是否强制初始化节点
      group_task_setting.pre_node = task.PreNode();                   // 前置节点
      group_setting.task_list[task.TaskName()] = group_task_setting;  // 添加到组内任务列表
    }
    executer_config.exclusive_task_group[group.GroupName()] = group_setting;  // 添加到排他任务组
  }

  // 创建执行器实例
  Executer executer(executer_config, node_map, "executer_desired_group");
  executer.CreateTask();  // 创建任务
  executer.InitTask();    // 初始化任务

  // 发布期望的任务组
  SharedMemoryTopicLcm desired_group_topic;
  DesiredGroupData desired_group_data;
  desired_group_data.desired_group = "passive";  // 设置期望组为"passive"
  desired_group_topic.Publish("executer_desired_group", "executer_desired_group", &desired_group_data);
  std::this_thread::sleep_for(std::chrono::seconds(3));  // 等待3秒
  desired_group_data.desired_group = "pdstand";          // 设置期望组为"pdstand"
  desired_group_topic.Publish("executer_desired_group", "executer_desired_group", &desired_group_data);
  std::this_thread::sleep_for(std::chrono::seconds(6));  // 等待6秒

  // 输出退出信息
  std::cout << "executer exit" << std::endl;
  timer_task.TaskDestroy();  // 销毁定时任务
  std::cout << "timer_task destroyed" << std::endl;
  executer.ExitAllTask();                                // 退出所有任务
  std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待1秒
  executer.TaskDestroy();                                // 销毁执行器任务
  std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待1秒
  return 0;                                              // 程序结束
}
