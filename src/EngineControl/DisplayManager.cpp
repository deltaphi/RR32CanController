#include "config.h"

#if (DISPLAY_ATTACHED == STD_ON)
#include "EngineControl/UIAssets.h"

#include "EngineControl/DisplayManager.h"

namespace EngineControl {

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
#define SYMBOL_FONT_PTR (RR32Symbols_8)
#elif (SYMBOL_FONT_HEIGHT == 10)
#define SYMBOL_FONT_PTR (RR32Symbols_10)
#else
#error "Unknown SYMBOL_FONT_HEIGHT configured".
#endif

constexpr const uint8_t statusFontHeight = SYMBOL_FONT_PTR[1];
constexpr const uint8_t statusLinePixelDistance = 1;
constexpr const uint8_t textLinePixelDistance = 2;

constexpr const uint8_t textLineFirst = 1;
constexpr const uint8_t textLineLast = 2;

const uint8_t DisplayManager::voffset[] = {0,
                                           0+statusFontHeight+statusLinePixelDistance,
                                           static_cast<uint8_t>(FONT_HEIGHT)+textLinePixelDistance+statusFontHeight+statusLinePixelDistance,
                                           63 - statusFontHeight};

const uint8_t DisplayManager::baselineOffset[] = {0, static_cast<uint8_t>(FONT_HEIGHT)+voffset[1], (2u*static_cast<uint8_t>(FONT_HEIGHT))+voffset[1], 0};

void DisplayManager::begin() {
  Serial.println("Starting Display");
  // Initialize the Display
  display.init();
#if (DISPLAY_FLIP_SCREEN == STD_ON)
  display.flipScreenVertically();
#endif
  memset(buffer, 0, sizeof(buffer));
  setSpeedValue(0);
  setDirection(Direction::UNKNOWN);
}

void DisplayManager::loop() {
  if (updateRequired) {
    updateRequired = false;
    display.clear();

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(FONT_PTR);

    for (uint8_t line = 0; line < DISPLAY_LINES; ++line) {
      display.drawString(0, voffset[line+textLineFirst], buffer[line]);
      if (cursorEnabled && cursorLine == line) {
        display.drawHorizontalLine(0, baselineOffset[line+textLineFirst], display.getWidth());
      }
    }

    // Draw assets in the top line
    
    // WIFI Symbol
    if (wifiOn) {
      display.setFont(SYMBOL_FONT_PTR);
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      constexpr const char topLineString[] = {static_cast<char>(RR32SymbolNames::WIFI), '\0'};
      display.drawString(128, voffset[0], topLineString);
    }

    // CAN text
    if (canOn) {
      display.setFont(ArialMT_Plain_10);    
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(128 - (SYMBOL_FONT_PTR[0] + 4 /* 4 pixels distance */), voffset[0], "CAN");
    }

    // Draw assets in the bottom line

    display.setFont(SYMBOL_FONT_PTR);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    if (direction == Direction::REVERSE) {
      display.drawString(0, voffset[3], "\2");
    } else {
      display.drawString(0, voffset[3], "\1");
    }

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    if (direction == Direction::FORWARD) {
      display.drawString(128, voffset[3], "\4");
    } else {
      display.drawString(128, voffset[3], "\3");
    }

    constexpr const uint8_t progressBarOffset = ((3*(SYMBOL_FONT_PTR[0]))/2);
    constexpr const uint8_t progressBarWidth = (128 - (2*progressBarOffset));
    display.drawProgressBar(progressBarOffset, voffset[3]+2, progressBarWidth, 6, speed);

    display.display();
  }
}

} /* namespace EngineControl */

#endif