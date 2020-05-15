#ifndef __HAL__DISPLAYDRIVER_H__
#define __HAL__DISPLAYDRIVER_H__

#include <Arduino.h>
#include "config.h"

#include "application/model/DisplayModel.h"

#if (DISPLAY_ATTACHED == STD_ON)

#if (DISPLAY_CONTROLLER_TYPE == DISPLAY_CONTROLLER_SSD1306)
#include "SSD1306Wire.h"
#endif
#if (DISPLAY_CONTROLLER_TYPE == DISPLAY_CONTROLLER_SH1106)
#include "SH1106Wire.h"
#endif


namespace hal {

/*
 * \brief Class DisplayDriver
 */    
class DisplayDriver {
  public:
  void begin();

  void loop(application::model::DisplayModel& displayModel);

  private:
  #if (DISPLAY_CONTROLLER_TYPE == DISPLAY_CONTROLLER_SSD1306)
  using DisplayController_t = SSD1306Wire;
#endif
#if (DISPLAY_CONTROLLER_TYPE == DISPLAY_CONTROLLER_SH1106)
  using DisplayController_t = SH1106Wire;
#endif

  static const uint8_t voffset[];
  static const uint8_t baselineOffset[];

  DisplayController_t display = {DISPLAY_TWI_ADDRESS, TWI_SDA_PIN, TWI_SCL_PIN};
};

}  // namespace hal

#endif /* Display attached */

#endif  // __HAL__DISPLAYDRIVER_H__
