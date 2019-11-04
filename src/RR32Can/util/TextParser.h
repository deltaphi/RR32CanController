#ifndef SRC_RR32CAN_UTIL_TEXTPARSER_H_
#define SRC_RR32CAN_UTIL_TEXTPARSER_H_

#include <cstdint>

#include "RR32Can/util/BufferManager.h"

// Forward-declarations so that we can friend testcases
class TextParserFixture;
class TextParserFixture_FindToken_01_Test;

namespace RR32Can {

// Forward-Declarations for test classes in namespace, so that they can be
// friended later.
class TextParserFixture;
class TextParserFixture_FindToken_01_Test;

class TextParserConsumer {
 public:
  virtual ~TextParserConsumer() = default;
  virtual void consumeConfigData(BufferManager& section, BufferManager& key,
                                 BufferManager& value) = 0;
};

/**
 * \brief Converts a text stream into events consisting of (section, key,
 * value).
 *
 * Note that for "lokomotive", additional subsections can be encountered.
 * Each subsection will be reported as a (section, key, EMPTY) tuple.
 * Each key in the subsection will be reported as usual.
 */
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

  void setConsumer(TextParserConsumer* consumer) { this->consumer = consumer; }

  /// Erase all buffers and set the parser to the initial state
  void reset();

  /// Add an additional parser and run the state machine on all contained bytes
  void addText(const BufferManager& input);

  State getState() const { return parserState; }

 private:
  /// Run the statemachine on the current buffer
  size_type processBuffer();

  /// Set the parser to the error state
  void reportParseError();

  struct FindTokenResult {
    /// Number of characters that were consumed
    uint8_t consumed;
    /// Whether any of the relevant characters was actually found
    bool success;
    /**
     * \brief The character that was matched in finishing the token.
     *
     * Only valid when success == true;
     */
    char matchingChar;
  };

  /// Look for a state-changing character in the buffer and copy all matching
  /// characters to the destination.
  FindTokenResult findToken(uint8_t offset, const char* stateChangingCharacters,
                            BufferManager* destinationBuffer);

  State parserState;
  TextParserConsumer* consumer = nullptr;

  BufferManager::value_type rawbuffer[kBufferLength + 1];
  BufferManager buffer;

  BufferManager::value_type rawsection[kSectionLength + 1];
  BufferManager section;

  BufferManager::value_type rawkey[kKeyLength + 1];
  BufferManager key;

  BufferManager::value_type rawvalue[kValueLength + 1];
  BufferManager value;

  friend class ::RR32Can::TextParserFixture;
  friend class ::RR32Can::TextParserFixture_FindToken_01_Test;
};

} /* namespace RR32Can */

#endif  // SRC_RR32CAN_UTIL_TEXTPARSER_H_
