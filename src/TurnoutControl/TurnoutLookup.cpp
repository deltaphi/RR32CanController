#include "config.h"
#include "model/TurnoutTypes.h"

namespace TurnoutControl {

// Action lists use human numbering
model::ActionList actionLists[model::NumActionLists] = {
    {{20, RR32Can::TurnoutDirection::RED},
     {4, RR32Can::TurnoutDirection::GREEN},
     {5, RR32Can::TurnoutDirection::GREEN}},  // 1 red
    {{20, RR32Can::TurnoutDirection::RED},
     {4, RR32Can::TurnoutDirection::GREEN},
     {5, RR32Can::TurnoutDirection::RED}},  // 2 red
    {{20, RR32Can::TurnoutDirection::RED},
     {4, RR32Can::TurnoutDirection::RED},
     {250, RR32Can::TurnoutDirection::GREEN}},  // 3 red. Last switch is a
                                                // "don't care"
    {{20, RR32Can::TurnoutDirection::GREEN},
     {7, RR32Can::TurnoutDirection::GREEN},
     {250, RR32Can::TurnoutDirection::RED}},  // 1 green. Last switch is a
                                              // "don't care"
    {{20, RR32Can::TurnoutDirection::GREEN},
     {7, RR32Can::TurnoutDirection::RED},
     {22, RR32Can::TurnoutDirection::RED}},  // 2 green
    {{20, RR32Can::TurnoutDirection::GREEN},
     {7, RR32Can::TurnoutDirection::RED},
     {22, RR32Can::TurnoutDirection::GREEN}},  // 3 green
};

} /* namespace TurnoutControl */
