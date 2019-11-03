#include "RR32Can/util/TextParser.h"

namespace RR32Can {

TextParser::TextParser()
    : buffer(rawbuffer, kBufferLength + 1),
      section(rawsection, kSectionLength + 1),
      key(rawkey, kKeyLength + 1),
      value(rawvalue, kValueLength + 1) {
  reset();
}

void TextParser::reset() {
  parserState = State::LOOKING_FOR_KEY_OR_SECTION_START;
  consumer = nullptr;
  buffer.erase();
  section.erase();
  key.erase();
  value.erase();
}

void TextParser::reportParseError() {
  parserState = State::ERROR;
  constexpr static const char* msg = "ERROR";
  section.set(msg);
  key.set(msg);
  value.set(msg);
  if (consumer != nullptr) {
    consumer->consumeConfigData(section, key, value);
  }
}

void TextParser::addText(const BufferManager& input) {
  if (input.length() > kBufferLength || parserState == State::ERROR) {
    // input won't fit.
    // TODO: Better checking, if the input can be handled in chunks
    reportParseError();
    return;
  }

  uint8_t inputConsumed = 0;

  while (inputConsumed < input.length()) {
    if (buffer.full() || parserState == State::ERROR) {
      // Buffer full at the start of a parse - no progress wil be made.
      reportParseError();
      return;
    }

    inputConsumed += buffer.push_back(input.subBufferManager(inputConsumed));

    uint8_t consumedBufferBytes = this->processBuffer();

    buffer.pop_front(consumedBufferBytes);
  }
}

TextParser::size_type TextParser::processBuffer() {
  size_type consumedBytes = 0;

  while (consumedBytes < buffer.length()) {
    FindTokenResult parseResult;
    switch (parserState) {
      case State::LOOKING_FOR_KEY_OR_SECTION_START: {
        parseResult = findToken(consumedBytes, kKeyOrSectionStart, nullptr);
        if (parseResult.success) {
          if (buffer[parseResult.consumed - 1] == kSectionStart) {
            section.erase();
            parserState = State::PARSING_SECTION;
          } else if (buffer[parseResult.consumed - 1] == kKeyStart) {
            parserState = State::PARSING_KEY;
          } else {
            reportParseError();
            return consumedBytes;
          }
        }
        consumedBytes = parseResult.consumed;
        break;
      }
      case State::PARSING_SECTION: {
        parseResult = findToken(consumedBytes, kSectionStop, &section);
        if (parseResult.success) {
          parserState = State::LOOKING_FOR_KEY_OR_SECTION_START;
        }
        consumedBytes = parseResult.consumed;
        break;
      }
      case State::PARSING_KEY: {
        static const char matchString[] = {kKeyStop[0], kValueStop[0], '\0'};
        parseResult = findToken(consumedBytes, matchString, &key);
        if (parseResult.success) {
          if (parseResult.matchingChar == kKeyStop[0]) {
            parserState = State::PARSING_VALUE;
          } else if (parseResult.matchingChar == kValueStop[0]) {
            // Report the current key without a value
            value.erase();
            if (consumer != nullptr) {
              consumer->consumeConfigData(section, key, value);
            }
            key.erase();
            parserState = State::LOOKING_FOR_KEY_OR_SECTION_START;
          }
        }
        consumedBytes = parseResult.consumed;
        break;
      }
        /*
              case State::LOOKING_FOR_VALUE_START: {
                parseResult =
                    findToken(consumedBytes, kKeyStop,
                              nullptr);  // TBD: Do we even need to find here?
           This
                                         // might be the current or the next
           character. if (parseResult.success) { parserState =
           State::PARSING_KEY;
                }
                consumedBytes = parseResult.consumed;
                break;
              }*/
      case State::PARSING_VALUE: {
        parseResult = findToken(consumedBytes, kValueStop, &value);
        if (parseResult.success) {
          if (consumer != nullptr) {
            consumer->consumeConfigData(section, key, value);
          }
          key.erase();
          value.erase();
          parserState = State::LOOKING_FOR_KEY_OR_SECTION_START;
        }
        consumedBytes = parseResult.consumed;
        break;
      }
      default:
        // TBD: Consume everything.
        consumedBytes = buffer.length();
    }
  }

  return consumedBytes;
}

TextParser::FindTokenResult TextParser::findToken(
    uint8_t offset, const char* chars, BufferManager* destinationBuffer) {
  FindTokenResult result;
  result.consumed = buffer.findFirstOf(chars, offset);

  if (result.consumed == BufferManager::npos) {
    result.success = false;
    result.consumed = buffer.length();
    result.matchingChar = '\0';
  } else {
    result.success = true;
    result.matchingChar = buffer[result.consumed];
  }

  if (destinationBuffer != nullptr) {
    BufferManager partial = buffer.subBufferManager(offset, result.consumed);
    size_type pushedBytes = destinationBuffer->push_back(partial);
    if (pushedBytes < partial.length()) {
      // found token did not fit
      reportParseError();
      result.success = false;
    }
  }

  if (result.success) {
    result.consumed += 1;  // convert offset from find to length
  }

  return result;
}

} /* namespace RR32Can */
