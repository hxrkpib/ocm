#include "debug_anywhere/debug_anywhere.hpp"
#include "common/ip_tool.hpp"
#include "debug_anywhere/debug_data.hpp"
#include "lcm/lcm-cpp.hpp"
#include "task/rt/sched_rt.hpp"

namespace ocm {

DebugDataStruct::DebugDataStruct(const std::string& t, const std::vector<double>& v) : topic(t), data(v) {
  // 构造函数:初始化话题名称和数据
}

DebugAnywhereTask::DebugAnywhereTask(const DebugAnywhereConfig& config) : config_(config), data_queue_(config.queue_size), sem_(0), running_(false) {
  // 构造函数:初始化配置、数据队列、信号量和运行状态
  running_.store(true);
  thread_ = std::thread([this] { Loop(); });
}

DebugAnywhereTask::~DebugAnywhereTask() {
  // 析构函数:停止运行并等待线程结束
  running_.store(false);
  sem_.release();
  if (thread_.joinable()) {
    thread_.join();
  }
}

void DebugAnywhereTask::Loop() {
  // 设置线程名称
  rt::set_thread_name("debug_anywhere");
  pid_t pid = gettid();
  if (config_.all_priority_enable) {
    // 设置线程优先级
    rt::set_thread_priority(pid, config_.system_setting.priority, SCHED_FIFO);
  }
  if (config_.all_cpu_affinity_enable) {
    // 设置CPU亲和性
    rt::set_thread_cpu_affinity(pid, config_.system_setting.cpu_affinity);
  }
  // 创建LCM实例并获取IP哈希值
  auto lcm = std::make_shared<lcm::LCM>("udpm://239.255.76.67:" + config_.port + "?ttl=" + config_.ttl);
  ip_hash_ = ipv4_to_unique_hash_hex(config_.ip);

  while (running_.load()) {
    DebugDataStruct* data_ptr;
    while (data_queue_.pop(data_ptr)) {
      // 处理数据队列中的调试数据
      DebugData debug_data;
      debug_data.count = data_ptr->data.size();
      debug_data.data = data_ptr->data;
      // 发布数据到LCM通道
      lcm->publish(data_ptr->topic + "_" + ip_hash_, &debug_data);
      delete data_ptr;
    }
    // 等待新数据
    sem_.acquire();
  }
}

void DebugAnywhereTask::Publish(const std::string& channel, const std::vector<double>& data) {
  // 检查是否启用调试功能
  if (!config_.enable) {
    return;
  }
  // 检查通道是否在白名单中
  if (config_.white_list_enable && std::find(config_.white_list.begin(), config_.white_list.end(), channel) == config_.white_list.end()) {
    return;
  }
  // 创建新的调试数据结构并尝试推送到队列
  DebugDataStruct* data_ptr = new DebugDataStruct(channel, data);
  if (data_queue_.push(data_ptr)) {
    sem_.release();
  } else {
    delete data_ptr;
  }
}

DebugAnywhere& DebugAnywhere::getInstance() {
  // 获取单例实例
  static DebugAnywhere instance;
  return instance;
}

void DebugAnywhere::initialize(const DebugAnywhereConfig& config) {
  // 初始化调试任务
  getInstance().task_ = std::make_shared<DebugAnywhereTask>(config);
}

void DebugAnywhere::Publish(const std::string& channel, const std::vector<double>& data) {
  // 发布调试数据
  getInstance().task_->Publish(channel, data);
}

DebugAnywhere::DebugAnywhere() = default;

DebugAnywhere::~DebugAnywhere() = default;

}  // namespace ocm
