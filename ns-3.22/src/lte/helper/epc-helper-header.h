
#ifndef EPC_HELPER_HEADER_H
#define EPC_HELPER_HEADER_H

#include <ns3/header.h>
#include <ns3/epc-tft.h>
#include <ns3/eps-bearer.h>

namespace ns3 {

class EpcHelperHeader : public Header
{
public:
  EpcHelperHeader ();
  virtual ~EpcHelperHeader ();
  
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
    AddUe = 1,
    AddEnb = 2,
    ActivateEpsBearer = 3,
  };

  enum TypeOfMessgae_t
  {
    InitiatingMessage = 4,
    SuccessfulOutcome = 5,
    UnsuccessfulOutcome = 6
  };

private:
  uint8_t m_procedureCode;
  uint8_t m_typeOfMessage;
};


class AddEnbRequestHeader : public Header
{
public:
  AddEnbRequestHeader ();
  virtual ~AddEnbRequestHeader ();
  
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
  uint16_t GetCellId () const;
  void SetCellId (uint16_t cellId);
  
  Ipv4Address GetEnbAddress () const;
  void SetEnbAddress (Ipv4Address enbAddress);

  Ipv4Address GetSgwAddress () const;
  void SetSgwAddress (Ipv4Address sgwAddress);

private:
  uint32_t m_numberOfIes;
  uint32_t m_headerLength;

  uint16_t m_cellId;
  Ipv4Address m_enbAddress;
  Ipv4Address m_sgwAddress;
};

class AddUeRequestHeader : public Header
{
public:
  AddUeRequestHeader ();
  virtual ~AddUeRequestHeader ();
  
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
  uint16_t GetImsi () const;
  void SetImsi (uint64_t imsi);
  
private:
  uint32_t m_numberOfIes;
  uint32_t m_headerLength;

  uint64_t m_imsi;
};

class ActivateEpsBearerRequestHeader : public Header
{
public:
  ActivateEpsBearerRequestHeader ();
  virtual ~ActivateEpsBearerRequestHeader ();
  
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
  uint64_t GetImsi () const;
  void SetImsi (uint64_t imsi);
  
  Ipv4Address GetUeAddress () const;
  void SetUeAddress (Ipv4Address ueAddress);
  
  EpcTft GetEpcTft () const;
  void SetEpcTft (EpcTft tft);
  
  EpsBearer GetEpsBearer () const;
  void SetEpsBearer (EpsBearer bearer);

private:
  uint32_t m_numberOfIes;
  uint32_t m_headerLength;

  uint64_t m_imsi;
  Ipv4Address m_ueAddress;
  
  EpcTft m_tft;
  std::list<EpcTft::PacketFilter> m_tftFilters;
  uint8_t m_tftNumFilters;

  EpsBearer m_bearer;
};

class ActivateEpsBearerResponseHeader : public Header
{
public:
  ActivateEpsBearerResponseHeader ();
  virtual ~ActivateEpsBearerResponseHeader ();
  
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
  uint8_t GetBearerId () const;
  void SetBearerId (uint8_t bearerId);

private:
  uint32_t m_numberOfIes;
  uint32_t m_headerLength;

  uint8_t m_bearerId;
};

} // namespace ns3

#endif // EPC_HELPER_HEADER_H
