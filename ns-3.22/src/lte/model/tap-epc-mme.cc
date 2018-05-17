/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Nicola Baldo <nbaldo@cttc.es>
 */

#include <ns3/tap-epc-mme.h>

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/packet.h>
#include <ns3/core-module.h>

#include <ns3/epc-s1ap-sap.h>
#include <ns3/epc-s1ap-header.h>
#include <ns3/epc-s11-sap.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TapEpcMme");

NS_OBJECT_ENSURE_REGISTERED (TapEpcMme);

TapEpcMme::TapEpcMme ()
  : m_s11SapSgw (0)
{
  NS_LOG_FUNCTION (this);
  m_s11SapMme = new MemberEpcS11SapMme<TapEpcMme> (this);
}

TapEpcMme::~TapEpcMme ()
{
  NS_LOG_FUNCTION (this);
}

void
TapEpcMme::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_s11SapMme;
}

TypeId
TapEpcMme::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TapEpcMme")
    .SetParent<Object> ()
    .AddConstructor<TapEpcMme> ()
    ;
  return tid;
}

void 
TapEpcMme::SetS11SapSgw (EpcS11SapSgw * s)
{
  m_s11SapSgw = s;
}

EpcS11SapMme* 
TapEpcMme::GetS11SapMme ()
{
  return m_s11SapMme;
}

void
TapEpcMme::AddS1apSocket (Address addr, Ptr<Socket> socket)
{
  InetSocketAddress inetAddr = InetSocketAddress::ConvertFrom (addr);
  Ipv4Address enbS1apAddr = inetAddr.GetIpv4 ();
  NS_LOG_FUNCTION (this << addr << socket << enbS1apAddr);
  m_s1apSocketMap[enbS1apAddr.Get ()] = socket;
}

void
TapEpcMme::SendToS1apSocket (Ptr<Socket> socket, Ptr<Packet> packet)
{
  socket->Send (packet);
}

void 
TapEpcMme::AddEnb (uint16_t cgi, Ipv4Address enbS1uAddr, Ipv4Address enbS1apAddr)
{
  NS_LOG_FUNCTION (this << cgi << enbS1uAddr);
  Ptr<EnbInfo> enbInfo = Create<EnbInfo> ();
  enbInfo->cgi = cgi;
  enbInfo->s1uAddr = enbS1uAddr;
  enbInfo->s1apAddr = enbS1apAddr;
  m_enbInfoMap[cgi] = enbInfo;
}

void 
TapEpcMme::AddUe (uint64_t imsi)
{
  NS_LOG_FUNCTION (this << imsi);
  Ptr<UeInfo> ueInfo = Create<UeInfo> ();
  ueInfo->imsi = imsi;
  ueInfo->mmeUeS1Id = imsi;
  m_ueInfoMap[imsi] = ueInfo;
  ueInfo->bearerCounter = 0;
}

uint8_t
TapEpcMme::AddBearer (uint64_t imsi, Ptr<EpcTft> tft, EpsBearer bearer)
{
  NS_LOG_FUNCTION (this << imsi);
  std::map<uint64_t, Ptr<UeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);
  NS_ASSERT_MSG (it->second->bearerCounter < 11, "too many bearers already! " << it->second->bearerCounter);
  BearerInfo bearerInfo;
  bearerInfo.bearerId = ++(it->second->bearerCounter);
  bearerInfo.tft = tft;
  bearerInfo.bearer = bearer;  
  it->second->bearersToBeActivated.push_back (bearerInfo);
  return bearerInfo.bearerId;
}

// S1-AP SAP MME forwarded methods
void 
TapEpcMme::DoInitialUeMessage (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, uint64_t imsi, uint16_t cgi)
{
  // mmeUeS1Id = imsi, enbUeS1Id = rnti, imsi = imsi, cgi = m_cellId
  NS_LOG_FUNCTION (this << mmeUeS1Id << enbUeS1Id << imsi << cgi);
  std::map<uint64_t, Ptr<UeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);
  it->second->cellId = cgi;
  EpcS11SapSgw::CreateSessionRequestMessage msg;
  msg.imsi = imsi;
  msg.uli.cgi = cgi;
  for (std::list<BearerInfo>::iterator bit = it->second->bearersToBeActivated.begin ();
       bit != it->second->bearersToBeActivated.end ();
       ++bit)
    {
      EpcS11SapSgw::BearerContextToBeCreated bearerContext;
      bearerContext.epsBearerId = bit->bearerId;
      bearerContext.bearerLevelQos = bit->bearer; 
      bearerContext.tft = bit->tft;
      msg.bearerContextsToBeCreated.push_back (bearerContext);
    }
  m_s11SapSgw->CreateSessionRequest (msg);
}

void 
TapEpcMme::DoInitialContextSetupResponse (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<EpcS1apSapMme::ErabSetupItem> erabSetupList)
{
  NS_LOG_FUNCTION (this << mmeUeS1Id << enbUeS1Id);
  NS_FATAL_ERROR ("unimplemented");
}

void
TapEpcMme::DoPathSwitchRequest (uint64_t enbUeS1Id, uint64_t mmeUeS1Id, uint16_t cgi, std::list<EpcS1apSapMme::ErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList)
{
  NS_LOG_FUNCTION (this << mmeUeS1Id << enbUeS1Id << cgi);

  uint64_t imsi = mmeUeS1Id;
  std::map<uint64_t, Ptr<UeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);
  NS_LOG_INFO ("IMSI " << imsi << " old eNB: " << it->second->cellId << ", new eNB: " << cgi);
  it->second->cellId = cgi;
  it->second->enbUeS1Id = enbUeS1Id;

  EpcS11SapSgw::ModifyBearerRequestMessage msg;
  msg.teid = imsi; // trick to avoid the need for allocating TEIDs on the S11 interface
  msg.uli.cgi = cgi;
  // bearer modification is not supported for now
  m_s11SapSgw->ModifyBearerRequest (msg);
}

// S11 SAP MME forwarded methods
void
TapEpcMme::DoCreateSessionResponse (EpcS11SapMme::CreateSessionResponseMessage msg)
{
  NS_LOG_FUNCTION (this << msg.teid);
  uint64_t imsi = msg.teid;
  std::list<EpcS1apSapEnb::ErabToBeSetupItem> erabToBeSetupList;
  for (std::list<EpcS11SapMme::BearerContextCreated>::iterator bit = msg.bearerContextsCreated.begin ();
       bit != msg.bearerContextsCreated.end ();
       ++bit)
    {
      EpcS1apSapEnb::ErabToBeSetupItem erab;
      erab.erabId = bit->epsBearerId;
      erab.erabLevelQosParameters = bit->bearerLevelQos;
      erab.transportLayerAddress = bit->sgwFteid.address;
      erab.sgwTeid = bit->sgwFteid.teid;      
      erabToBeSetupList.push_back (erab);
    }
  std::map<uint64_t, Ptr<UeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);
  uint16_t cellId = it->second->cellId;
  uint16_t enbUeS1Id = it->second->enbUeS1Id;
  uint64_t mmeUeS1Id = it->second->mmeUeS1Id;
  std::map<uint16_t, Ptr<EnbInfo> >::iterator jt = m_enbInfoMap.find (cellId);
  NS_ASSERT_MSG (jt != m_enbInfoMap.end (), "could not find any eNB with CellId " << cellId);

  Ipv4Address enbS1apAddr = jt->second->s1apAddr;
  Ptr<Socket> s1apSocket = m_s1apSocketMap[enbS1apAddr.Get ()];
  
  EpcS1apHeader epcS1apHeader;
  epcS1apHeader.SetProcedureCode (EpcS1apHeader::InitialContextSetup);
  epcS1apHeader.SetTypeOfMessage (EpcS1apHeader::InitiatingMessage);
  
  InitialContextSetupRequestHeader initialContextSetupRequestHeader;
  initialContextSetupRequestHeader.SetMmeUeS1Id (mmeUeS1Id);
  initialContextSetupRequestHeader.SetEnbUeS1Id (enbUeS1Id);
  initialContextSetupRequestHeader.SetErabToBeSetupList (erabToBeSetupList);

  Ptr<Packet> packet = Create<Packet> ();
  packet->AddHeader (initialContextSetupRequestHeader);
  packet->AddHeader (epcS1apHeader);
  SendToS1apSocket (s1apSocket, packet);
}

void
TapEpcMme::DoModifyBearerResponse (EpcS11SapMme::ModifyBearerResponseMessage msg)
{
  NS_LOG_FUNCTION (this << msg.teid);
  NS_ASSERT (msg.cause == EpcS11SapMme::ModifyBearerResponseMessage::REQUEST_ACCEPTED);
  uint64_t imsi = msg.teid;
  std::map<uint64_t, Ptr<UeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);
  uint64_t enbUeS1Id = it->second->enbUeS1Id;
  uint64_t mmeUeS1Id = it->second->mmeUeS1Id;
  uint16_t cgi = it->second->cellId;
  std::list<EpcS1apSapEnb::ErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList; // unused for now
  std::map< uint16_t, Ptr<EnbInfo> >::iterator jt = m_enbInfoMap.find (it->second->cellId);
  NS_ASSERT_MSG (jt != m_enbInfoMap.end (), "could not find any eNB with CellId " << it->second->cellId);

  Ipv4Address enbS1apAddr = jt->second->s1apAddr;
  Ptr<Socket> s1apSocket = m_s1apSocketMap[enbS1apAddr.Get ()];
  
  EpcS1apHeader epcS1apHeader;
  epcS1apHeader.SetProcedureCode (EpcS1apHeader::PathSwitchRequest);
  epcS1apHeader.SetTypeOfMessage (EpcS1apHeader::SuccessfulOutcome);
  
  PathSwitchRequestAcknowledgeHeader pathSwitchRequestAcknowledgeHeader;
  pathSwitchRequestAcknowledgeHeader.SetMmeUeS1Id (mmeUeS1Id);
  pathSwitchRequestAcknowledgeHeader.SetEnbUeS1Id (enbUeS1Id);
  pathSwitchRequestAcknowledgeHeader.SetCgi (cgi);
  pathSwitchRequestAcknowledgeHeader.SetErabToBeSwitchedInUplinkList (erabToBeSwitchedInUplinkList);

  Ptr<Packet> packet = Create<Packet> ();
  packet->AddHeader (pathSwitchRequestAcknowledgeHeader);
  packet->AddHeader (epcS1apHeader);
  SendToS1apSocket (s1apSocket, packet);
}

void
TapEpcMme::DoErabReleaseIndication (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<EpcS1apSapMme::ErabToBeReleasedIndication> erabToBeReleaseIndication)
{
  NS_LOG_FUNCTION (this << mmeUeS1Id << enbUeS1Id);
  uint64_t imsi = mmeUeS1Id;
  std::map<uint64_t, Ptr<UeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);

  EpcS11SapSgw::DeleteBearerCommandMessage msg;
  // trick to avoid the need for allocating TEIDs on the S11 interface
  msg.teid = imsi;

  for (std::list<EpcS1apSapMme::ErabToBeReleasedIndication>::iterator bit = erabToBeReleaseIndication.begin (); bit != erabToBeReleaseIndication.end (); ++bit)
    {
      EpcS11SapSgw::BearerContextToBeRemoved bearerContext;
      bearerContext.epsBearerId =  bit->erabId;
      msg.bearerContextsToBeRemoved.push_back (bearerContext);
    }
  //Delete Bearer command towards epc-sgw-pgw-application
  m_s11SapSgw->DeleteBearerCommand (msg);
}

void
TapEpcMme::DoDeleteBearerRequest (EpcS11SapMme::DeleteBearerRequestMessage msg)
{
  NS_LOG_FUNCTION (this);
  uint64_t imsi = msg.teid;
  std::map<uint64_t, Ptr<UeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);
  EpcS11SapSgw::DeleteBearerResponseMessage res;

  res.teid = imsi;

  for (std::list<EpcS11SapMme::BearerContextRemoved>::iterator bit = msg.bearerContextsRemoved.begin ();
       bit != msg.bearerContextsRemoved.end ();
       ++bit)
    {
      EpcS11SapSgw::BearerContextRemovedSgwPgw bearerContext;
      bearerContext.epsBearerId = bit->epsBearerId;
      res.bearerContextsRemoved.push_back (bearerContext);

      RemoveBearer (it->second, bearerContext.epsBearerId); //schedules function to erase, context of de-activated bearer
    }
  //schedules Delete Bearer Response towards epc-sgw-pgw-application
  m_s11SapSgw->DeleteBearerResponse (res);
}

void TapEpcMme::RemoveBearer (Ptr<UeInfo> ueInfo, uint8_t epsBearerId)
{
  NS_LOG_FUNCTION (this << epsBearerId);
  for (std::list<BearerInfo>::iterator bit = ueInfo->bearersToBeActivated.begin ();
       bit != ueInfo->bearersToBeActivated.end ();
       ++bit)
    {
      if (bit->bearerId == epsBearerId)
        {
          ueInfo->bearersToBeActivated.erase (bit);
          break;
        }
    }
}

void TapEpcMme::PathSwitchRequest (uint64_t enbUeS1Id, uint64_t mmeUeS1Id, uint16_t cgi, std::list<EpcS1apSapMme::ErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList)
{
  DoPathSwitchRequest (enbUeS1Id, mmeUeS1Id, cgi, erabToBeSwitchedInDownlinkList);
}

void TapEpcMme::ErabReleaseIndication (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<EpcS1apSapMme::ErabToBeReleasedIndication> erabToBeReleaseIndication)
{
  return DoErabReleaseIndication (mmeUeS1Id, enbUeS1Id, erabToBeReleaseIndication);
}

void TapEpcMme::InitialUeMessage (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, uint64_t imsi, uint16_t ecgi)
{
  DoInitialUeMessage (mmeUeS1Id, enbUeS1Id, imsi, ecgi);
}

} // namespace ns3
