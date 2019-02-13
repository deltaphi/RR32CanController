#include "EngineControl/EngineControl.h"
#include "config.h"

namespace EngineControl {

#if (DISPLAY_ATTACHED == STD_ON)
SSD1306Wire display(DISPLAY_TWI_ADDRESS, TWI_SDA_PIN, TWI_SCL_PIN);
#endif

#if (ENCODER_ENABLED == STD_ON)
RotaryEncoder encoder(ENCODER_A_PIN, ENCODER_B_PIN);
int encoderPosition = 0;
#endif

void begin() {

#if (DISPLAY_ATTACHED == STD_ON)
  Serial.println("Starting Display");
  // Initialize the Display
  display.init();
  display.flipScreenVertically();
#endif

#if (ENCODER_ENABLED == STD_ON)
  Serial.println("Starting Encoder");
  pinMode(ENCODER_BUTTON_PIN, INPUT);
#endif
}

void loop() {
#if (DISPLAY_ATTACHED == STD_ON)
  loopDisplay();
#endif

#if (ENCODER_ENABLED == STD_ON)
  loopEncoder();
#endif
}


#if (DISPLAY_ATTACHED == STD_ON)
void loopDisplay() {
  display.clear();

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Hello world");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Hello world");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "Hello world");

  display.display();
}
#endif

#if (ENCODER_ENABLED == STD_ON)

void loopEncoder() {
    uint8_t readButton = digitalRead(ENCODER_BUTTON_PIN);
    if (readButton == LOW) {
        Serial.println("Pressed Button");
    }

    encoder.tick();

    int newPosition = encoder.getPosition();
    if (encoderPosition != newPosition) {
        Serial.print("Encoder position: ");
        Serial.print(newPosition);
        Serial.println();
        encoderPosition = newPosition;
    }
}
#endif

} /* namespace EngineControl */
