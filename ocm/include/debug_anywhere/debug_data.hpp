
#pragma once

#include <lcm/lcm_coretypes.h>

#include <vector>

namespace ocm {

class DebugData {
 public:
  int8_t count;

  std::vector<double> data;

 public:
  /**
   * 将消息编码成二进制形式。
   *
   * @param buf 输出缓冲区。
   * @param offset 编码开始的 @p buf 中的字节偏移量。
   * @param maxlen 要写入的最大字节数。通常应等于 getEncodedSize()。
   * @return 编码的字节数，或在出错时返回 <0。
   */
  inline int encode(void *buf, int offset, int maxlen) const;

  /**
   * 检查编码此消息所需的字节数。
   */
  inline int getEncodedSize() const;

  /**
   * 从二进制形式解码消息到此实例。
   *
   * @param buf 包含编码消息的缓冲区。
   * @param offset 编码消息在 @p buf 中开始的字节偏移量。
   * @param maxlen 解码时要读取的最大字节数。
   * @return 解码的字节数，或在发生错误时返回 <0。
   */
  inline int decode(const void *buf, int offset, int maxlen);

  /**
   * 获取标识消息结构的64位指纹。
   * 注意，该指纹对相同消息类型的所有实例都是相同的，并且是基于消息类型定义的指纹，而不是消息内容的指纹。
   */
  inline static int64_t getHash();

  /**
   * 返回 "DebugData"
   */
  inline static const char *getTypeName();

  // LCM support functions. Users should not call these
  // LCM 支持函数。用户不应调用这些函数。
  inline int _encodeNoHash(void *buf, int offset, int maxlen) const;
  inline int _getEncodedSizeNoHash() const;
  inline int _decodeNoHash(const void *buf, int offset, int maxlen);
  inline static uint64_t _computeHash(const __lcm_hash_ptr *p);
};

int DebugData::encode(void *buf, int offset, int maxlen) const {
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

int DebugData::decode(const void *buf, int offset, int maxlen) {
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

int DebugData::getEncodedSize() const { return 8 + _getEncodedSizeNoHash(); }

int64_t DebugData::getHash() {
  static int64_t hash = static_cast<int64_t>(_computeHash(NULL));
  return hash;
}

const char *DebugData::getTypeName() { return "DebugData"; }

int DebugData::_encodeNoHash(void *buf, int offset, int maxlen) const {
  int pos = 0, tlen;

  tlen = __int8_t_encode_array(buf, offset + pos, maxlen - pos, &this->count, 1);
  if (tlen < 0)
    return tlen;
  else
    pos += tlen;

  if (this->count > 0) {
    tlen = __double_encode_array(buf, offset + pos, maxlen - pos, &this->data[0], this->count);
    if (tlen < 0)
      return tlen;
    else
      pos += tlen;
  }

  return pos;
}

int DebugData::_decodeNoHash(const void *buf, int offset, int maxlen) {
  int pos = 0, tlen;

  tlen = __int8_t_decode_array(buf, offset + pos, maxlen - pos, &this->count, 1);
  if (tlen < 0)
    return tlen;
  else
    pos += tlen;

  if (this->count) {
    this->data.resize(this->count);
    tlen = __double_decode_array(buf, offset + pos, maxlen - pos, &this->data[0], this->count);
    if (tlen < 0)
      return tlen;
    else
      pos += tlen;
  }

  return pos;
}

int DebugData::_getEncodedSizeNoHash() const {
  int enc_size = 0;
  enc_size += __int8_t_encoded_array_size(NULL, 1);
  enc_size += __double_encoded_array_size(NULL, this->count);
  return enc_size;
}

uint64_t DebugData::_computeHash(const __lcm_hash_ptr *) {
  uint64_t hash = 0xe1ca5d5fccfef5e9LL;
  return (hash << 1) + ((hash >> 63) & 1);
}

}  // namespace ocm
