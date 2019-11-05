#ifndef SRC_RR32CAN_UTIL_BUFFERMANAGER_H_
#define SRC_RR32CAN_UTIL_BUFFERMANAGER_H_

#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace RR32Can {

class BufferManager {
 public:
  using value_type = char;
  using size_type = uint8_t;

  constexpr static const size_type npos = -1;

  constexpr BufferManager() : BufferManager(nullptr, 0, 0) {}

  constexpr BufferManager(value_type* buffer, size_type bufferLength)
      : BufferManager(buffer, bufferLength, 0) {}

  constexpr BufferManager(value_type* buffer, size_type bufferLength,
                          size_type currentBufferLength)
      : buffer(buffer),
        currentBufferLength(currentBufferLength),
        maxBufferLength(bufferLength) {}

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

  bool strncmp(const char* other) {
    return 0 == (::strncmp(buffer, other, currentBufferLength));
  }

  /*
   * \brief Get a reference to the character at the index.
   *
   * If index is outside of the currently valid range of elements, it is limited
   * to the last element.
   */
  value_type& at(size_type index) { return buffer[limitIndex(index)]; }

  size_type findFirstOf(value_type character, size_type offset = 0) const;
  size_type findFirstOf(const value_type* chars, size_type offset = 0) const;

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

  uint8_t asUint8() const { return strtol(buffer, nullptr, 10); }

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
   * \brief Add a single character to the end of the buffer.
   *
   * If the buffer is full, it remains unmodified.
   *
   * \return The number of characters added to the buffer.
   */
  size_type push_back(const char c) {
    if (currentBufferLength < maxBufferLength) {
      buffer[currentBufferLength] = c;
      ++currentBufferLength;
      return 1;
    } else {
      return 0;
    }
  }

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
  BufferManager subBufferManager(size_type start_offset) const;
  BufferManager subBufferManager(size_type start_offset,
                                 size_type end_offset) const;

 private:
  value_type* buffer;
  size_type currentBufferLength;
  size_type maxBufferLength;
};

} /* namespace RR32Can */

#endif  // SRC_RR32CAN_UTIL_BUFFERMANAGER_H_
