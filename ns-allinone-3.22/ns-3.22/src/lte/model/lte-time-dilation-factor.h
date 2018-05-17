#ifndef LTE_TIME_DILATION_FACTOR_H
#define LTE_TIME_DILATION_FACTOR_H

#include <ns3/object.h>
#include <stdint.h>

namespace ns3 {

namespace LteTimeDilationFactor {

  void SetTimeDilationFactor (uint16_t tdf);
  uint16_t GetTimeDilationFactor (void);

}

}

#endif
