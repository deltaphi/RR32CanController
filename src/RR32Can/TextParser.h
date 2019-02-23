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
    LOOKING_FOR_KEY_OR_SECTION_START = 0,
    PARSING_SECTION,
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

  constexpr static const char* kKeyOrSectionStart = ".[";
  constexpr static char kSectionStart = '[';
  constexpr static const char* kSectionStop = "]";
  constexpr static char kKeyStart = '.';
  constexpr static const char* kKeyStop = "=";

  constexpr static const char* kValueStop = "\n";

  TextParser();

  /// Erase all buffers and set the parser to the initial state
  void reset();

  /// Add an additional parser and run the state machine on all contained bytes
  void addText(const BufferManager& input);

  State getState() const { return parserState; }

 public:
  /// Run the statemachine on the current buffer
  size_type processBuffer();

  /// Set the parser to the error state
  void reportParseError();

  struct FindTokenResult {
    /// Number of characters that were consumed
    uint8_t consumed;
    /// Whether any of the relevant characters was actually found
    bool success;
  };

  /// Look for a state-changing character in the buffer and copy all matching
  /// characters to the destination.
  FindTokenResult findToken(uint8_t offset, const char* stateChangingCharacters,
                            BufferManager* destinationBuffer);

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