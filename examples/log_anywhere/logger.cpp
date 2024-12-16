#include "log_anywhere/log_anywhere.hpp"

using namespace ocm;
int main() {
  // 配置日志
  LoggerConfig log_config;
  log_config.log_file = "my_logs/executer_test.log";  // 日志文件路径
  log_config.queue_size = 8192;                       // 可选，默认8192
  log_config.thread_count = 1;                        // 可选，默认1
  log_config.all_priority_enable = true;              // 启用所有线程优先级
  log_config.all_cpu_affinity_enable = true;          // 启用所有线程CPU亲和性
  log_config.system_setting.priority = 80;            // 设置系统优先级
  log_config.system_setting.cpu_affinity = {7};       // 设置系统CPU亲和性
  auto logger_generator = std::make_shared<LogAnywhere>(log_config);
  auto logger = GetLogger();

  logger->info("Logger 已初始化，日志文件：{}", log_config.log_file);
  logger->warn("这是一个警告信息");
  logger->error("发生错误，错误码：{}", 404);

  sleep(100);
  return 0;
}
