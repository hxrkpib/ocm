
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <ctime>
#include <stdexcept>
#include "common/prefix_string.hpp"
#include "ocm/shared_memory_semaphore.hpp"

namespace ocm {

SharedMemorySemaphore::SharedMemorySemaphore(const std::string& name, unsigned int value) { Init(name, value); }

void SharedMemorySemaphore::Init(const std::string& name, unsigned int value) {
  const auto sem_name = GetNamePrefix(name);                // 获取信号量名称前缀
  sem_ = sem_open(sem_name.c_str(), O_CREAT, 0644, value);  // 打开或创建信号量
  if (sem_ == SEM_FAILED) {                                 // 检查信号量是否成功打开
    throw std::runtime_error("[SharedMemorySemaphore] Failed to initialize shared memory semaphore: " + std::string(strerror(errno)));  // 抛出异常
  } else {
    name_ = sem_name;  // 保存信号量名称
  }
}

void SharedMemorySemaphore::Increment() {
  if (sem_post(sem_) != 0) {                                                                                             // 尝试增加信号量
    throw std::runtime_error("[SharedMemorySemaphore] Failed to increment semaphore: " + std::string(strerror(errno)));  // 抛出异常
  }
}

void SharedMemorySemaphore::IncrementWhenZero() {
  int value;                                                                                                             // 定义信号量值
  if (sem_getvalue(sem_, &value) != 0) {                                                                                 // 获取信号量当前值
    throw std::runtime_error("[SharedMemorySemaphore] Failed to get semaphore value: " + std::string(strerror(errno)));  // 抛出异常
  }
  if (value == 0) {                                                                                                        // 如果信号量值为零
    if (sem_post(sem_) != 0) {                                                                                             // 尝试增加信号量
      throw std::runtime_error("[SharedMemorySemaphore] Failed to increment semaphore: " + std::string(strerror(errno)));  // 抛出异常
    }
  }
}

void SharedMemorySemaphore::Increment(unsigned int value) {
  for (unsigned int i = 0; i < value; ++i) {                                                                               // 循环增加信号量
    if (sem_post(sem_) != 0) {                                                                                             // 尝试增加信号量
      throw std::runtime_error("[SharedMemorySemaphore] Failed to increment semaphore: " + std::string(strerror(errno)));  // 抛出异常
    }
  }
}

void SharedMemorySemaphore::Decrement() {
  if (sem_wait(sem_) != 0) {                                                                                             // 尝试减少信号量
    throw std::runtime_error("[SharedMemorySemaphore] Failed to decrement semaphore: " + std::string(strerror(errno)));  // 抛出异常
  }
}

bool SharedMemorySemaphore::TryDecrement() {
  return (sem_trywait(sem_) == 0);  // 尝试减少信号量并返回结果
}

bool SharedMemorySemaphore::DecrementTimeout(uint64_t milliseconds) {
  struct timespec ts;                                                                                                 // 定义时间结构
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {                                                                     // 获取当前时间
    throw std::runtime_error("[SharedMemorySemaphore] Failed to get current time: " + std::string(strerror(errno)));  // 抛出异常
  }

  ts.tv_sec += milliseconds / 1000;               // 增加秒数
  ts.tv_nsec += (milliseconds % 1000) * 1000000;  // 增加纳秒数
  ts.tv_sec += ts.tv_nsec / 1000000000;           // 处理秒和纳秒的进位
  ts.tv_nsec %= 1000000000;                       // 确保纳秒在有效范围内

  return (sem_timedwait(sem_, &ts) == 0);  // 尝试在超时内减少信号量
}

int SharedMemorySemaphore::GetValue() const {
  int value;                                                                                                             // 定义信号量值
  if (sem_getvalue(sem_, &value) != 0) {                                                                                 // 获取信号量当前值
    throw std::runtime_error("[SharedMemorySemaphore] Failed to get semaphore value: " + std::string(strerror(errno)));  // 抛出异常
  }
  return value;  // 返回信号量值
}

void SharedMemorySemaphore::Destroy() {
  if (sem_close(sem_) != 0) {                                                                                        // 尝试关闭信号量
    throw std::runtime_error("[SharedMemorySemaphore] Failed to close semaphore: " + std::string(strerror(errno)));  // 抛出异常
  }
  if (sem_unlink(name_.c_str()) != 0) {                                                                                 // 尝试取消链接信号量
    if (errno != ENOENT) {                                                                                              // 检查错误类型
      throw std::runtime_error("[SharedMemorySemaphore] Failed to unlink semaphore: " + std::string(strerror(errno)));  // 抛出异常
    }
  }
}

SharedMemorySemaphore::~SharedMemorySemaphore() = default;

}  // namespace ocm
