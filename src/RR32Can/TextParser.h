#ifndef __RR32Can__TEXTPARSER_H__
#define __RR32Can__TEXTPARSER_H__

#include <cstdint>

#include "RR32Can/BufferManager.h"

extern void RR32CanValueHandler(const RR32Can::BufferManager& section,
                                    const RR32Can::BufferManager& key,
                                    const RR32Can::BufferManager& value);

class TextParserFixture;

namespace RR32Can {

class TextParser {
 public:
  using size_type = BufferManager::size_type;

  enum class State {
    LOOKING_FOR_SECTION_START = 0,
    PARSING_SECTION,
    LOOKING_FOR_KEY_START,
    PARSING_KEY,
    LOOKING_FOR_VALUE_START,
    PARSING_VALUE,
    ERROR
  };

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
  // void handleValue(const BufferManager& section, const BufferManager& key,
  //                 const BufferManager& value);

  State getState() const { return parserState; }

 private:
  size_type consumeBuffer();
  void reportParseError();

  struct Result {
	  uint8_t consumed;
	  bool success;
  };

  Result consumeToCharacter(uint8_t offset, char character);

  State parserState;

  BufferManager::value_type rawbuffer[kBufferLength + 1];
  BufferManager buffer;

  BufferManager::value_type rawsection[kSectionLength + 1];
  BufferManager section;

  BufferManager::value_type rawkey[kKeyLength + 1];
  BufferManager key;

  BufferManager::value_type rawvalue[kValueLength + 1];
  BufferManager value;

  friend class TextParserFixture;
};

} /* namespace RR32Can */

#endif