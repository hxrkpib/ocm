#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "ocm/shard_memory_data.hpp"
#include "ocm/shared_memory_semaphore.hpp"
#include "rclcpp/serialization.hpp"
#include "rclcpp/serialized_message.hpp"
#include "rcutils/types.h"

namespace ocm {
/**
 * @brief 共享内存主题管理器。
 *
 * `SharedMemoryTopicRos2` 类简化了使用共享内存发布和订阅主题的过程。
 * 它管理多个共享内存段和信号量，允许不同主题之间高效的进程间通信。
 */
class SharedMemoryTopicRos2 {
 public:
  /**
   * @brief 默认构造函数。
   *
   * 初始化 `SharedMemoryTopicRos2` 实例。
   */
  SharedMemoryTopicRos2() = default;

  /**
   * @brief 删除的拷贝构造函数。
   *
   * 防止复制 `SharedMemoryTopicRos2` 实例以保持唯一所有权语义。
   */
  SharedMemoryTopicRos2(const SharedMemoryTopicRos2&) = delete;

  /**
   * @brief 删除的拷贝赋值运算符。
   *
   * 防止将一个 `SharedMemoryTopicRos2` 赋值给另一个以保持唯一所有权语义。
   */
  SharedMemoryTopicRos2& operator=(const SharedMemoryTopicRos2&) = delete;

  /**
   * @brief 删除的移动构造函数。
   *
   * 防止移动 `SharedMemoryTopicRos2` 实例以保持唯一所有权语义。
   */
  SharedMemoryTopicRos2(SharedMemoryTopicRos2&&) = delete;

  /**
   * @brief 删除的移动赋值运算符。
   *
   * 防止移动赋值 `SharedMemoryTopicRos2` 实例以保持唯一所有权语义。
   */
  SharedMemoryTopicRos2& operator=(SharedMemoryTopicRos2&&) = delete;

  /**
   * @brief 析构函数。
   *
   * 默认析构函数确保共享内存主题的正确清理。
   */
  ~SharedMemoryTopicRos2() = default;

  /**
   * @brief 发布单个消息到指定主题。
   *
   * 将消息写入与 `shm_name` 关联的共享内存段，并发送与 `topic_name` 关联的信号量以通知订阅者。
   *
   * @tparam MessageType 发布消息的类型。必须支持 `encode` 和 `getEncodedSize` 方法。
   * @param topic_name 发布到的主题名。
   * @param shm_name 共享内存段的名称。
   * @param msg 指向要发布的消息的指针。
   *
   * @throws std::runtime_error 如果写入共享内存或发布信号量失败。
   */
  template <class MessageType>
  void Publish(const std::string& topic_name, const std::string& shm_name, const MessageType& msg) {
    WriteDataToSHM(shm_name, msg);
    PublishSem(topic_name);
  }

  /**
   * @brief 发布多个消息到多个指定主题。
   *
   * 将消息列表写入与 `shm_name` 关联的共享内存段，并发送与提供的每个 `topic_name` 关联的信号量以通知订阅者。
   *
   * @tparam MessageType 发布消息的类型。必须支持 `encode` 和 `getEncodedSize` 方法。
   * @param topic_names 发布消息的主题名称向量。
   * @param shm_name 共享内存段的名称。
   * @param msgs 要发布的消息向量。
   *
   * @throws std::runtime_error 如果写入共享内存或发布任何信号量失败。
   */
  template <class MessageType>
  void PublishList(const std::vector<std::string>& topic_names, const std::string& shm_name, const std::vector<MessageType>& msgs) {
    WriteDataToSHM(shm_name, msgs);
    for (const auto& topic : topic_names) {
      PublishSem(topic);
    }
  }

  /**
   * @brief 订阅指定主题并使用回调处理接收的消息。
   *
   * 等待与 `topic_name` 关联的信号量，读取共享内存段 `shm_name` 中的消息，
   * 解码它，并使用解码后的消息调用提供的 `callback`。
   *
   * @tparam MessageType 订阅的消息类型。必须支持 `decode` 方法。
   * @tparam Callback 处理接收消息的回调函数类型。
   * @param topic_name 要订阅的主题名。
   * @param shm_name 共享内存段的名称。
   * @param callback 处理接收消息的回调函数。
   *
   * @throws std::runtime_error 如果访问共享内存或信号量失败。
   */
  template <class MessageType, typename Callback>
  void Subscribe(const std::string& topic_name, const std::string& shm_name, Callback callback) {
    CheckSemExist(topic_name);
    sem_map_.at(topic_name)->Decrement();
    CheckSHMExist(shm_name, false);
    MessageType msg;
    rclcpp::Serialization<MessageType> serializer;
    shm_map_.at(shm_name)->Lock();

    // 直接使用共享内存中的数据作为序列化消息的缓冲区
    rclcpp::SerializedMessage serialized_msg{rmw_get_zero_initialized_serialized_message()};
    serialized_msg.get_rcl_serialized_message().buffer = shm_map_.at(shm_name)->Get();
    serialized_msg.get_rcl_serialized_message().buffer_length = shm_map_.at(shm_name)->GetSize();
    serialized_msg.get_rcl_serialized_message().buffer_capacity = shm_map_.at(shm_name)->GetSize();

    // 零拷贝反序列化
    serializer.deserialize_message(&serialized_msg, &msg);

    // 避免序列化消息析构时释放共享内存
    serialized_msg.get_rcl_serialized_message().buffer = nullptr;
    serialized_msg.get_rcl_serialized_message().buffer_length = 0;
    serialized_msg.get_rcl_serialized_message().buffer_capacity = 0;

    shm_map_.at(shm_name)->UnLock();
    callback(msg);
  }

  /**
   * @brief 尝试订阅指定主题而不阻塞。
   *
   * 尝试减少与 `topic_name` 关联的信号量。如果成功，则从共享内存段 `shm_name` 中读取并解码消息，
   * 并使用解码后的消息调用提供的 `callback`。
   *
   * @tparam MessageType 订阅的消息类型。必须支持 `decode` 方法。
   * @tparam Callback 处理接收消息的回调函数类型。
   * @param topic_name 要订阅的主题名。
   * @param shm_name 共享内存段的名称。
   * @param callback 处理接收消息的回调函数。
   */
  template <class MessageType, typename Callback>
  void SubscribeNoWait(const std::string& topic_name, const std::string& shm_name, Callback callback) {
    CheckSemExist(topic_name);
    if (sem_map_.at(topic_name)->TryDecrement()) {
      CheckSHMExist(shm_name, false);
      MessageType msg;
      rclcpp::Serialization<MessageType> serializer;
      shm_map_.at(shm_name)->Lock();

      // 直接使用共享内存中的数据作为序列化消息的缓冲区
      rclcpp::SerializedMessage serialized_msg{rmw_get_zero_initialized_serialized_message()};
      serialized_msg.get_rcl_serialized_message().buffer = shm_map_.at(shm_name)->Get();
      serialized_msg.get_rcl_serialized_message().buffer_length = shm_map_.at(shm_name)->GetSize();
      serialized_msg.get_rcl_serialized_message().buffer_capacity = shm_map_.at(shm_name)->GetSize();

      // 零拷贝反序列化
      serializer.deserialize_message(&serialized_msg, &msg);

      // 避免序列化消息析构时释放共享内存
      serialized_msg.get_rcl_serialized_message().buffer = nullptr;
      serialized_msg.get_rcl_serialized_message().buffer_length = 0;
      serialized_msg.get_rcl_serialized_message().buffer_capacity = 0;

      shm_map_.at(shm_name)->UnLock();
      callback(msg);
    }
  }

  /**
   * @brief 订阅指定主题并设置超时时间。
   *
   * 等待与 `topic_name` 关联的信号量，并在超时时间内读取共享内存段 `shm_name` 中的消息，
   * 解码它，并使用解码后的消息调用提供的 `callback`。
   *
   * @tparam MessageType 订阅的消息类型。必须支持 `decode` 方法。
   * @tparam Callback 处理接收消息的回调函数类型。
   * @param topic_name 要订阅的主题名。
   * @param shm_name 共享内存段的名称。
   * @param callback 处理接收消息的回调函数。
   * @param timeout 等待的超时时间（毫秒）。
   */
  template <class MessageType, typename Callback>
  void SubscribeTimeout(const std::string& topic_name, const std::string& shm_name, Callback callback, int timeout) {
    CheckSemExist(topic_name);
    if (sem_map_.at(topic_name)->DecrementTimeout(timeout)) {
      CheckSHMExist(shm_name, false);
      MessageType msg;
      rclcpp::Serialization<MessageType> serializer;
      shm_map_.at(shm_name)->Lock();

      // 直接使用共享内存中的数据作为序列化消息的缓冲区
      rclcpp::SerializedMessage serialized_msg{rmw_get_zero_initialized_serialized_message()};
      serialized_msg.get_rcl_serialized_message().buffer = shm_map_.at(shm_name)->Get();
      serialized_msg.get_rcl_serialized_message().buffer_length = shm_map_.at(shm_name)->GetSize();
      serialized_msg.get_rcl_serialized_message().buffer_capacity = shm_map_.at(shm_name)->GetSize();

      // 零拷贝反序列化
      serializer.deserialize_message(&serialized_msg, &msg);

      // 避免序列化消息析构时释放共享内存
      serialized_msg.get_rcl_serialized_message().buffer = nullptr;
      serialized_msg.get_rcl_serialized_message().buffer_length = 0;
      serialized_msg.get_rcl_serialized_message().buffer_capacity = 0;

      shm_map_.at(shm_name)->UnLock();
      callback(msg);
    }
  }

 private:
  /**
   * @brief 将消息写入共享内存段。
   *
   * 将 `msg` 编码到由 `shm_name` 标识的共享内存段中。
   *
   * @tparam MessageType 要写入的消息类型。必须支持 `encode` 和 `getEncodedSize` 方法。
   * @param shm_name 共享内存段的名称。
   * @param msg 指向要写入的消息的指针。
   *
   * @throws std::runtime_error 如果写入共享内存失败。
   */
  template <class MessageType>
  void WriteDataToSHM(const std::string& shm_name, const MessageType& msg) {
    rclcpp::Serialization<MessageType> serializer;
    rclcpp::SerializedMessage serialized_msg;
    serializer.serialize_message(&msg, &serialized_msg);
    int datalen = serialized_msg.size();
    CheckSHMExist(shm_name, true, datalen);
    shm_map_.at(shm_name)->Lock();
    std::memcpy(shm_map_.at(shm_name)->Get(), serialized_msg.get_rcl_serialized_message().buffer, datalen);
    shm_map_.at(shm_name)->UnLock();
  }

  /**
   * @brief 信号量通知主题。
   *
   * 如果指定的 `topic_name` 当前值为零，则增加其信号量。
   *
   * @param topic_name 要通知的主题名。
   *
   * @throws std::runtime_error 如果信号量通知失败。
   */
  void PublishSem(const std::string& topic_name) {
    CheckSemExist(topic_name);
    sem_map_.at(topic_name)->IncrementWhenZero();
  }

  /**
   * @brief 确保共享内存段存在。
   *
   * 如果由 `shm_name` 标识的共享内存段不存在，则创建一个新的。
   * 如果 `check_size` 为真，则检查大小是否符合预期。
   *
   * @param shm_name 共享内存段的名称。
   * @param check_size 是否检查共享内存段的大小。
   * @param size 共享内存段的预期大小（以字节为单位）。如果 `check_size` 为真，则需要提供。
   *
   * @throws std::runtime_error 如果创建或访问共享内存失败。
   */
  void CheckSHMExist(const std::string& shm_name, bool check_size, int size = 0) {
    if (shm_map_.find(shm_name) == shm_map_.end()) {
      shm_map_.emplace(shm_name, std::make_shared<SharedMemoryData<uint8_t>>(shm_name, check_size, size));
    }
  }

  /**
   * @brief 确保主题的信号量存在。
   *
   * 如果与 `topic_name` 关联的信号量不存在，则创建一个新的。
   *
   * @param topic_name 要确保的主题名。
   *
   * @throws std::runtime_error 如果创建或访问信号量失败。
   */
  void CheckSemExist(const std::string& topic_name) {
    if (sem_map_.find(topic_name) == sem_map_.end()) {
      sem_map_.emplace(topic_name, std::make_shared<SharedMemorySemaphore>(topic_name, 0));
    }
  }

  std::unordered_map<std::string, std::shared_ptr<SharedMemoryData<uint8_t>>> shm_map_; /**< 共享内存段的名称键映射。 */
  std::unordered_map<std::string, std::shared_ptr<SharedMemorySemaphore>> sem_map_;     /**< 主题名称键的信号量映射。 */
};

}  // namespace ocm
