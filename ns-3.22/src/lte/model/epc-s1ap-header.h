
#ifndef EPC_S1AP_HEADER_H
#define EPC_S1AP_HEADER_H

#include <ns3/header.h>
#include <ns3/epc-s1ap-sap.h>

namespace ns3 {

class EpcS1apHeader : public Header
{
public:
  EpcS1apHeader ();
  virtual ~EpcS1apHeader ();
  
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
  uint8_t GetProcedureCode () const;
  void SetProcedureCode (uint8_t procedureCode);
  
  uint8_t GetTypeOfMessage () const;
  void SetTypeOfMessage (uint8_t typeOfMessage);

  enum ProcedureCode_t
  {
    InitialUeMessage = 1,
    ErabRelease = 2,
    InitialContextSetup = 3,
    PathSwitchRequest = 4
  };

  enum TypeOfMessgae_t
  {
    InitiatingMessage = 5,
    SuccessfulOutcome = 6,
    UnsuccessfulOutcome = 7
  };

private:
  uint8_t m_procedureCode;
  uint8_t m_typeOfMessage;
};

class InitialUeRequestHeader : public Header
{
public:
  InitialUeRequestHeader ();
  virtual ~InitialUeRequestHeader ();
  
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
  uint64_t GetMmeUeS1Id () const;
  void SetMmeUeS1Id (uint64_t mmeUeS1Id);
  
  uint16_t GetEnbUeS1Id () const;
  void SetEnbUeS1Id (uint16_t enbUeS1Id);
  
  uint64_t GetStmsi () const;
  void SetStmsi (uint64_t stmsi);
  
  uint16_t GetEcgi () const;
  void SetEcgi (uint16_t ecgi);

private:
  uint32_t m_numberOfIes;
  uint32_t m_headerLength;

  uint64_t m_mmeUeS1Id;
  uint16_t m_enbUeS1Id;
  uint64_t m_stmsi;
  uint16_t m_ecgi;
};

class ErabReleaseIndicationHeader : public Header
{
public:
  ErabReleaseIndicationHeader ();
  virtual ~ErabReleaseIndicationHeader ();
  
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
  uint64_t GetMmeUeS1Id () const;
  void SetMmeUeS1Id (uint64_t mmeUeS1Id);
  
  uint16_t GetEnbUeS1Id () const;
  void SetEnbUeS1Id (uint16_t enbUeS1Id);
  
  std::list<EpcS1apSapMme::ErabToBeReleasedIndication> GetErabToBeReleaseIndication () const;
  void SetErabToBeReleaseIndication (std::list<EpcS1apSapMme::ErabToBeReleasedIndication> erabToBeReleaseIndication);

private:
  uint32_t m_numberOfIes;
  uint32_t m_headerLength;

  uint64_t m_mmeUeS1Id;
  uint16_t m_enbUeS1Id;
  std::list<EpcS1apSapMme::ErabToBeReleasedIndication> m_erabToBeReleaseIndication;
};

class InitialContextSetupResponseHeader : public Header
{
public:
  InitialContextSetupResponseHeader ();
  virtual ~InitialContextSetupResponseHeader ();
  
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
  uint64_t GetMmeUeS1Id () const;
  void SetMmeUeS1Id (uint64_t mmeUeS1Id);
  
  uint16_t GetEnbUeS1Id () const;
  void SetEnbUeS1Id (uint16_t enbUeS1Id);

private:
  uint32_t m_numberOfIes;
  uint32_t m_headerLength;

  uint64_t m_mmeUeS1Id;
  uint16_t m_enbUeS1Id;
};

class PathSwitchRequestHeader : public Header
{
public:
  PathSwitchRequestHeader ();
  virtual ~PathSwitchRequestHeader ();
  
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
  uint64_t GetEnbUeS1Id () const;
  void SetEnbUeS1Id (uint64_t enbUeS1Id);
  
  uint64_t GetMmeUeS1Id () const;
  void SetMmeUeS1Id (uint64_t mmeUeS1Id);
  
  uint16_t GetCgi () const;
  void SetCgi (uint16_t cgi);
  
  std::list<EpcS1apSapMme::ErabSwitchedInDownlinkItem> GetErabToBeSwitchedInDownlinkList () const;
  void SetErabToBeSwitchedInDownlinkList (std::list<EpcS1apSapMme::ErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList);

private:
  uint32_t m_numberOfIes;
  uint32_t m_headerLength;
  
  uint64_t m_enbUeS1Id;
  uint64_t m_mmeUeS1Id;
  uint16_t m_cgi;
  std::list<EpcS1apSapMme::ErabSwitchedInDownlinkItem> m_erabToBeSwitchedInDownlinkList;
};

class InitialContextSetupRequestHeader : public Header
{
public:
  InitialContextSetupRequestHeader ();
  virtual ~InitialContextSetupRequestHeader ();
  
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
  uint64_t GetMmeUeS1Id () const;
  void SetMmeUeS1Id (uint64_t mmeUeS1Id);
  
  uint16_t GetEnbUeS1Id () const;
  void SetEnbUeS1Id (uint16_t enbUeS1Id);
  
  std::list<EpcS1apSapEnb::ErabToBeSetupItem> GetErabToBeSetupList () const;
  void SetErabToBeSetupList (std::list<EpcS1apSapEnb::ErabToBeSetupItem> erabToBeSetupList);
  
private:
  uint32_t m_numberOfIes;
  uint32_t m_headerLength;

  uint64_t m_mmeUeS1Id;
  uint16_t m_enbUeS1Id;
  std::list<EpcS1apSapEnb::ErabToBeSetupItem> m_erabToBeSetupList;
};

class PathSwitchRequestAcknowledgeHeader : public Header
{
public:
  PathSwitchRequestAcknowledgeHeader ();
  virtual ~PathSwitchRequestAcknowledgeHeader ();
  
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
  uint64_t GetEnbUeS1Id () const;
  void SetEnbUeS1Id (uint64_t enbUeS1Id);
  
  uint64_t GetMmeUeS1Id () const;
  void SetMmeUeS1Id (uint64_t mmeUeS1Id);
  
  uint16_t GetCgi () const;
  void SetCgi (uint16_t cgi);
  
  std::list<EpcS1apSapEnb::ErabSwitchedInUplinkItem> GetErabToBeSwitchedInUplinkList () const;
  void SetErabToBeSwitchedInUplinkList (std::list<EpcS1apSapEnb::ErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList);

private:
  uint32_t m_numberOfIes;
  uint32_t m_headerLength;

  uint64_t m_enbUeS1Id;
  uint64_t m_mmeUeS1Id;
  uint16_t m_cgi;
  std::list<EpcS1apSapEnb::ErabSwitchedInUplinkItem> m_erabToBeSwitchedInUplinkList;
};

} // namespace ns3

#endif // EPC_S1AP_HEADER_H
