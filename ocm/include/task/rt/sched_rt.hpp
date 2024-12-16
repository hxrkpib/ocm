#pragma once

#include <linux/types.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <set>
#include <string>
#include <vector>

namespace ocm::rt {

/**
 * @brief 调度策略定义。
 *
 * 定义了用于设置线程优先级和行为的各种调度策略。
 */
#define SCHED_OTHER 0
#define SCHED_FIFO 1
#define SCHED_RR 2
#define SCHED_BATCH 3
#define SCHED_ISO 4
#define SCHED_IDLE 5
#define SCHED_DEADLINE 6

// Define syscall numbers if not already defined
#ifndef SYS_sched_setattr
  #ifdef __x86_64__
    #define SYS_sched_setattr 314
    #define SYS_sched_getattr 315
  #elif defined(__i386__)
    #define SYS_sched_setattr 351
    #define SYS_sched_getattr 352
  #elif defined(__arm__)
    #define SYS_sched_setattr 380
    #define SYS_sched_getattr 381
  #elif defined(__aarch64__)
    #define SYS_sched_setattr 274
    #define SYS_sched_getattr 275
  #else
    #error "Unsupported architecture"
  #endif
#endif

/**
 * @brief 调度标志定义。
 *
 * 定义了调度属性中使用的各种标志。
 */
#define SF_SIG_RORUN 2
#define SF_SIG_DMISS 4
#define SF_BWRECL_DL 8
#define SF_BWRECL_RT 16
#define SF_BWRECL_OTH 32

/**
 * @brief 资源限制常量。
 *
 * 定义了截止时间和实时时间的资源限制常量。
 */
#define RLIMIT_DLDLINE 16
#define RLIMIT_DLRTIME 17

/**
 * @brief 表示调度属性的结构。
 *
 * `sched_attr_t` 结构用于指定线程或进程的各种调度参数。
 */
struct sched_attr_t {
  __u32 size; /**< 结构的大小 */

  __u32 sched_policy; /**< 调度策略 */
  __u64 sched_flags;  /**< 调度标志 */

  /* SCHED_NORMAL, SCHED_BATCH */
  __s32 sched_nice; /**< 降级值 */

  /* SCHED_FIFO, SCHED_RR */
  __u32 sched_priority; /**< 调度优先级 */

  /* SCHED_DEADLINE */
  __u64 sched_runtime;  /**< 截止调度的运行时间 */
  __u64 sched_deadline; /**< 截止调度的截止时间 */
  __u64 sched_period;   /**< 截止调度的周期 */
};

/**
 * @brief 设置线程或进程的调度属性。
 *
 * 该函数调用 `sched_setattr` 系统调用以设置调度属性。
 *
 * @param pid 要设置属性的线程的进程 ID。使用 `0` 表示调用线程。
 * @param attr 指向包含调度属性的 `sched_attr_t` 结构的指针。
 * @param flags 设置属性的标志。
 *
 * @return 成功时返回 `0`，失败时返回 `-1` 并相应地设置 `errno`。
 */
inline int sched_setattr(const pid_t pid, const sched_attr_t* attr, const unsigned int flags) { return syscall(SYS_sched_setattr, pid, attr, flags); }

/**
 * @brief 获取线程或进程的调度属性。
 *
 * 该函数调用 `sched_getattr` 系统调用以获取调度属性。
 *
 * @param pid 要检索属性的线程的进程 ID。使用 `0` 表示调用线程。
 * @param attr 指向将存储调度属性的 `sched_attr_t` 结构的指针。
 * @param size `sched_attr_t` 结构的大小。
 * @param flags 获取属性的标志。
 *
 * @return 成功时返回 `0`，失败时返回 `-1` 并相应地设置 `errno`。
 */
inline int sched_getattr(const pid_t pid, sched_attr_t* attr, unsigned int size, const unsigned int flags) {
  return syscall(SYS_sched_getattr, pid, attr, size, flags);
}

/**
 * @brief 设置线程的优先级和调度策略。
 *
 * 该函数使用 `sched_setscheduler` 来设置线程的调度策略和优先级。
 *
 * @param pid 要设置优先级的线程的进程 ID。使用 `0` 表示调用线程。
 * @param sched_priority 要设置的调度优先级。
 * @param policy 要设置的调度策略（例如，`SCHED_FIFO`，`SCHED_RR`）。
 *
 * @return 成功时返回 `0`，失败时返回 `-1` 并相应地设置 `errno`。
 */
inline int set_thread_priority(const pid_t pid, size_t const sched_priority, const int policy) {
  struct sched_param param;
  memset(&param, 0, sizeof(param));
  param.sched_priority = sched_priority;
  return sched_setscheduler(pid, policy, &param);
}

/**
 * @brief 设置线程的CPU亲和性。
 *
 * 该函数为指定的线程设置CPU亲和性掩码，限制其仅在指定的CPU上运行。
 *
 * @param pid 要设置CPU亲和性的线程的进程 ID。
 * @param cpu_list 线程应绑定的CPU索引向量。
 *
 * @return 成功时返回 `0`，失败时返回 `-1` 并相应地设置 `errno`。
 *
 * @note 该函数确保CPU索引唯一且在有效范围内。
 */
inline int set_thread_cpu_affinity(const pid_t pid, const std::vector<int>& cpu_list) {
  cpu_set_t set;
  CPU_ZERO(&set);

  // Get the number of available CPUs 获取可用CPU的数量
  long num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
  if (num_cpus == -1) {
    perror("sysconf");
    return -1;
  }

  // Use std::set to automatically handle duplicate values
  // 使用 std::set 自动处理重复值
  std::set<int> unique_cpus(cpu_list.begin(), cpu_list.end());

  for (const int cpu : unique_cpus) {
    // Check if CPU index is within the valid range 检查CPU索引是否在有效范围内
    if (cpu < 0 || cpu >= num_cpus) {
      fprintf(stderr, "Invalid CPU index: %d\n", cpu);
      return -1;
    }
    CPU_SET(cpu, &set);
  }

  // Set CPU affinity 设置CPU亲和性
  if (sched_setaffinity(pid, sizeof(set), &set) == -1) {
    perror("sched_setaffinity");
    return -1;
  }

  return 0;
}

/**
 * @brief 设置当前线程的名称。
 *
 * 该函数使用 `prctl` 设置调用线程的名称。可以使用 `top` 或 `htop` 等工具查看名称。
 *
 * @param name 要分配给线程的名称。包括空终止符在内，不应超过16个字符。
 *
 * @note 如果名称超过最大允许长度，将被截断。
 */
inline void set_thread_name(const std::string& name) {
  const auto iErr = prctl(PR_SET_NAME, name.c_str());
  (void)iErr;  // Suppress unused variable warning
}

}  // namespace ocm::rt
