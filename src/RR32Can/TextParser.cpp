#include "RR32Can/TextParser.h"

namespace RR32Can {

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
  // handleValue(section, key, value);
  RR32CanValueHandler(section, key, value);
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

TextParser::Result TextParser::consumeToCharacter(uint8_t offset, char character, BufferManager & destinationBuffer) {
	Result result;
	result.consumed =
		buffer.findFirstOf(kSectionStop, offset);
	result.success = result.consumed != BufferManager::npos;
	BufferManager partial;
	uint8_t pushedBytes = 0;
	if (result.success) {
		// Section end found
		partial = buffer.subBufferManager(offset, result.consumed - 1);
		pushedBytes = destinationBuffer.push_back(partial);
		
	}
	else {

		// Section end not contained in the current buffer. Consume it completely
		partial = buffer.subBufferManager(offset);
		result.consumed = buffer.length();
	}

	pushedBytes = destinationBuffer.push_back(partial);
	if (pushedBytes < partial.length()) {
		// found token did not fit
		reportParseError();
		result.success = false;
		return result;
	}
}

uint8_t TextParser::consumeBuffer() {
  uint8_t consumedBytes = 0;

  while (consumedBytes < buffer.length()) {
	  Result parseResult;
    switch (parserState) {
      case State::LOOKING_FOR_SECTION_START: {
		  parseResult = consumeToCharacter(consumedBytes, kSectionStart);
		  if (parseResult.success) {
			  parserState = State::PARSING_SECTION;
		  }
		  consumedBytes = parseResult.consumed;
        break;
      }
      case State::PARSING_SECTION: {
		  parseResult = consumeToCharacter(consumedBytes, kSectionStart);
		  if (parseResult.success) {
			  parserState = State::LOOKING_FOR_KEY_START;
		  }
		  consumedBytes = parseResult.consumed;
		  break;
      }
	  case State::LOOKING_FOR_KEY_START: {
		  parseResult = consumeToCharacter(consumedBytes, kKeyStart);
		  if (parseResult.success) {
			  parserState = State::PARSING_KEY;
		  }
		  consumedBytes = parseResult.consumed;
		  break;
	  }
	  case State::PARSING_KEY: {
		  parseResult = consumeToCharacter(consumedBytes, kKeyStop);
		  if (parseResult.success) {
			  parserState = State::PARSING_KEY;
		  }
		  consumedBytes = parseResult.consumed;
		  break;
	  }

	  case State::LOOKING_FOR_VALUE_START: {
		  parseResult = consumeToCharacter(consumedBytes, k);
		  if (parseResult.success) {
			  parserState = State::PARSING_KEY;
		  }
		  consumedBytes = parseResult.consumed;
		  break;
	  }
	  case State::PARSING_VALUE: {
		  parseResult = consumeToCharacter(consumedBytes, kValueStop);
		  if (parseResult.success) {
			  parserState = State::PARSING_KEY;
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

} /* namespace RR32Can */
