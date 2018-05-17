
#include <ns3/log.h>
#include <ns3/epc-s1ap-header.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("EpcS1apHeader");

NS_OBJECT_ENSURE_REGISTERED (EpcS1apHeader);

EpcS1apHeader::EpcS1apHeader ()
  : m_procedureCode (0x00),
    m_typeOfMessage (0x00)
{
}

EpcS1apHeader::~EpcS1apHeader ()
{
}

TypeId
EpcS1apHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EpcS1apHeader")
    .SetParent<Header> ()
    .SetGroupName ("Lte")
    .AddConstructor<EpcS1apHeader> ()
  ;
  return tid;
}

TypeId
EpcS1apHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
EpcS1apHeader::GetSerializedSize (void) const
{
  return 2;
}

void
EpcS1apHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (m_procedureCode);
  i.WriteU8 (m_typeOfMessage);
}

uint32_t
EpcS1apHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_procedureCode = i.ReadU8 ();
  m_typeOfMessage = i.ReadU8 ();
  return GetSerializedSize ();
}

void
EpcS1apHeader::Print (std::ostream &os) const
{
  os << "ProcedureCode=" << m_procedureCode;
  os << "TypeOfMessage=" << m_typeOfMessage;
}

uint8_t
EpcS1apHeader::GetTypeOfMessage () const
{
  return m_typeOfMessage;
}

void
EpcS1apHeader::SetTypeOfMessage (uint8_t typeOfMessage)
{
  m_typeOfMessage = typeOfMessage;
}
  
uint8_t
EpcS1apHeader::GetProcedureCode () const
{
  return m_procedureCode;
}

void
EpcS1apHeader::SetProcedureCode (uint8_t procedureCode)
{
  m_procedureCode = procedureCode;
}

NS_OBJECT_ENSURE_REGISTERED (InitialUeRequestHeader);

InitialUeRequestHeader::InitialUeRequestHeader ()
  : m_numberOfIes (0x00),
    m_headerLength (0x14),
    m_mmeUeS1Id (0x00),
    m_enbUeS1Id (0x00),
    m_stmsi (0x00),
    m_ecgi (0x00)
{
}

InitialUeRequestHeader::~InitialUeRequestHeader ()
{
}

TypeId
InitialUeRequestHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::InitialUeRequestHeader")
    .SetParent<Header> ()
    .SetGroupName ("Lte")
    .AddConstructor<InitialUeRequestHeader> ()
  ;
  return tid;
}

TypeId
InitialUeRequestHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
InitialUeRequestHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
InitialUeRequestHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU64 (m_mmeUeS1Id);
  i.WriteHtonU16 (m_enbUeS1Id);
  i.WriteHtonU64 (m_stmsi);
  i.WriteHtonU16 (m_ecgi);
}

uint32_t
InitialUeRequestHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_headerLength = 0;
  
  m_mmeUeS1Id = i.ReadNtohU64 ();
  m_headerLength += sizeof m_mmeUeS1Id;
  ++m_numberOfIes;
  
  m_enbUeS1Id = i.ReadNtohU16 ();
  m_headerLength += sizeof m_enbUeS1Id;
  ++m_numberOfIes;
  
  m_stmsi = i.ReadNtohU64 ();
  m_headerLength += sizeof m_stmsi;
  ++m_numberOfIes;
  
  m_ecgi = i.ReadNtohU16 ();
  m_headerLength += sizeof m_ecgi;
  ++m_numberOfIes;
  
  return GetSerializedSize ();
}

void
InitialUeRequestHeader::Print (std::ostream &os) const
{
  os << "MmeUeS1Id=" << m_mmeUeS1Id;
  os << "EnbUeS1Id=" << m_enbUeS1Id;
  os << "Stmsi=" << m_stmsi;
  os << "Ecgi=" << m_ecgi;
}

uint64_t
InitialUeRequestHeader::GetMmeUeS1Id () const
{
  return m_mmeUeS1Id;
}

void
InitialUeRequestHeader::SetMmeUeS1Id (uint64_t mmeUeS1Id)
{
  m_mmeUeS1Id = mmeUeS1Id;
}
  
uint16_t
InitialUeRequestHeader::GetEnbUeS1Id () const
{
  return m_enbUeS1Id;
}

void
InitialUeRequestHeader::SetEnbUeS1Id (uint16_t enbUeS1Id)
{
  m_enbUeS1Id = enbUeS1Id;
}
  
uint64_t
InitialUeRequestHeader::GetStmsi () const
{
  return m_stmsi;
}

void
InitialUeRequestHeader::SetStmsi (uint64_t stmsi)
{
  m_stmsi = stmsi;
}
  
uint16_t
InitialUeRequestHeader::GetEcgi () const
{
  return m_ecgi;
}

void
InitialUeRequestHeader::SetEcgi (uint16_t ecgi)
{
  m_ecgi = ecgi;
}

NS_OBJECT_ENSURE_REGISTERED (ErabReleaseIndicationHeader);

ErabReleaseIndicationHeader::ErabReleaseIndicationHeader ()
  : m_numberOfIes (0x00),
    m_headerLength (0x1B),
    m_mmeUeS1Id (0x00),
    m_enbUeS1Id (0x00),
    m_erabToBeReleaseIndication ()
{
}

ErabReleaseIndicationHeader::~ErabReleaseIndicationHeader ()
{
}

TypeId
ErabReleaseIndicationHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ErabReleaseIndicationHeader")
    .SetParent<Header> ()
    .SetGroupName ("Lte")
    .AddConstructor<ErabReleaseIndicationHeader> ()
  ;
  return tid;
}

TypeId
ErabReleaseIndicationHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
ErabReleaseIndicationHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
ErabReleaseIndicationHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU64 (m_mmeUeS1Id);
  i.WriteHtonU16 (m_enbUeS1Id);
  
  i.WriteU8 (m_erabToBeReleaseIndication.size ());

  std::list<EpcS1apSapMme::ErabToBeReleasedIndication> indiation = m_erabToBeReleaseIndication;
  std::list<EpcS1apSapMme::ErabToBeReleasedIndication>::iterator it;
  for (it = indiation.begin ();
       it != indiation.end ();
       ++it)
    {
      i.WriteU8 (it->erabId);
    }
}

uint32_t
ErabReleaseIndicationHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_headerLength = 0;
  
  m_mmeUeS1Id = i.ReadNtohU64 ();
  m_headerLength += sizeof m_mmeUeS1Id;
  ++m_numberOfIes;
  
  m_enbUeS1Id = i.ReadNtohU16 ();
  m_headerLength += sizeof m_enbUeS1Id;
  ++m_numberOfIes;
  
  uint8_t size = i.ReadU8 ();
  for (uint8_t u = 0; u < size; ++u)
    {
      EpcS1apSapMme::ErabToBeReleasedIndication erab;
      erab.erabId = i.ReadU8 ();
      m_headerLength += sizeof(uint8_t);
      
      m_erabToBeReleaseIndication.push_back (erab);
      ++m_numberOfIes;
    }
  
  return GetSerializedSize ();
}

void
ErabReleaseIndicationHeader::Print (std::ostream &os) const
{
  os << "MmeUeS1Id=" << m_mmeUeS1Id;
  os << "EnbUeS1Id=" << m_enbUeS1Id;
}

uint64_t
ErabReleaseIndicationHeader::GetMmeUeS1Id () const
{
  return m_mmeUeS1Id;
}

void
ErabReleaseIndicationHeader::SetMmeUeS1Id (uint64_t mmeUeS1Id)
{
  m_mmeUeS1Id = mmeUeS1Id;
}
  
uint16_t
ErabReleaseIndicationHeader::GetEnbUeS1Id () const
{
  return m_enbUeS1Id;
}

void
ErabReleaseIndicationHeader::SetEnbUeS1Id (uint16_t enbUeS1Id)
{
  m_enbUeS1Id = enbUeS1Id;
}

std::list<EpcS1apSapMme::ErabToBeReleasedIndication>
ErabReleaseIndicationHeader::GetErabToBeReleaseIndication () const
{
  return m_erabToBeReleaseIndication;
}

void
ErabReleaseIndicationHeader::SetErabToBeReleaseIndication (std::list<EpcS1apSapMme::ErabToBeReleasedIndication> erabToBeReleaseIndication)
{
  m_erabToBeReleaseIndication = erabToBeReleaseIndication;
}

NS_OBJECT_ENSURE_REGISTERED (InitialContextSetupResponseHeader);

InitialContextSetupResponseHeader::InitialContextSetupResponseHeader ()
  : m_numberOfIes (0x00),
    m_headerLength (0x0A),
    m_mmeUeS1Id (0x00),
    m_enbUeS1Id (0x00)
{
}

InitialContextSetupResponseHeader::~InitialContextSetupResponseHeader ()
{
}

TypeId
InitialContextSetupResponseHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::InitialContextSetupResponseHeader")
    .SetParent<Header> ()
    .SetGroupName ("Lte")
    .AddConstructor<InitialContextSetupResponseHeader> ()
  ;
  return tid;
}

TypeId
InitialContextSetupResponseHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
InitialContextSetupResponseHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
InitialContextSetupResponseHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU64 (m_mmeUeS1Id);
  i.WriteHtonU16 (m_enbUeS1Id);
}

uint32_t
InitialContextSetupResponseHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_headerLength = 0;
  
  m_mmeUeS1Id = i.ReadNtohU64 ();
  m_headerLength += sizeof m_mmeUeS1Id;
  ++m_numberOfIes;
  
  m_enbUeS1Id = i.ReadNtohU16 ();
  m_headerLength += sizeof m_enbUeS1Id;
  ++m_numberOfIes;
  
  return GetSerializedSize ();
}

void
InitialContextSetupResponseHeader::Print (std::ostream &os) const
{
  os << "MmeUeS1Id=" << m_mmeUeS1Id;
  os << "EnbUeS1Id=" << m_enbUeS1Id;
}

uint64_t
InitialContextSetupResponseHeader::GetMmeUeS1Id () const
{
  return m_mmeUeS1Id;
}

void
InitialContextSetupResponseHeader::SetMmeUeS1Id (uint64_t mmeUeS1Id)
{
  m_mmeUeS1Id = mmeUeS1Id;
}
  
uint16_t
InitialContextSetupResponseHeader::GetEnbUeS1Id () const
{
  return m_enbUeS1Id;
}

void
InitialContextSetupResponseHeader::SetEnbUeS1Id (uint16_t enbUeS1Id)
{
  m_enbUeS1Id = enbUeS1Id;
}

NS_OBJECT_ENSURE_REGISTERED (PathSwitchRequestHeader);

PathSwitchRequestHeader::PathSwitchRequestHeader ()
  : m_numberOfIes (0x00),
    m_headerLength (0xB3),
    m_enbUeS1Id (0x00),
    m_mmeUeS1Id (0x00),
    m_cgi (0x00),
    m_erabToBeSwitchedInDownlinkList ()
{
}

PathSwitchRequestHeader::~PathSwitchRequestHeader ()
{
}

TypeId
PathSwitchRequestHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PathSwitchRequestHeader")
    .SetParent<Header> ()
    .SetGroupName ("Lte")
    .AddConstructor<PathSwitchRequestHeader> ()
  ;
  return tid;
}

TypeId
PathSwitchRequestHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
PathSwitchRequestHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
PathSwitchRequestHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU64 (m_mmeUeS1Id);
  i.WriteHtonU64 (m_enbUeS1Id);
  i.WriteHtonU16 (m_cgi);
  
  i.WriteU8 (m_erabToBeSwitchedInDownlinkList.size ());
  
  std::list<EpcS1apSapMme::ErabSwitchedInDownlinkItem> items = m_erabToBeSwitchedInDownlinkList;
  std::list<EpcS1apSapMme::ErabSwitchedInDownlinkItem>::iterator it;
  for (it = items.begin ();
       it != items.end ();
       ++it)
    {
      i.WriteHtonU16 (it->erabId);
      i.WriteHtonU32 (it->enbTransportLayerAddress.Get ());
      i.WriteHtonU32 (it->enbTeid);
    }
}

uint32_t
PathSwitchRequestHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_headerLength = 0;
  
  m_mmeUeS1Id = i.ReadNtohU64 ();
  m_headerLength += sizeof m_mmeUeS1Id;
  ++m_numberOfIes;
  
  m_enbUeS1Id = i.ReadNtohU64 ();
  m_headerLength += sizeof m_enbUeS1Id;
  ++m_numberOfIes;
  
  m_cgi = i.ReadNtohU16 ();
  m_headerLength += sizeof m_cgi;
  ++m_numberOfIes;
  
  uint8_t size = i.ReadU8 ();
  for (uint8_t j = 0; j < size; ++j) 
    {
      EpcS1apSapMme::ErabSwitchedInDownlinkItem erab;
      erab.erabId = i.ReadNtohU16 ();
      m_headerLength += sizeof(uint16_t);
      
      erab.enbTransportLayerAddress = Ipv4Address (i.ReadNtohU32 ());
      erab.enbTeid = i.ReadNtohU32 ();
      m_headerLength +=  sizeof(uint32_t) * 2;
      
      m_erabToBeSwitchedInDownlinkList.push_back (erab);
      ++m_numberOfIes;
    }
  
  return GetSerializedSize ();
}

void
PathSwitchRequestHeader::Print (std::ostream &os) const
{
  os << "EnbUeS1Id=" << m_enbUeS1Id;
  os << "MmeUeS1Id=" << m_mmeUeS1Id;
  os << "Cgi=" << m_cgi;
}

uint64_t
PathSwitchRequestHeader::GetMmeUeS1Id () const
{
  return m_mmeUeS1Id;
}

void
PathSwitchRequestHeader::SetMmeUeS1Id (uint64_t mmeUeS1Id)
{
  m_mmeUeS1Id = mmeUeS1Id;
}
  
uint64_t
PathSwitchRequestHeader::GetEnbUeS1Id () const
{
  return m_enbUeS1Id;
}

void
PathSwitchRequestHeader::SetEnbUeS1Id (uint64_t enbUeS1Id)
{
  m_enbUeS1Id = enbUeS1Id;
}

uint16_t
PathSwitchRequestHeader::GetCgi () const
{
  return m_cgi;
}

void
PathSwitchRequestHeader::SetCgi (uint16_t cgi)
{
  m_cgi = cgi;
}

std::list<EpcS1apSapMme::ErabSwitchedInDownlinkItem>
PathSwitchRequestHeader::GetErabToBeSwitchedInDownlinkList () const
{
  return m_erabToBeSwitchedInDownlinkList;
}

void
PathSwitchRequestHeader::SetErabToBeSwitchedInDownlinkList (std::list<EpcS1apSapMme::ErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList)
{
  m_erabToBeSwitchedInDownlinkList = erabToBeSwitchedInDownlinkList;
}

NS_OBJECT_ENSURE_REGISTERED (InitialContextSetupRequestHeader);

InitialContextSetupRequestHeader::InitialContextSetupRequestHeader ()
  : m_numberOfIes (0x00),
    m_headerLength (0x290),
    m_mmeUeS1Id (0x00),
    m_enbUeS1Id (0x00)
{
}

InitialContextSetupRequestHeader::~InitialContextSetupRequestHeader ()
{
}

TypeId
InitialContextSetupRequestHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::InitialContextSetupRequestHeader")
    .SetParent<Header> ()
    .SetGroupName ("Lte")
    .AddConstructor<InitialContextSetupRequestHeader> ()
  ;
  return tid;
}

TypeId
InitialContextSetupRequestHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
InitialContextSetupRequestHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
InitialContextSetupRequestHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU64 (m_mmeUeS1Id);
  i.WriteHtonU16 (m_enbUeS1Id);
  
  i.WriteU8 (m_erabToBeSetupList.size ());
  
  std::list<EpcS1apSapEnb::ErabToBeSetupItem> items = m_erabToBeSetupList;
  std::list<EpcS1apSapEnb::ErabToBeSetupItem>::iterator it;
  for (it = items.begin ();
       it != items.end ();
       ++it)
    {
      i.WriteU8 (it->erabId);
      
      i.WriteHtonU16 (it->erabLevelQosParameters.qci);
      i.WriteHtonU64 (it->erabLevelQosParameters.gbrQosInfo.gbrDl);
      i.WriteHtonU64 (it->erabLevelQosParameters.gbrQosInfo.gbrUl);
      i.WriteHtonU64 (it->erabLevelQosParameters.gbrQosInfo.mbrDl);
      i.WriteHtonU64 (it->erabLevelQosParameters.gbrQosInfo.mbrUl);
      i.WriteU8 (it->erabLevelQosParameters.arp.priorityLevel);
      i.WriteU8 (it->erabLevelQosParameters.arp.preemptionCapability);
      i.WriteU8 (it->erabLevelQosParameters.arp.preemptionVulnerability);
      
      i.WriteHtonU32 (it->transportLayerAddress.Get ());
      i.WriteHtonU32 (it->sgwTeid);
    }
}

uint32_t
InitialContextSetupRequestHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_headerLength = 0;
  
  m_mmeUeS1Id = i.ReadNtohU64 ();
  m_headerLength += sizeof m_mmeUeS1Id;
  ++m_numberOfIes;
  
  m_enbUeS1Id = i.ReadNtohU16 ();
  m_headerLength += sizeof m_enbUeS1Id;
  ++m_numberOfIes;
  
  uint8_t size = i.ReadU8 ();
  for (uint8_t u = 0; u < size; ++u)
    {
      EpcS1apSapEnb::ErabToBeSetupItem erab;
      erab.erabId = i.ReadU8 ();
      m_headerLength += sizeof(uint8_t);
      
      erab.erabLevelQosParameters = EpsBearer ((EpsBearer::Qci) i.ReadNtohU16 ());
      m_headerLength += sizeof(uint16_t);
  
      erab.erabLevelQosParameters.gbrQosInfo.gbrDl = i.ReadNtohU64 ();
      erab.erabLevelQosParameters.gbrQosInfo.gbrUl = i.ReadNtohU64 ();
      erab.erabLevelQosParameters.gbrQosInfo.mbrDl = i.ReadNtohU64 ();
      erab.erabLevelQosParameters.gbrQosInfo.mbrUl = i.ReadNtohU64 ();
      m_headerLength += sizeof(uint64_t) * 4;
  
      erab.erabLevelQosParameters.arp.priorityLevel = i.ReadU8 ();
      erab.erabLevelQosParameters.arp.preemptionCapability = i.ReadU8 ();
      erab.erabLevelQosParameters.arp.preemptionVulnerability = i.ReadU8 ();
      m_headerLength += sizeof(uint8_t) * 3;
      
      erab.transportLayerAddress = Ipv4Address (i.ReadNtohU32 ());
      erab.sgwTeid = i.ReadNtohU32 ();
      m_headerLength += sizeof(uint32_t) * 2;
  
      m_erabToBeSetupList.push_back (erab);
      ++m_numberOfIes;
    }
    
  return GetSerializedSize ();
}

void
InitialContextSetupRequestHeader::Print (std::ostream &os) const
{
  os << "MmeUeS1Id=" << m_mmeUeS1Id;
  os << "EnbUeS1Id=" << m_enbUeS1Id;
}

uint64_t
InitialContextSetupRequestHeader::GetMmeUeS1Id () const
{
  return m_mmeUeS1Id;
}

void
InitialContextSetupRequestHeader::SetMmeUeS1Id (uint64_t mmeUeS1Id)
{
  m_mmeUeS1Id = mmeUeS1Id;
}
  
uint16_t
InitialContextSetupRequestHeader::GetEnbUeS1Id () const
{
  return m_enbUeS1Id;
}

void
InitialContextSetupRequestHeader::SetEnbUeS1Id (uint16_t enbUeS1Id)
{
  m_enbUeS1Id = enbUeS1Id;
}

std::list<EpcS1apSapEnb::ErabToBeSetupItem>
InitialContextSetupRequestHeader::GetErabToBeSetupList () const
{
  return m_erabToBeSetupList;
}

void
InitialContextSetupRequestHeader::SetErabToBeSetupList (std::list<EpcS1apSapEnb::ErabToBeSetupItem> erabToBeSetupList)
{
  m_erabToBeSetupList = erabToBeSetupList;
}

NS_OBJECT_ENSURE_REGISTERED (PathSwitchRequestAcknowledgeHeader);

PathSwitchRequestAcknowledgeHeader::PathSwitchRequestAcknowledgeHeader ()
  : m_numberOfIes (0x00),
    m_headerLength (0xA3),
    m_enbUeS1Id (0x00),
    m_mmeUeS1Id (0x00),
    m_cgi (0x00),
    m_erabToBeSwitchedInUplinkList ()
{
}

PathSwitchRequestAcknowledgeHeader::~PathSwitchRequestAcknowledgeHeader ()
{
}

TypeId
PathSwitchRequestAcknowledgeHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PathSwitchRequestAcknowledgeHeader")
    .SetParent<Header> ()
    .SetGroupName ("Lte")
    .AddConstructor<PathSwitchRequestAcknowledgeHeader> ()
  ;
  return tid;
}

TypeId
PathSwitchRequestAcknowledgeHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
PathSwitchRequestAcknowledgeHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
PathSwitchRequestAcknowledgeHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU64 (m_mmeUeS1Id);
  i.WriteHtonU64 (m_enbUeS1Id);
  i.WriteHtonU16 (m_cgi);
  
  i.WriteU8 (m_erabToBeSwitchedInUplinkList.size ());
  
  std::list<EpcS1apSapEnb::ErabSwitchedInUplinkItem> items = m_erabToBeSwitchedInUplinkList;
  std::list<EpcS1apSapEnb::ErabSwitchedInUplinkItem>::iterator it;
  for (it = items.begin ();
       it != items.end ();
       ++it)
    {
      i.WriteU8 (it->erabId);
      i.WriteHtonU32 (it->transportLayerAddress.Get ());
      i.WriteHtonU32 (it->enbTeid);
    }
}

uint32_t
PathSwitchRequestAcknowledgeHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_headerLength = 0;
  
  m_mmeUeS1Id = i.ReadNtohU64 ();
  m_headerLength += sizeof m_mmeUeS1Id;
  ++m_numberOfIes;
  
  m_enbUeS1Id = i.ReadNtohU64 ();
  m_headerLength += sizeof m_enbUeS1Id;
  ++m_numberOfIes;
  
  m_cgi = i.ReadNtohU16 ();
  m_headerLength += sizeof m_cgi;
  ++m_numberOfIes;
  
  uint8_t size = i.ReadU8 ();
  for (uint8_t u = 0; u < size; ++u)
    {
      EpcS1apSapEnb::ErabSwitchedInUplinkItem erab;
      erab.erabId = i.ReadU8 ();
      m_headerLength += sizeof(uint8_t);
      
      erab.transportLayerAddress = Ipv4Address (i.ReadNtohU32 ());
      erab.enbTeid = i.ReadNtohU32 ();
      m_headerLength += sizeof(uint32_t) * 2;
      
      m_erabToBeSwitchedInUplinkList.push_back (erab);
      ++m_numberOfIes;
    }
  
  return GetSerializedSize ();
}

void
PathSwitchRequestAcknowledgeHeader::Print (std::ostream &os) const
{
  os << "MmeUeS1Id=" << m_mmeUeS1Id;
  os << "EnbUeS1Id=" << m_enbUeS1Id;
  os << "Cgi=" << m_cgi;
}

uint64_t
PathSwitchRequestAcknowledgeHeader::GetMmeUeS1Id () const
{
  return m_mmeUeS1Id;
}

void
PathSwitchRequestAcknowledgeHeader::SetMmeUeS1Id (uint64_t mmeUeS1Id)
{
  m_mmeUeS1Id = mmeUeS1Id;
}
  
uint64_t
PathSwitchRequestAcknowledgeHeader::GetEnbUeS1Id () const
{
  return m_enbUeS1Id;
}

void
PathSwitchRequestAcknowledgeHeader::SetEnbUeS1Id (uint64_t enbUeS1Id)
{
  m_enbUeS1Id = enbUeS1Id;
}

uint16_t
PathSwitchRequestAcknowledgeHeader::GetCgi () const
{
  return m_cgi;
}

void
PathSwitchRequestAcknowledgeHeader::SetCgi (uint16_t cgi)
{
  m_cgi = cgi;
}

std::list<EpcS1apSapEnb::ErabSwitchedInUplinkItem>
PathSwitchRequestAcknowledgeHeader::GetErabToBeSwitchedInUplinkList () const
{
  return m_erabToBeSwitchedInUplinkList;
}

void
PathSwitchRequestAcknowledgeHeader::SetErabToBeSwitchedInUplinkList (std::list<EpcS1apSapEnb::ErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList)
{
  m_erabToBeSwitchedInUplinkList = erabToBeSwitchedInUplinkList;
}

}
