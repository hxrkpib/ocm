#pragma once
#include <arpa/inet.h>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

/**
 * @brief 将IPv4地址字符串转换为唯一的十六进制哈希值。
 *
 * 该函数接受一个IPv4地址的字符串格式（例如 "192.168.1.1"），
 * 将其转换为32位整数表示形式，并将其格式化为8个字符的十六进制字符串。
 * 这个唯一的哈希值可用于各种应用，如网络设备识别、哈希算法，
 * 或创建IP地址的紧凑表示。
 *
 * @param ip_str IPv4地址的字符串形式。
 * @return 一个包含IPv4地址唯一十六进制哈希值的字符串。
 *
 * @throws std::invalid_argument 如果提供的IP地址无效。
 *
 */

inline std::string ipv4_to_unique_hash_hex(const std::string& ip_str) {
  struct in_addr ipv4_addr;

  // 将IPv4地址从文本形式转换为二进制形式。
  if (inet_pton(AF_INET, ip_str.c_str(), &ipv4_addr) != 1) {
    throw std::invalid_argument("Invalid IPv4 address: " + ip_str);
  }

  // 将二进制地址从网络字节序转换为主机字节序。
  uint32_t ip_int = ntohl(ipv4_addr.s_addr);

  // 创建一个字符串流，将整数格式化为8字符的十六进制字符串。
  std::ostringstream oss;
  oss << std::hex           // 使用十六进制格式
      << std::setw(8)       // 设置宽度为8个字符
      << std::setfill('0')  // 如果必要，用零填充
      << ip_int;            // 插入整数值

  return oss.str();
}
