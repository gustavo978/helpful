#include <ns3/lte-time-dilation-factor.h>
#include <ns3/singleton.h>
#include <ns3/core-module.h>
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteTimeDilationFactor");

class LteTimeDilationFactorImpl
{
public:

  LteTimeDilationFactorImpl (void);
  void SetTimeDilationFactor (uint16_t tdf);
  uint16_t GetTimeDilationFactor (void);

private:

  uint16_t m_timeDilationFactor;
};

LteTimeDilationFactorImpl::LteTimeDilationFactorImpl (void)
{
  SetTimeDilationFactor (1);
}

void
LteTimeDilationFactorImpl::SetTimeDilationFactor (uint16_t tdf)
{
  m_timeDilationFactor = tdf;

  uint32_t srs = 40 * tdf;
  if (srs > 320)
    {
      srs = 320;
    }

  Config::SetDefault ("ns3::LteEnbRrc::SystemInformationPeriodicity",  TimeValue (MilliSeconds (80 * tdf)));
  Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (srs));
  Config::SetDefault ("ns3::LteEnbRrc::ConnectionRequestTimeoutDuration", TimeValue (MilliSeconds (15 * tdf)));
  Config::SetDefault ("ns3::LteEnbRrc::ConnectionSetupTimeoutDuration", TimeValue (MilliSeconds (150 * tdf)));
  Config::SetDefault ("ns3::LteEnbRrc::ConnectionRejectedTimeoutDuration", TimeValue (MilliSeconds (30 * tdf)));
  Config::SetDefault ("ns3::LteEnbRrc::HandoverJoiningTimeoutDuration", TimeValue (MilliSeconds (200 * tdf)));
  Config::SetDefault ("ns3::LteEnbRrc::HandoverLeavingTimeoutDuration", TimeValue (MilliSeconds (500 * tdf)));
  Config::SetDefault ("ns3::LteUeRrc::T300", TimeValue (MilliSeconds (100 * tdf)));
}

uint16_t
LteTimeDilationFactorImpl::GetTimeDilationFactor (void)
{
  return m_timeDilationFactor;
}

namespace LteTimeDilationFactor {

void
SetTimeDilationFactor (uint16_t tdf)
{
  Singleton<LteTimeDilationFactorImpl>::Get ()->SetTimeDilationFactor (tdf);
}

uint16_t
GetTimeDilationFactor (void)
{
  return Singleton<LteTimeDilationFactorImpl>::Get ()->GetTimeDilationFactor ();
}

}

}
