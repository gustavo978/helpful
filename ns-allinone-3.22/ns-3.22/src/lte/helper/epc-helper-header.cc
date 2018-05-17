
#include <ns3/log.h>
#include <ns3/epc-helper-header.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("EpcHelperHeader");

NS_OBJECT_ENSURE_REGISTERED (EpcHelperHeader);

EpcHelperHeader::EpcHelperHeader ()
  : m_procedureCode (0x00),
    m_typeOfMessage (0x00)
{
}

EpcHelperHeader::~EpcHelperHeader ()
{
}

TypeId
EpcHelperHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EpcHelperHeader")
    .SetParent<Header> ()
    .SetGroupName ("Lte")
    .AddConstructor<EpcHelperHeader> ()
  ;
  return tid;
}

TypeId
EpcHelperHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
EpcHelperHeader::GetSerializedSize (void) const
{
  return 2;
}

void
EpcHelperHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (m_procedureCode);
  i.WriteU8 (m_typeOfMessage);
}

uint32_t
EpcHelperHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_procedureCode = i.ReadU8 ();
  m_typeOfMessage = i.ReadU8 ();
  return GetSerializedSize ();
}

void
EpcHelperHeader::Print (std::ostream &os) const
{
  os << "ProcedureCode=" << m_procedureCode;
  os << "TypeOfMessage=" << m_typeOfMessage;
}

uint8_t
EpcHelperHeader::GetTypeOfMessage () const
{
  return m_typeOfMessage;
}

void
EpcHelperHeader::SetTypeOfMessage (uint8_t typeOfMessage)
{
  m_typeOfMessage = typeOfMessage;
}
  
uint8_t
EpcHelperHeader::GetProcedureCode () const
{
  return m_procedureCode;
}

void
EpcHelperHeader::SetProcedureCode (uint8_t procedureCode)
{
  m_procedureCode = procedureCode;
}

NS_OBJECT_ENSURE_REGISTERED (AddEnbRequestHeader);

AddEnbRequestHeader::AddEnbRequestHeader ()
  : m_numberOfIes (0x00),
    m_headerLength (0x0A),
    m_cellId (0x00),
    m_enbAddress (),
    m_sgwAddress ()
{
}

AddEnbRequestHeader::~AddEnbRequestHeader ()
{
}

TypeId
AddEnbRequestHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::AddEnbRequestHeader")
    .SetParent<Header> ()
    .SetGroupName ("Lte")
    .AddConstructor<AddEnbRequestHeader> ()
  ;
  return tid;
}

TypeId
AddEnbRequestHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
AddEnbRequestHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
AddEnbRequestHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU16 (m_cellId);
  i.WriteHtonU32 (m_enbAddress.Get ());
  i.WriteHtonU32 (m_sgwAddress.Get ());
}

uint32_t
AddEnbRequestHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_headerLength = 0;
  
  m_cellId = i.ReadNtohU16 ();
  m_headerLength += sizeof m_cellId;
  ++m_numberOfIes;
  
  m_enbAddress = Ipv4Address (i.ReadNtohU32 ());
  m_headerLength += sizeof(uint32_t);
  ++m_numberOfIes;
  
  m_sgwAddress = Ipv4Address (i.ReadNtohU32 ());
  m_headerLength += sizeof(uint32_t);
  ++m_numberOfIes;
  return GetSerializedSize ();
}

void
AddEnbRequestHeader::Print (std::ostream &os) const
{
  os << "CellId=" << m_cellId;
  os << "EnbAddress=" << m_enbAddress;
  os << "SgwAddress=" << m_sgwAddress;
}

uint16_t
AddEnbRequestHeader::GetCellId () const
{
  return m_cellId;
}

void
AddEnbRequestHeader::SetCellId (uint16_t cellId)
{
  m_cellId = cellId;
}
 
Ipv4Address
AddEnbRequestHeader::GetEnbAddress () const
{
  return m_enbAddress;
}

void
AddEnbRequestHeader::SetEnbAddress (Ipv4Address enbAddress)
{
  m_enbAddress = enbAddress;
}

Ipv4Address
AddEnbRequestHeader::GetSgwAddress () const
{
  return m_sgwAddress;
}

void
AddEnbRequestHeader::SetSgwAddress (Ipv4Address sgwAddress)
{
  m_sgwAddress = sgwAddress;
}

NS_OBJECT_ENSURE_REGISTERED (AddUeRequestHeader);

AddUeRequestHeader::AddUeRequestHeader ()
  : m_numberOfIes (0x00),
    m_headerLength (0x08),
    m_imsi (0x00)
{
}

AddUeRequestHeader::~AddUeRequestHeader ()
{
}

TypeId
AddUeRequestHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::AddUeRequestHeader")
    .SetParent<Header> ()
    .SetGroupName ("Lte")
    .AddConstructor<AddUeRequestHeader> ()
  ;
  return tid;
}

TypeId
AddUeRequestHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
AddUeRequestHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
AddUeRequestHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU64 (m_imsi);
}

uint32_t
AddUeRequestHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_headerLength = 0;
  
  m_imsi = i.ReadNtohU64 ();
  m_headerLength += sizeof m_imsi;
  ++m_numberOfIes;
  return GetSerializedSize ();
}

void
AddUeRequestHeader::Print (std::ostream &os) const
{
  os << "Imsi=" << m_imsi;
}

uint16_t
AddUeRequestHeader::GetImsi () const
{
  return m_imsi;
}

void
AddUeRequestHeader::SetImsi (uint64_t imsi)
{
  m_imsi = imsi;
}

NS_OBJECT_ENSURE_REGISTERED (ActivateEpsBearerRequestHeader);

ActivateEpsBearerRequestHeader::ActivateEpsBearerRequestHeader ()
  : m_numberOfIes (0x00),
    m_headerLength (0x4E),
    m_imsi (0x00),
    m_ueAddress (),
    m_tft (),
    m_bearer ()
{
}

ActivateEpsBearerRequestHeader::~ActivateEpsBearerRequestHeader ()
{
}

TypeId
ActivateEpsBearerRequestHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ActivateEpsBearerRequestHeader")
    .SetParent<Header> ()
    .SetGroupName ("Lte")
    .AddConstructor<ActivateEpsBearerRequestHeader> ()
  ;
  return tid;
}

TypeId
ActivateEpsBearerRequestHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
ActivateEpsBearerRequestHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
ActivateEpsBearerRequestHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU64 (m_imsi);
  i.WriteHtonU32 (m_ueAddress.Get ());
  
  i.WriteU8 (m_tftNumFilters);
  std::list<EpcTft::PacketFilter> filters = m_tftFilters;
  std::list<EpcTft::PacketFilter>::iterator it;
  for (it = filters.begin (); it != filters.end (); ++it)
    {
      i.WriteU8 (it->precedence);
      i.WriteU8 (it->direction);
      
      i.WriteHtonU32 (it->remoteAddress.Get ());
      i.WriteHtonU32 (it->remoteMask.Get ());
      i.WriteHtonU32 (it->localAddress.Get ());
      i.WriteHtonU32 (it->localMask.Get ());
      
      i.WriteHtonU16 (it->remotePortStart);
      i.WriteHtonU16 (it->remotePortEnd);
      i.WriteHtonU16 (it->localPortStart);
      i.WriteHtonU16 (it->localPortEnd);
      
      i.WriteU8 (it->typeOfService);
      i.WriteU8 (it->typeOfServiceMask);
    }
  
  i.WriteHtonU16 (m_bearer.qci);
  i.WriteHtonU64 (m_bearer.gbrQosInfo.gbrDl);
  i.WriteHtonU64 (m_bearer.gbrQosInfo.gbrUl);
  i.WriteHtonU64 (m_bearer.gbrQosInfo.mbrDl);
  i.WriteHtonU64 (m_bearer.gbrQosInfo.mbrUl);
  i.WriteU8 (m_bearer.arp.priorityLevel);
  i.WriteU8 (m_bearer.arp.preemptionCapability);
  i.WriteU8 (m_bearer.arp.preemptionVulnerability);
}

uint32_t
ActivateEpsBearerRequestHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_headerLength = 0;
  
  m_imsi = i.ReadNtohU64 ();
  m_headerLength += sizeof m_imsi;
  ++m_numberOfIes;
  
  m_ueAddress = Ipv4Address (i.ReadNtohU32 ());
  m_headerLength += sizeof(uint32_t);
  ++m_numberOfIes;
  
  m_tftNumFilters = i.ReadU8 ();
  m_tft.SetNumFilters (m_tftNumFilters);
  m_headerLength += sizeof(uint8_t);
  ++m_numberOfIes;
  
  std::list<EpcTft::PacketFilter> filters;
  for (uint8_t j = 0; j < m_tftNumFilters; ++j)
    {
      EpcTft::PacketFilter filter;
      filter.precedence = i.ReadU8 ();
      switch (i.ReadU8 ())
        {
          case EpcTft::DOWNLINK:
             filter.direction = EpcTft::DOWNLINK;
          case EpcTft::UPLINK:
             filter.direction = EpcTft::UPLINK;
          case EpcTft::BIDIRECTIONAL:
             filter.direction = EpcTft::BIDIRECTIONAL;
        }
      m_headerLength += sizeof(uint8_t) * 2;
      
      filter.remoteAddress = Ipv4Address (i.ReadNtohU32 ());
      filter.remoteMask = Ipv4Mask (i.ReadNtohU32 ());
      filter.localAddress = Ipv4Address (i.ReadNtohU32 ());
      filter.localMask = Ipv4Mask (i.ReadNtohU32 ());
      m_headerLength += sizeof(uint32_t) * 4;
      
      filter.remotePortStart = i.ReadNtohU16 ();
      filter.remotePortEnd = i.ReadNtohU16 ();
      filter.localPortStart = i.ReadNtohU16 ();
      filter.localPortEnd = i.ReadNtohU16 ();
      m_headerLength += sizeof(uint16_t) * 4;
      
      filter.typeOfService = i.ReadU8 ();
      filter.typeOfServiceMask = i.ReadU8 ();
      m_headerLength += sizeof(uint8_t) * 2;
      
      filters.push_back (filter);
      ++m_numberOfIes;
    }
  m_tft.SetFilters (filters);
    
  m_bearer = EpsBearer ((EpsBearer::Qci) i.ReadNtohU16 ());
  m_headerLength += sizeof(uint16_t);
  
  m_bearer.gbrQosInfo.gbrDl = i.ReadNtohU64 ();
  m_bearer.gbrQosInfo.gbrUl = i.ReadNtohU64 ();
  m_bearer.gbrQosInfo.mbrDl = i.ReadNtohU64 ();
  m_bearer.gbrQosInfo.mbrUl = i.ReadNtohU64 ();
  m_headerLength += sizeof(uint64_t) * 4;
  
  m_bearer.arp.priorityLevel = i.ReadU8 ();
  m_bearer.arp.preemptionCapability = i.ReadU8 ();
  m_bearer.arp.preemptionVulnerability = i.ReadU8 ();
  m_headerLength += sizeof(uint8_t) * 3;
  
  return GetSerializedSize ();
}

void
ActivateEpsBearerRequestHeader::Print (std::ostream &os) const
{
  os << "Imsi=" << m_imsi;
  os << "UeAddress=" << m_ueAddress;
}

uint64_t
ActivateEpsBearerRequestHeader::GetImsi () const
{
  return m_imsi;
}

void
ActivateEpsBearerRequestHeader::SetImsi (uint64_t imsi)
{
  m_imsi = imsi;
}

Ipv4Address
ActivateEpsBearerRequestHeader::GetUeAddress () const
{
  return m_ueAddress;
}

void
ActivateEpsBearerRequestHeader::SetUeAddress (Ipv4Address ueAddress)
{
  m_ueAddress = ueAddress;
}
  
EpcTft
ActivateEpsBearerRequestHeader::GetEpcTft () const
{
  return m_tft;
}

void
ActivateEpsBearerRequestHeader::SetEpcTft (EpcTft tft)
{
  m_tft = tft;
  m_tftFilters = m_tft.GetFilters ();
  m_tftNumFilters = m_tft.GetNumFilters ();
}
  
EpsBearer
ActivateEpsBearerRequestHeader::GetEpsBearer () const
{
 return m_bearer;
}

void
ActivateEpsBearerRequestHeader::SetEpsBearer (EpsBearer bearer)
{
  m_bearer = bearer;
}

NS_OBJECT_ENSURE_REGISTERED (ActivateEpsBearerResponseHeader);

ActivateEpsBearerResponseHeader::ActivateEpsBearerResponseHeader ()
  : m_numberOfIes (0x00),
    m_headerLength (0x01),
    m_bearerId (0x00)
{
}

ActivateEpsBearerResponseHeader::~ActivateEpsBearerResponseHeader ()
{
}

TypeId
ActivateEpsBearerResponseHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ActivateEpsBearerResponseHeader")
    .SetParent<Header> ()
    .SetGroupName ("Lte")
    .AddConstructor<ActivateEpsBearerResponseHeader> ()
  ;
  return tid;
}

TypeId
ActivateEpsBearerResponseHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
ActivateEpsBearerResponseHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
ActivateEpsBearerResponseHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (m_bearerId);
}

uint32_t
ActivateEpsBearerResponseHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_headerLength = 0;
  
  m_bearerId = i.ReadU8 ();
  m_headerLength += sizeof m_bearerId;
  ++m_numberOfIes;
  return GetSerializedSize ();
}

void
ActivateEpsBearerResponseHeader::Print (std::ostream &os) const
{
  os << "BearerId=" << m_bearerId;
}

uint8_t
ActivateEpsBearerResponseHeader::GetBearerId () const
{
  return m_bearerId;
}

void
ActivateEpsBearerResponseHeader::SetBearerId (uint8_t bearerId)
{
  m_bearerId = bearerId;
}

}
