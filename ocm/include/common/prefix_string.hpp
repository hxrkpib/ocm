#pragma once
#include <string>

namespace ocm {
/**
 * @brief 命名前缀
 */
#define NAME_PREFIX "openrobot_ocm_"

/**
 * @brief 获取带前缀的名称
 *
 * @param name 原始名称。
 * @return 返回带前缀的名称。
 */
inline std::string GetNamePrefix(const std::string& name) { return NAME_PREFIX + name; }

}  // namespace ocm
