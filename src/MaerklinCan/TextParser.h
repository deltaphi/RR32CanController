#ifndef __MAERKLINCAN__TEXTPARSER_H__
#define __MAERKLINCAN__TEXTPARSER_H__

#include <Arduino.h>

#include "MaerklinCan/BufferManager.h"

namespace MaerklinCan {

class TextParser {
 public:
  using size_type = BufferManager::size_type;

  /**
   * \brief Length of the parse buffer in characters
   */
  constexpr static size_type kBufferLength = 30;

  constexpr static size_type kSectionLength = 10;
  constexpr static size_type kKeyLength = 10;
  constexpr static size_type kValueLength = 25;

  constexpr static char kSectionStart = '[';
  constexpr static char kSectionStop = ']';
  constexpr static char kKeyStart = '.';
  constexpr static char kKeyStop = '=';

  constexpr static char kValueStop = ']';

  TextParser();

  void reset();
  void addText(const BufferManager& input);
  void handleValue(const BufferManager& section, const BufferManager& key,
                   const BufferManager& value);

 private:
  size_type consumeBuffer();
  void reportParseError();

  enum class State {
    LOOKING_FOR_SECTION_START = 0,
    PARSING_SECTION,
    LOOKING_FOR_KEY_START,
    PARSING_KEY,
    LOOKING_FOR_VALUE_START,
    PARSING_VALUE,
    ERROR
  };

  State parserState;

  BufferManager::value_type rawbuffer[kBufferLength + 1];
  BufferManager buffer;

  BufferManager::value_type rawsection[kSectionLength + 1];
  BufferManager section;

  BufferManager::value_type rawkey[kKeyLength + 1];
  BufferManager key;

  BufferManager::value_type rawvalue[kValueLength + 1];
  BufferManager value;
};

} /* namespace MaerklinCan */

#endif