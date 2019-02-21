#include <cstring>

#include "MaerklinCan/BufferManager.h"

namespace MaerklinCan {

BufferManager::size_type BufferManager::findFirstOf(value_type character,
                                                    size_type offset) const {
  if (offset >= currentBufferLength) {
    return npos;
  }
  for (size_type i = offset; i < currentBufferLength; ++i) {
    if (buffer[i] == character) {
      return i;
    }
  }
  return npos;
}

void BufferManager::erase() {
  memset(buffer, '\0', maxBufferLength);
  currentBufferLength = 0;
}

/**
 * \brief Remove the first num_elements elements and move the remainder of the
 * buffer to the front
 */
void BufferManager::pop_front(size_type num_elements) {
  if (num_elements >= currentBufferLength) {
    erase();
  } else {
    currentBufferLength -= num_elements;
    memmove(buffer, buffer + num_elements, currentBufferLength);
    memset(buffer + currentBufferLength, '\0', capacity_remaining());
  }
}

BufferManager::size_type BufferManager::push_back(
    const BufferManager& otherBuffer) {
  size_type bytesToCopy = this->capacity_remaining() < otherBuffer.length()
                              ? this->capacity_remaining()
                              : otherBuffer.length();
  memcpy(buffer + currentBufferLength, otherBuffer.data(), bytesToCopy);
  currentBufferLength += bytesToCopy;
  return bytesToCopy;
}

void BufferManager::set(const char* str) {
  erase();
  strncpy(buffer, str, maxBufferLength - 1);  // ensure a 0-byte
  currentBufferLength = strlen(buffer);
}

BufferManager::size_type BufferManager::move_back(BufferManager& otherBuffer) {
  auto movedBytes = this->push_back(otherBuffer);
  otherBuffer.pop_front(movedBytes);
  return movedBytes;
}

BufferManager BufferManager::subBufferManager(size_type offset) const {
  if (offset >= maxBufferLength) {
    offset = maxBufferLength;
  }
  BufferManager mgr(buffer + offset, maxBufferLength - offset);
  if (currentBufferLength >= offset) {
    mgr.currentBufferLength = currentBufferLength - offset;
  }
  return mgr;
}

} /* namespace MaerklinCan */
