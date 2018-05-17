#ifndef TEID_DSCP_MAPPING_H
#define TEID_DSCP_MAPPING_H

#include <stdint.h>
#include "ns3/ipv4-header.h"
#include "ns3/eps-bearer.h"

namespace ns3 {

namespace TeidDscpMapping {

  void SetTeidDscpMapping (uint32_t teid, Ipv4Header::DscpType dscp);
  void SetTeidDscpMapping (uint32_t teid, EpsBearer::Qci qci);
  Ipv4Header::DscpType GetTeidDscpMapping (uint32_t teid);

}

}

#endif
