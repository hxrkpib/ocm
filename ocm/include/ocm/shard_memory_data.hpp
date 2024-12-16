#pragma once

#include <cassert>
#include <stdexcept>
#include <string>
#include "common/prefix_string.hpp"
#include "ocm/shared_memory_semaphore.hpp"

namespace ocm {
/**
 * @brief 共享内存数据包装器。
 *
 * `SharedMemoryData` 类管理共享内存段，提供线程安全的访问和使用信号量进行同步。它通过允许多个进程对共享内存进行读写操作，促进进程间通信。
 *
 * @tparam T 存储在共享内存中的数据类型。
 */
template <typename T>
class SharedMemoryData {
 public:
  /**
   * @brief 构造一个 SharedMemoryData 实例。
   *
   * 使用给定的名称初始化共享内存段。如果 `check_size` 为真，
   * 则确保现有的共享内存段与指定的大小相匹配。
   *
   * @param name 共享内存段的标识符。
   * @param check_size 标志，指示是否验证现有共享内存的大小。
   * @param size 共享内存段的大小（以字节为单位）。如果 `check_size` 为真，则需要此参数。
   *
   * @throws std::runtime_error 如果初始化失败。
   */
  SharedMemoryData(const std::string& name, bool check_size, size_t size = 0) : sem_(name + "_shm", 1), data_(nullptr), fd_(0) {
    Init(name, check_size, size);
  }

  /**
   * @brief 析构函数。
   *
   * 默认析构函数确保正确清理共享内存资源。
   */
  ~SharedMemoryData() = default;

  /**
   * @brief 初始化共享内存段。
   *
   * 打开现有的共享内存段或在不存在时创建一个新的共享内存段。
   * 可选择检查大小是否与预期大小匹配。
   *
   * @param name 共享内存段的标识符。
   * @param check_size 标志，指示是否验证现有共享内存的大小。
   * @param size 共享内存段的大小（以字节为单位）。如果 `check_size` 为真，则需要此参数。
   *
   * @throws std::runtime_error 如果初始化失败。
   */
  void Init(const std::string& name, bool check_size, size_t size) {
    assert(!data_);
    bool is_create = false;
    name_ = GetNamePrefix(name);
    size_ = size;

    fd_ = shm_open(name_.c_str(), O_RDWR, 0);
    if (fd_ == -1) {
      if (errno == ENOENT) {
        fd_ = shm_open(name_.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH);
        if (fd_ == -1) {
          throw std::runtime_error("[SharedMemoryData] Failed to create shared memory \"" + name + "\": " + std::string(strerror(errno)));
        }
        if (ftruncate(fd_, size_) != 0) {
          throw std::runtime_error("[SharedMemoryData] ftruncate failed for \"" + name + "\": " + std::string(strerror(errno)));
        }
        is_create = true;
      } else {
        throw std::runtime_error("[SharedMemoryData] shm_open failed for \"" + name + "\": " + std::string(strerror(errno)));
      }
    } else {
      struct stat s;
      if (fstat(fd_, &s)) {
        throw std::runtime_error("[SharedMemoryData] fstat failed for \"" + name + "\": " + std::string(strerror(errno)));
      }
      if (check_size) {
        if ((size_t)s.st_size != size_) {
          throw std::runtime_error("[SharedMemoryData] Existing shared memory \"" + name + "\" size mismatch! Expected: " + std::to_string(size_) +
                                   ", Actual: " + std::to_string(s.st_size));
        }
      } else {
        size_ = s.st_size;
      }
    }

    void* mem = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (mem == MAP_FAILED) {
      throw std::runtime_error("[SharedMemoryData] mmap failed for \"" + name + "\": " + std::string(strerror(errno)));
    }
    if (is_create) {
      memset(mem, 0, size_);
    }
    data_ = static_cast<T*>(mem);
  }

  /**
   * @brief 关闭并销毁现有的共享内存段。
   *
   * 关闭信号量，取消映射共享内存，从系统中取消链接，并关闭文件描述符。
   *
   * @throws std::runtime_error 如果任何清理操作失败。
   */
  void CloseExisting() {
    sem_.Destroy();
    assert(data_);
    if (munmap(static_cast<void*>(data_), size_) != 0) {
      throw std::runtime_error("[SharedMemoryData::CloseExisting] munmap failed: " + std::string(strerror(errno)));
    }
    data_ = nullptr;
    if (shm_unlink(name_.c_str()) != 0) {
      if (errno != ENOENT) {
        throw std::runtime_error("[SharedMemoryData::CloseExisting] shm_unlink failed: " + std::string(strerror(errno)));
      }
    }
    if (close(fd_) != 0) {
      throw std::runtime_error("[SharedMemoryData::CloseExisting] close failed: " + std::string(strerror(errno)));
    }
    fd_ = 0;
  }

  /**
   * @brief 分离共享内存段而不销毁它。
   *
   * 取消映射共享内存并关闭文件描述符。
   *
   * @throws std::runtime_error 如果分离失败。
   */
  void Detach() {
    assert(data_);
    if (munmap(static_cast<void*>(data_), size_) != 0) {
      throw std::runtime_error("[SharedMemoryData::Detach] munmap failed: " + std::string(strerror(errno)));
    }
    data_ = nullptr;
    if (close(fd_) != 0) {
      throw std::runtime_error("[SharedMemoryData::Detach] close failed: " + std::string(strerror(errno)));
    }
    fd_ = 0;
  }

  /**
   * @brief 获取指向共享数据的指针。
   *
   * @return 指向类型 `T` 的共享数据的指针。
   *
   * @throws std::runtime_error 如果共享内存未映射。
   */
  T* Get() {
    assert(data_);
    return data_;
  }

  /**
   * @brief 获取信号量锁。
   *
   * 减少信号量以获得对共享内存的独占访问权限。
   *
   * @throws std::runtime_error 如果锁操作失败。
   */
  void Lock() { sem_.Decrement(); }

  /**
   * @brief 释放信号量锁。
   *
   * 增加信号量以释放对共享内存的独占访问权限。
   *
   * @throws std::runtime_error 如果解锁操作失败。
   */
  void UnLock() { sem_.Increment(); }

  /**
   * @brief 获取共享内存段的大小。
   *
   * @return 共享内存的大小（以字节为单位）。
   */
  int GetSize() const { return static_cast<int>(size_); }

 private:
  SharedMemorySemaphore sem_; /**< 共享内存访问同步的信号量。 */
  T* data_ = nullptr;         /**< 指向共享内存数据的指针。 */
  std::string name_;          /**< 共享内存段的标识符。 */
  size_t size_;               /**< 共享内存段的大小（以字节为单位）。 */
  int fd_;                    /**< 共享内存的文件描述符。 */
};

}  // namespace ocm
