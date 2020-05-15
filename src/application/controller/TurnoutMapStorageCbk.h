#ifndef __APPLICATION__CONTROLLER__TURNOUTMAPSTORAGECBK_H__
#define __APPLICATION__CONTROLLER__TURNOUTMAPSTORAGECBK_H__

#include "application/model/TurnoutMap.h"

namespace application {
namespace controller {

/*
 * \brief Class TurnoutMapStorageCbk
 */
class TurnoutMapStorageCbk {
 public:
  virtual bool load(application::model::TurnoutMap& turnoutMap) = 0;
  virtual void store(const application::model::TurnoutMap& turnoutMap) = 0;
};

}  // namespace controller
}  // namespace application

#endif  // __APPLICATION__CONTROLLER__TURNOUTMAPSTORAGECBK_H__
