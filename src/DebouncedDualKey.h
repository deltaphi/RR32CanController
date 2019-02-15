#include <Arduino.h>

template <uint8_t lowCycles, uint8_t highCycles>
class DebouncedKey {
 public:
  using KeyStateType = uint8_t;
  using CyclesType = uint8_t;

 private:
  KeyStateType
      debouncedKeyState;  ///< Assumed state of the key after debouncing
  // CyclesType lowCycles; ///< cycles a key needs to remain stable low to be
  // considered debounced-low CyclesType highCycles;  ///< cycles a key needs to
  // remain stable how to be considered debounced-low
  KeyStateType lastStateRead;  ///< The last physical state before debouncing
                               ///< that was read
  CyclesType cyclesSinceLastTransition;  ///< The number of cycles since the
                                         ///< last transition

  bool edge;  ///< Whether there was an edge since the last getEdge();

 public:
  /**
   * \brief input a new physical value into the debouncing algorithm
   */
  void cycle(KeyStateType value) {
    // Update debounce algorithm
    if (value == this->lastStateRead) {
      // Input is continuous
      if (value != this->debouncedKeyState) {
        // value is not yet accepted
        --(this->cyclesSinceLastTransition);
        // Serial.print(F("Decreasing counter to "));
        // Serial.println(this->cyclesSinceLastTransition);
      }
    } else {
      // input value has changed
      this->lastStateRead = value;
      this->cyclesSinceLastTransition = (value == LOW ? lowCycles : highCycles);
      // Serial.print(F("Set counter to "));
      // Serial.println(this->cyclesSinceLastTransition);
    }

    // check for value acceptance
    if ((value != this->debouncedKeyState) &&
        (this->cyclesSinceLastTransition == 0)) {
      this->debouncedKeyState = value;
      edge = true;
      // Serial.println(F(" Accept value "));
    }
  }

  /**
   * \brief Force the debounced value.
   *
   * This lets the debouncing algorithm assume that teh forces value is the new
   * stable value, e.g., when known from environmental conditions.
   *
   * If the current physical value does not match the forced value, debouncing
   * will restart with the new physical value.
   */
  void forceDebounce(KeyStateType forcedValue) {
    edge = this->debouncedKeyState != forcedValue;
    this->debouncedKeyState = forcedValue;
    this->lastStateRead = forcedValue;
    this->cyclesSinceLastTransition = 0;
  }

  KeyStateType getDebouncedValue() const { return this->debouncedKeyState; }

  bool getAndResetEdgeFlag() {
    bool tempEdge = edge;
    edge = false;
    return tempEdge;
  }
};