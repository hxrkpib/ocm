#pragma once

#include <lcm/lcm_coretypes.h>

#include <string>

namespace ocm {

/**
 * @class DesiredGroupData
 * @brief 描述期望组的数据类
 *
 * @details
 * DesiredGroupData 类负责对期望组的信息进行编码和解码操作。
 */
class DesiredGroupData {
 public:
  std::string desired_group;

 public:
  /**
   * @brief 将消息转换为二进制格式。
   *
   * @param buf 输出缓冲区指针。
   * @param offset 编码起始的字节偏移量。
   * @param maxlen 允许写入的最大字节数，通常应等于 getEncodedSize()。
   * @return 编码后的字节数，若发生错误则返回小于0的值。
   */
  inline int encode(void *buf, int offset, int maxlen) const;

  /**
   * @brief 获取编码此消息所需的总字节数。
   */
  inline int getEncodedSize() const;

  /**
   * @brief 从二进制数据解码消息到当前实例。
   *
   * @param buf 包含编码消息的缓冲区。
   * @param offset 编码消息在 buf 中的起始字节偏移量。
   * @param maxlen 解码时允许读取的最大字节数。
   * @return 解码的字节数，若出错则返回小于0的值。
   */
  inline int decode(const void *buf, int offset, int maxlen);

  /**
   * @brief 获取用于标识消息结构的64位哈希值。
   *
   * @details
   * 此哈希值对于同一消息类型的所有实例是一致的，基于消息类型定义而非内容。
   */
  inline static int64_t getHash();

  /**
   * @brief 返回消息类型的名称 "DesiredGroupData"。
   */
  inline static const char *getTypeName();

  // LCM 支持函数。用户不应调用这些函数。
  inline int _encodeNoHash(void *buf, int offset, int maxlen) const;
  inline int _getEncodedSizeNoHash() const;
  inline int _decodeNoHash(const void *buf, int offset, int maxlen);
  inline static uint64_t _computeHash(const __lcm_hash_ptr *p);
};

int DesiredGroupData::encode(void *buf, int offset, int maxlen) const {
  int pos = 0, tlen;
  int64_t hash = getHash();

  tlen = __int64_t_encode_array(buf, offset + pos, maxlen - pos, &hash, 1);
  if (tlen < 0)
    return tlen;
  else
    pos += tlen;

  tlen = this->_encodeNoHash(buf, offset + pos, maxlen - pos);
  if (tlen < 0)
    return tlen;
  else
    pos += tlen;

  return pos;
}

int DesiredGroupData::decode(const void *buf, int offset, int maxlen) {
  int pos = 0, thislen;

  int64_t msg_hash;
  thislen = __int64_t_decode_array(buf, offset + pos, maxlen - pos, &msg_hash, 1);
  if (thislen < 0)
    return thislen;
  else
    pos += thislen;
  if (msg_hash != getHash()) return -1;

  thislen = this->_decodeNoHash(buf, offset + pos, maxlen - pos);
  if (thislen < 0)
    return thislen;
  else
    pos += thislen;

  return pos;
}

int DesiredGroupData::getEncodedSize() const { return 8 + _getEncodedSizeNoHash(); }

int64_t DesiredGroupData::getHash() {
  static int64_t hash = static_cast<int64_t>(_computeHash(NULL));
  return hash;
}

const char *DesiredGroupData::getTypeName() { return "DesiredGroupData"; }

int DesiredGroupData::_encodeNoHash(void *buf, int offset, int maxlen) const {
  int pos = 0, tlen;

  char *desired_group_cstr = const_cast<char *>(this->desired_group.c_str());
  tlen = __string_encode_array(buf, offset + pos, maxlen - pos, &desired_group_cstr, 1);
  if (tlen < 0)
    return tlen;
  else
    pos += tlen;

  return pos;
}

int DesiredGroupData::_decodeNoHash(const void *buf, int offset, int maxlen) {
  int pos = 0, tlen;

  int32_t __desired_group_len__;
  tlen = __int32_t_decode_array(buf, offset + pos, maxlen - pos, &__desired_group_len__, 1);
  if (tlen < 0)
    return tlen;
  else
    pos += tlen;
  if (__desired_group_len__ > maxlen - pos) return -1;
  this->desired_group.assign(static_cast<const char *>(buf) + offset + pos, __desired_group_len__ - 1);
  pos += __desired_group_len__;

  return pos;
}

int DesiredGroupData::_getEncodedSizeNoHash() const {
  int enc_size = 0;
  enc_size += this->desired_group.size() + 4 + 1;
  return enc_size;
}

uint64_t DesiredGroupData::_computeHash(const __lcm_hash_ptr *) {
  uint64_t hash = 0xe21c68d2e41b458dLL;
  return (hash << 1) + ((hash >> 63) & 1);
}

}  // namespace ocm
