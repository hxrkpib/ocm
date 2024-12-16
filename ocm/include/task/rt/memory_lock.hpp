#pragma once

#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <limits.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <vector>

namespace ocm {

/**
 * @brief 用于锁定当前和未来的内存分配，防止它们被换出。
 *
 * 本函数使用 `mlockall` 锁定所有当前和未来的内存分配，确保进程的内存驻留在 RAM 中。它还通过禁用 malloc 修剪和 mmap
 * 使用来配置内存分配行为，以提升实时性能并降低延迟。
 *
 * @return 成功时返回 `0`，失败时返回 `-1`。
 *
 * @note 失败时，会在 `stderr` 打印错误消息。
 */
inline int lock_memory() {
  if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
    perror("mlockall failed");
    return -1;
  }

  // 关闭 malloc 修剪。
  if (mallopt(M_TRIM_THRESHOLD, -1) == 0) {
    perror("mallopt for trim threshold failed");
    munlockall();
    return -1;
  }

  // 关闭 mmap 使用。
  if (mallopt(M_MMAP_MAX, 0) == 0) {
    perror("mallopt for mmap failed");
    mallopt(M_TRIM_THRESHOLD, 128 * 1024);
    munlockall();
    return -1;
  }
  return 0;
}

/**
 * @brief 锁定内存并预取动态内存分配以防止页面错误。
 *
 * 该函数首先使用 `lock_memory()` 锁定进程的内存。然后通过分配和访问大内存块进行预取内存，直到不再发生额外的页面错误。这确保所有所需的内存页都加载到
 * RAM 中，减少在实时操作期间发生页面错误的可能性。
 *
 * @return 成功时返回 `0`，失败时返回 `-1`。
 *
 * @note 失败时，会在 `stderr` 打印错误消息，并在返回前释放所有已分配的内存。
 */
inline int lock_and_prefault_dynamic() {
  if (lock_memory() != 0) {
    return -1;
  }

  struct rusage usage;
  size_t page_size = sysconf(_SC_PAGESIZE);
  getrusage(RUSAGE_SELF, &usage);
  std::vector<char*> prefaulter;
  size_t prev_minflts = usage.ru_minflt;
  size_t prev_majflts = usage.ru_majflt;
  size_t encountered_minflts = 1;
  size_t encountered_majflts = 1;
  // 预取，直到不再遇到页面错误
  while (encountered_minflts > 0 || encountered_majflts > 0) {
    char* ptr;
    try {
      ptr = new char[64 * page_size];
      memset(ptr, 0, 64 * page_size);
    } catch (std::bad_alloc& e) {
      fprintf(stderr, "捕捉到异常: %s\n", e.what());
      fprintf(stderr, "解锁内存并继续。\n");
      for (auto& ptr : prefaulter) {
        delete[] ptr;
      }

      mallopt(M_TRIM_THRESHOLD, 128 * 1024);
      mallopt(M_MMAP_MAX, 65536);
      munlockall();
      return -1;
    }
    prefaulter.push_back(ptr);
    getrusage(RUSAGE_SELF, &usage);
    size_t current_minflt = usage.ru_minflt;
    size_t current_majflt = usage.ru_majflt;
    encountered_minflts = current_minflt - prev_minflts;
    encountered_majflts = current_majflt - prev_majflts;
    prev_minflts = current_minflt;
    prev_majflts = current_majflt;
  }

  for (auto& ptr : prefaulter) {
    delete[] ptr;
  }
  return 0;
}

/**
 * @brief 锁定内存并预取指定量的动态内存。
 *
 * 该函数使用 `lock_memory()` 锁定进程的内存并预取指定量的动态内存，以确保内存驻留在 RAM
 * 中。它分配与系统页面大小匹配的对齐内存块并访问它们以触发页面加载。
 *
 * @param process_max_dynamic_memory 需要预取的动态内存的最大量（以字节为单位）。
 *
 * @return 成功时返回 `0`。
 *
 * @throws std::runtime_error 如果内存对齐或分配失败，则抛出 `std::runtime_error`。
 *
 * @note 失败时，会在 `stderr` 打印错误消息，并抛出 `std::runtime_error`。
 */
inline int lock_and_prefault_dynamic(size_t process_max_dynamic_memory) {
  if (lock_memory() != 0) {
    return -1;
  }

  void* buf = nullptr;
  const size_t pg_sz = sysconf(_SC_PAGESIZE);
  int res;
  res = posix_memalign(&buf, static_cast<size_t>(pg_sz), process_max_dynamic_memory);
  if (res != 0) {
    std::cerr << "进程实时初始化内存对齐失败: " << strerror(errno) << std::endl;
    throw std::runtime_error("进程实时初始化内存对齐失败");
  }
  memset(buf, 0, process_max_dynamic_memory);
  free(buf);

  return 0;
}

}  // namespace ocm
