
#include "log_anywhere/log_anywhere.hpp"

#include <spdlog/common.h>
#include <filesystem>
#include <iostream>
#include <vector>
#include "task/rt/sched_rt.hpp"

namespace ocm {

LogAnywhere::LogAnywhere(const LoggerConfig& config) {
  rt::set_thread_name("log_anywhere");  // 设置线程名称
  pid_t pid = gettid();                 // 获取线程ID
  if (config.all_priority_enable) {
    rt::set_thread_priority(pid, config.system_setting.priority, SCHED_FIFO);  // 设置线程优先级
  }
  if (config.all_cpu_affinity_enable) {
    rt::set_thread_cpu_affinity(pid, config.system_setting.cpu_affinity);  // 设置CPU亲和性
  }
  try {
    std::filesystem::create_directories(std::filesystem::path(config.log_file).parent_path());  // 确保日志目录存在

    spdlog::init_thread_pool(config.queue_size, config.thread_count);  // 初始化spdlog线程池

    std::vector<spdlog::sink_ptr> sinks;                                                             // 创建接收器列表
    sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(config.log_file, true));  // 文件接收器
    sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());                     // 控制台接收器

    logger_ = std::make_shared<spdlog::async_logger>("openrobot_ocm_logger", sinks.begin(), sinks.end(), spdlog::thread_pool(),
                                                     spdlog::async_overflow_policy::block);  // 创建异步记录器

    logger_->set_level(spdlog::level::trace);  // 设置日志级别为trace

    logger_->set_pattern("[%T][%^%l%$] %v");  // 定义日志消息模式

    logger_->flush_on(spdlog::level::info);  // 在info级别及以上刷新日志

    spdlog::register_logger(logger_);  // 全局注册记录器

    logger_->info("LogAnywhere initialized successfully.");  // 记录初始化成功信息
  } catch (const spdlog::spdlog_ex& ex) {
    std::cerr << "LogAnywhere initialization failed: " << ex.what() << std::endl;  // 处理初始化失败，输出错误信息
  }
}

LogAnywhere::~LogAnywhere() {
  spdlog::shutdown();  // 关闭spdlog，确保所有日志被刷新并释放资源
}

}  // namespace ocm
