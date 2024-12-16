
#pragma once

#include <spdlog/async.h>
#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <memory>
#include <set>
#include <string>
#include "common/struct_type.hpp"
namespace ocm {

/**
 * @brief 将集合中的元素连接成以分隔符分隔的单个字符串。
 *
 * 此函数接受一个 `std::set<std::string>` 和一个分隔符字符串，
 * 然后将集合中的所有元素连接成一个字符串，元素之间用指定的分隔符分隔。
 *
 * @param s 要连接的字符串集合。
 * @param delimiter 用于分隔元素的字符串。
 * @return 一个由分隔符分隔的单个连接字符串。
 */
inline std::string JointStrSet(const std::set<std::string>& s, const std::string& delimiter) {
  std::string result;
  for (auto it = s.begin(); it != s.end(); ++it) {
    if (it != s.begin()) {
      result += delimiter;
    }
    result += *it;
  }
  return result;
}

/**
 * @enum ColorEnum
 * @brief 用于彩色输出的颜色枚举。
 *
 * 定义可以用于在控制台中为字符串着色的各种颜色。
 */
enum class ColorEnum : uint8_t {
  RED = 0, /**< 红色 */
  GREEN,   /**< 绿色 */
  YELLOW,  /**< 黄色 */
  BLUE,    /**< 蓝色 */
  MAGENTA, /**< 品红色 */
  CYAN,    /**< 青色 */
  WHITE    /**< 白色 */
};

/**
 * @brief 为彩色控制台输出应用ANSI颜色代码。
 *
 * 此函数接受一个字符串和一个颜色枚举，然后用相应的
 * ANSI颜色代码包裹字符串，以在支持的终端中显示彩色文本。
 *
 * @param str 要着色的输入字符串。
 * @param color 要应用的颜色，由 `ColorEnum` 枚举指定。
 * @return 一个用ANSI颜色代码包裹的新字符串。
 */
inline std::string ColorPrint(const std::string& str, ColorEnum color) {
  std::string color_code;

  // 根据 ColorEnum 值设置ANSI颜色代码
  switch (color) {
    case ColorEnum::RED:
      color_code = "\033[31m";  // Red
      break;
    case ColorEnum::GREEN:
      color_code = "\033[32m";  // Green
      break;
    case ColorEnum::YELLOW:
      color_code = "\033[33m";  // Yellow
      break;
    case ColorEnum::BLUE:
      color_code = "\033[34m";  // Blue
      break;
    case ColorEnum::MAGENTA:
      color_code = "\033[35m";  // Magenta
      break;
    case ColorEnum::CYAN:
      color_code = "\033[36m";  // Cyan
      break;
    case ColorEnum::WHITE:
      color_code = "\033[37m";  // White
      break;
    default:
      color_code = "\033[0m";  // Reset
      // 重置
      break;
  }

  // 重置颜色
  std::string reset_code = "\033[0m";

  // 返回着色的字符串
  return color_code + str + reset_code;
}

/**
 * @struct LoggerConfig
 * @brief LogAnywhere记录器的配置设置。
 *
 * 该结构体包含初始化记录器所需的配置参数，
 * 包括日志文件路径、异步记录的队列大小和后端线程数。
 */
struct LoggerConfig {
  std::string log_file = "logs/log.txt"; /**< 日志文件路径 */
  size_t queue_size = 8192;              /**< 异步记录队列的大小 */
  size_t thread_count = 1;               /**< 用于记录的后端线程数量 */
  bool all_priority_enable = false;      /**< 是否为所有线程启用优先级 */
  bool all_cpu_affinity_enable = false;  /**< 是否为所有线程启用CPU亲和性 */
  SystemSetting system_setting;          /**< 线程优先级和CPU亲和性设置 */
};

/**
 * @class LogAnywhere
 * @brief 使用spdlog管理日志记录的单例类。
 *
 * LogAnywhere类使用文件和控制台sinks初始化和管理spdlog记录器。
 * 它确保日志记录正确设置，具有异步功能，并提供记录器的全局访问点。
 */
class LogAnywhere {
 public:
  /**
   * @brief 使用给定配置构造LogAnywhere单例。
   *
   * 使用文件和控制台sinks初始化spdlog记录器，设置日志级别和格式，
   * 并注册记录器以供全局访问。
   *
   * @param config 记录器的配置设置。
   */
  LogAnywhere(const LoggerConfig& config);

  /**
   * @brief 默认析构函数。
   *
   * 关闭spdlog记录器，确保所有日志被刷新并清理资源。
   */
  ~LogAnywhere();

  // 删除拷贝构造函数和赋值操作符以防止复制
  LogAnywhere(const LogAnywhere&) = delete;
  LogAnywhere& operator=(const LogAnywhere&) = delete;

 private:
  std::shared_ptr<spdlog::logger> logger_; /**< 指向spdlog记录器实例的共享指针 */
};

/**
 * @brief 获取 "openrobot_ocm_logger" 的记录器实例。
 *
 * 该函数检查是否存在名为 "openrobot_ocm_logger" 的记录器。
 * 如果未找到记录器，则创建并返回一个默认记录器。
 * 默认记录器将日志写入名为 "logs/default.log" 的文件。
 *
 * @return 指向记录器实例的共享指针。
 * @throws std::runtime_error 如果无法初始化或找到记录器，则抛出 std::runtime_error。
 */
inline std::shared_ptr<spdlog::logger> GetLogger() {
  auto logger = spdlog::get("openrobot_ocm_logger");
  if (!logger) {
    // 如果未找到记录器，则创建一个默认记录器
    logger = spdlog::basic_logger_mt("openrobot_ocm_logger", "logs/default.log");
    logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(logger);
  }
  return logger;
}

}  // namespace ocm
