#ifndef __MAERKLINCAN__BUFFERMANAGER_H__
#define __MAERKLINCAN__BUFFERMANAGER_H__

#include <cstdint>

namespace MaerklinCan {

class BufferManager {
 public:
  using value_type = char;
  using size_type = uint8_t;

  constexpr static const size_type npos = -1;

  constexpr BufferManager(value_type* buffer, size_type bufferLength)
      : buffer(buffer), currentBufferLength(0), maxBufferLength(bufferLength) {}

  BufferManager(const BufferManager&) = delete;
  BufferManager& operator=(const BufferManager&) = delete;
  BufferManager(BufferManager&&) = default;
  BufferManager& operator=(BufferManager&&) = default;

  ~BufferManager() = default;

  size_type limitIndex(size_type index) const {
    return index >= currentBufferLength ? currentBufferLength - 1 : index;
  }

  value_type operator[](size_type index) const {
    return buffer[limitIndex(index)];
  }

  /*
   * \brief Get a reference to the character at the index.
   *
   * If index is outside of the currently valid range of elements, it is limited
   * to the last element.
   */
  value_type& at(size_type index) { return buffer[limitIndex(index)]; }

  size_type findFirstOf(value_type character, size_type offset = 0) const;

  void erase();

  /**
   * \brief Remove the first num_elements elements and move the remainder of the
   * buffer to the front
   */
  void pop_front(size_type num_elements);

  constexpr size_type capacity() const { return maxBufferLength; }

  constexpr size_type capacity_remaining() const {
    return maxBufferLength - currentBufferLength;
  }

  constexpr bool full() const { return capacity_remaining() == 0; }

  constexpr bool empty() const { return length() == 0; }

  constexpr size_type length() const { return currentBufferLength; }

  constexpr value_type* data() const { return buffer; }

  /*
   * \brief Read as many bytes as possible from the otherBuffer.
   *
   * There may be unread bytes left in the otherBuffer.
   * Does not modify otherBuffer.
   *
   * \return The number of bytes actually read.
   */
  size_type push_back(const BufferManager& otherBuffer);

  /*
   * \brief Set the buffer to a given string value.
   *
   * Note that the BufferManager will refuse to set the last byte using this
   * method - it will always be a '\0'
   */
  void set(const char* str);

  size_type move_back(BufferManager& otherBuffer);

  /**
   * \brief Creates a view-buffer on a section of this buffer.
   *
   * Do not use the returned buffer for modificatoins of the underlying memory.
   * Results will definitely be surprising.
   */
  BufferManager subBufferManager(size_type offset) const;

 private:
  value_type* buffer;
  size_type currentBufferLength;
  const size_type maxBufferLength;
};

} /* namespace MaerklinCan */

#endif