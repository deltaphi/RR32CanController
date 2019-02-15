#ifndef __MAERKLIN_CAN_H__
#define __MAERKLIN_CAN_H__

#include <Arduino.h>

#include "MaerklinCan/Identifier.h"
#include "MaerklinCan/Constants.h"
#include "MaerklinCan/Data.h"

void HandleMaerklinCommand(const MaerklinCan::Identifier& id,
                           const MaerklinCan::Data& data);
void processAccessory(const MaerklinCan::Data& data);


class MaerklinSystemClass {
 public:
  bool systemOn;
};

extern MaerklinSystemClass MaerklinSystem;

#endif /* __MAERKLIN_CAN_H__ */
