#include "hal/DisplayDriver.h"
#include "hal/UIAssets.h"

#define FONT_HEIGHT (16u)

#if (FONT_HEIGHT == 16)
#define FONT_PTR (ArialMT_Plain_16)
#elif (FONT_HEIGHT == 24)
#define FONT_PTR (ArialMT_Plain_24)
#else
#error "Unknown FONT_HEIGHT configured".
#endif

#define SYMBOL_FONT_HEIGHT (10u)

#if (SYMBOL_FONT_HEIGHT == 8)
#define SYMBOL_FONT_PTR (hal::RR32Symbols_8)
#elif (SYMBOL_FONT_HEIGHT == 10)
#define SYMBOL_FONT_PTR (hal::RR32Symbols_10)
#else
#error "Unknown SYMBOL_FONT_HEIGHT configured".
#endif

namespace hal {

constexpr const uint8_t statusFontHeight = SYMBOL_FONT_PTR[1];
constexpr const uint8_t statusLinePixelDistance = 1;
constexpr const uint8_t textLinePixelDistance = 2;

constexpr const uint8_t textLineFirst = 1;
constexpr const uint8_t textLineLast = 2;

const uint8_t DisplayDriver::voffset[] = {0, 0 + statusFontHeight + statusLinePixelDistance,
                                          static_cast<uint8_t>(FONT_HEIGHT) +
                                              textLinePixelDistance + statusFontHeight +
                                              statusLinePixelDistance,
                                          63 - statusFontHeight};

const uint8_t DisplayDriver::baselineOffset[] = {
    0, static_cast<uint8_t>(FONT_HEIGHT + voffset[1]),
    static_cast<uint8_t>((2u * FONT_HEIGHT) + voffset[1] + textLinePixelDistance), 0};

void DisplayDriver::begin() {
  printf("Starting Display Driver\n");
  // Initialize the Display
  display.init();
#if (DISPLAY_FLIP_SCREEN == STD_ON)
  display.flipScreenVertically();
#endif
}

void DisplayDriver::loop(application::model::DisplayModel& displayModel) {
  if (displayModel.updateRequired) {
    displayModel.updateRequired = false;
    display.clear();

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(FONT_PTR);

    for (uint8_t line = 0; line < DISPLAY_LINES; ++line) {
      display.drawString(0, voffset[line + textLineFirst], displayModel.buffer[line]);
      if (displayModel.isCursorEnabled() && displayModel.getCursorLine() == line) {
        display.drawHorizontalLine(0, baselineOffset[line + textLineFirst], display.getWidth());
      }
    }

    // Draw assets in the top line

    // WIFI Symbol
    if (displayModel.isWifi()) {
      display.setFont(SYMBOL_FONT_PTR);
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      constexpr const char topLineString[] = {static_cast<char>(hal::RR32SymbolNames::WIFI), '\0'};
      display.drawString(128, voffset[0], topLineString);
    }

    // CAN text
    if (displayModel.isCan()) {
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(128 - (SYMBOL_FONT_PTR[0] + 4 /* 4 pixels distance */), voffset[0], "CAN");
    }

    // STOP text
    if (!displayModel.getSystemOn()) {
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(128 / 2 /* center */, voffset[0], "-STOP-");
    }

    // Draw the function bits

    RR32Can::FunctionBits_t mask = 1;
    constexpr const uint8_t rectHeight = 4;
    constexpr const uint8_t rectWidth = 10;
    constexpr const uint8_t rectDistance = 6;

    const uint8_t rectvoffset = voffset[3] - 2 - rectHeight;

    for (uint8_t i = 0; i < 8; ++i) {
      bool functionOn = (displayModel.functionBits & mask) != 0;

      uint8_t recthoffset = (i * (rectWidth + rectDistance)) + (rectDistance / 2);
      if (functionOn) {
        display.fillRect(recthoffset, rectvoffset, rectWidth, rectHeight);
      } else {
        display.drawRect(recthoffset, rectvoffset, rectWidth, rectHeight);
      }

      mask <<= 1;
    }

    // Draw assets in the bottom line

    display.setFont(SYMBOL_FONT_PTR);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    if (displayModel.direction == RR32Can::EngineDirection::REVERSE) {
      display.drawString(0, voffset[3], "\2");
    } else {
      display.drawString(0, voffset[3], "\1");
    }

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    if (displayModel.direction == RR32Can::EngineDirection::FORWARD) {
      display.drawString(128, voffset[3], "\4");
    } else {
      display.drawString(128, voffset[3], "\3");
    }

    constexpr const uint8_t progressBarOffset = ((3 * (SYMBOL_FONT_PTR[0])) / 2);
    constexpr const uint8_t progressBarWidth = (128 - (2 * progressBarOffset));
    display.drawProgressBar(progressBarOffset, voffset[3] + 2, progressBarWidth, 6,
                            displayModel.speed);

    display.display();
  }
}
}  // namespace hal
