#include "MaerklinCan/TextParser.h"

namespace MaerklinCan {

TextParser::TextParser()
    : buffer(rawbuffer, kBufferLength + 1),
      section(rawsection, kSectionLength + 1),
      key(rawkey, kKeyLength + 1),
      value(rawvalue, kValueLength + 1) {
  reset();
}

void TextParser::reset() {
  parserState = State::LOOKING_FOR_SECTION_START;
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
  handleValue(section, key, value);
  reset();
}

void TextParser::addText(const BufferManager& input) {
  if (input.length() > kBufferLength) {
    // input won't fit.
    // TODO: Better checking, if the input can be handled in chunks
    reportParseError();
    return;
  }

  uint8_t inputConsumed = 0;

  while (inputConsumed < input.length()) {

    if (buffer.full()) {
      // Buffer full at the start of a parse - no progress wil be made.
      reportParseError();
      return;
    }

    inputConsumed += buffer.push_back(input.subBufferManager(inputConsumed));

    uint8_t consumedBufferBytes = consumeBuffer();

    buffer.pop_front(consumedBufferBytes);
  }
}

uint8_t TextParser::consumeBuffer() {
  uint8_t consumedBytes = 0;
// TODO
  return consumedBytes;
}

} /* namespace MaerklinCan */
