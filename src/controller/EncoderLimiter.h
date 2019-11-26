#ifndef __CONTROLLER__ENCODERLIMITER_H__
#define __CONTROLLER__ENCODERLIMITER_H__

namespace controller {

/*
 * \brief Class EncoderLimiter
 */
class EncoderLimiter {
 public:
  using EncoderValue_t = long;

  EncoderValue_t limitedValue(EncoderValue_t value);

  void setMin(EncoderValue_t min) { minimum = min; };
  void setMax(EncoderValue_t max) { maximum = max; };

 private:
  EncoderValue_t minimum = 0;
  EncoderValue_t maximum = 0;
};

}  // namespace controller

#endif  // __CONTROLLER__ENCODERLIMITER_H__
