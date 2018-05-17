#include "ns3/teid-dscp-mapping.h"
#include "ns3/singleton.h"
#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/eps-bearer.h"
#include "ns3/ipv4-header.h"
#include <map>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TeidDscpMapping");

class TeidDscpMappingImpl
{
public:

  TeidDscpMappingImpl (void);
  void SetTeidDscpMapping (uint32_t teid, Ipv4Header::DscpType dscp);
  void SetTeidDscpMapping (uint32_t teid, EpsBearer::Qci qci);
  Ipv4Header::DscpType GetTeidDscpMapping (uint32_t teid);

private:

  std::map<uint32_t, Ipv4Header::DscpType> m_teidDscpMap;
};

TeidDscpMappingImpl::TeidDscpMappingImpl (void)
{
}

void
TeidDscpMappingImpl::SetTeidDscpMapping (uint32_t teid, Ipv4Header::DscpType dscp)
{
  m_teidDscpMap[teid] = dscp;
}

void
TeidDscpMappingImpl::SetTeidDscpMapping (uint32_t teid, EpsBearer::Qci qci)
{
  Ipv4Header::DscpType dscp;
  switch (qci)
    {
      case EpsBearer::GBR_CONV_VOICE:
      case EpsBearer::GBR_CONV_VIDEO:
      case EpsBearer::GBR_GAMING:
           dscp = Ipv4Header::DSCP_EF;
           break;
      case EpsBearer::GBR_NON_CONV_VIDEO:
           dscp = Ipv4Header::DSCP_AF41;
           break;
      case EpsBearer::NGBR_IMS:
      case EpsBearer::NGBR_VIDEO_TCP_OPERATOR:
           dscp = Ipv4Header::DSCP_AF31;
           break;
      case EpsBearer::NGBR_VOICE_VIDEO_GAMING:
           dscp = Ipv4Header::DSCP_AF21;
           break;
      case EpsBearer::NGBR_VIDEO_TCP_PREMIUM:
           dscp = Ipv4Header::DSCP_AF11;
           break;
      case EpsBearer::NGBR_VIDEO_TCP_DEFAULT:
      default:
           dscp = Ipv4Header::DscpDefault;
           break;
    }
  m_teidDscpMap[teid] = dscp;
  NS_LOG_LOGIC ("TeidDscpMappingImpl::SetTeidDscpMapping TEID: " << teid << " DSCP: " << dscp);
}

Ipv4Header::DscpType
TeidDscpMappingImpl::GetTeidDscpMapping (uint32_t teid)
{
  return m_teidDscpMap[teid];
}

namespace TeidDscpMapping {

void
SetTeidDscpMapping (uint32_t teid, Ipv4Header::DscpType dscp)
{
  Singleton<TeidDscpMappingImpl>::Get ()->SetTeidDscpMapping (teid, dscp);
}

void
SetTeidDscpMapping (uint32_t teid, EpsBearer::Qci qci)
{
  Singleton<TeidDscpMappingImpl>::Get ()->SetTeidDscpMapping (teid, qci);
}

Ipv4Header::DscpType
GetTeidDscpMapping (uint32_t teid)
{
  return Singleton<TeidDscpMappingImpl>::Get ()->GetTeidDscpMapping (teid);
}

}

}
