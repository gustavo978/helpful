/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011-2013 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Jaume Nin <jnin@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 *         Manuel Requena <manuel.requena@cttc.es>
 */

#include <ns3/tap-epc-helper.h>
#include <ns3/epc-helper-header.h>
#include <ns3/log.h>
#include <ns3/inet-socket-address.h>
#include <ns3/mac48-address.h>
#include <ns3/arp-cache.h>
#include <ns3/eps-bearer.h>
#include <ns3/ipv4-address.h>
#include <ns3/ipv4-interface.h>
#include <ns3/internet-stack-helper.h>
#include <ns3/packet-socket-helper.h>
#include <ns3/packet-socket-address.h>
#include <ns3/tap-epc-enb-application.h>
#include <ns3/epc-sgw-pgw-application.h>
#include <ns3/tap-fd-net-device-helper.h>
#include <ns3/callback.h>

#include <ns3/lte-enb-rrc.h>
#include <ns3/epc-x2.h>
#include <ns3/lte-enb-net-device.h>
#include <ns3/lte-ue-net-device.h>
#include <ns3/tap-epc-mme.h>
#include <ns3/epc-s1ap-header.h>
#include <ns3/epc-s1ap-sap.h>
#include <ns3/epc-ue-nas.h>
#include <ns3/string.h>
#include <ns3/enum.h>
#include <ns3/uinteger.h>
#include <ns3/abort.h>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TapEpcHelper");

NS_OBJECT_ENSURE_REGISTERED (TapEpcHelper);

TapEpcHelper::TapEpcHelper () 
  : m_gtpuUdpPort (2152),  // fixed by the standard
    m_s1apTcpPort (36412),
    m_masterTcpPort (36413)
{
  m_scheduleTime = Seconds (0.0);
  NS_LOG_FUNCTION (this);
}

TapEpcHelper::~TapEpcHelper ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
TapEpcHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TapEpcHelper")
    .SetParent<EpcHelper> ()
    .AddConstructor<TapEpcHelper> ()
    .AddAttribute ("SgwDeviceName",
                   "The name of the device used for the S1-U interface of the SGW",
                   StringValue ("sgwTap"),
                   MakeStringAccessor (&TapEpcHelper::m_sgwDeviceName),
                   MakeStringChecker ())
    .AddAttribute ("EnbDeviceName",
                   "The name of the device used for the S1-U interface of the eNB",
                   StringValue ("enbTap"),
                   MakeStringAccessor (&TapEpcHelper::m_enbDeviceName),
                   MakeStringChecker ())
    .AddAttribute ("MmeDeviceName",
                   "The name of the device used for the S1-AP interface of the MME",
                   StringValue ("mmeTap"),
                   MakeStringAccessor (&TapEpcHelper::m_mmeDeviceName),
                   MakeStringChecker ())
    .AddAttribute ("MasterDeviceName",
                   "The name of the device used for Master",
                   StringValue ("masterTap"),
                   MakeStringAccessor (&TapEpcHelper::m_masterDeviceName),
                   MakeStringChecker ())
    .AddAttribute ("EpcSlaveDeviceName",
                   "The name of the device used for Slave",
                   StringValue ("slaveTap"),
                   MakeStringAccessor (&TapEpcHelper::m_slaveDeviceName),
                   MakeStringChecker ())
    .AddAttribute ("Mode",
                   "The operating mode to use",
                   EnumValue (Master),
                   MakeEnumAccessor (&TapEpcHelper::m_mode),
                   MakeEnumChecker (Master, "Master",
                                    Slave, "Slave"))
    .AddAttribute ("MasterUeIpAddressBase",
                   "The base of master mode UE IP address",
                   StringValue ("0.0.0.1"),
                   MakeStringAccessor (&TapEpcHelper::m_masterUeIpAddressBase),
                   MakeStringChecker ())
    .AddAttribute ("SlaveUeIpAddressBase",
                   "The base of slave mode UE IP address",
                   StringValue ("0.0.1.1"),
                   MakeStringAccessor (&TapEpcHelper::m_slaveUeIpAddressBase),
                   MakeStringChecker ())
    .AddAttribute ("MasterIpAddressBase",
                   "The base of master mode IP address",
                   StringValue ("0.0.0.1"),
                   MakeStringAccessor (&TapEpcHelper::m_masterIpAddressBase),
                   MakeStringChecker ())
    .AddAttribute ("SlaveIpAddressBase",
                   "The base of slave mode IP address",
                   StringValue ("0.0.1.1"),
                   MakeStringAccessor (&TapEpcHelper::m_slaveIpAddressBase),
                   MakeStringChecker ())
    .AddAttribute ("TunMacAddress", 
                   "MAC address used for the TUN",
                   StringValue ("00:00:00:59:00:aa"),
                   MakeStringAccessor (&TapEpcHelper::m_tunMacAddress),
                   MakeStringChecker ())
    .AddAttribute ("SgwMacAddress", 
                   "MAC address used for the SGW",
                   StringValue ("00:00:00:59:00:ab"),
                   MakeStringAccessor (&TapEpcHelper::m_sgwMacAddress),
                   MakeStringChecker ())
    .AddAttribute ("MmeMacAddress", 
                   "MAC address used for the MME",
                   StringValue ("00:00:00:59:00:ac"),
                   MakeStringAccessor (&TapEpcHelper::m_mmeMacAddress),
                   MakeStringChecker ())
    .AddAttribute ("MasterMacAddress", 
                   "MAC address used for the Master",
                   StringValue ("00:00:00:59:00:ad"),
                   MakeStringAccessor (&TapEpcHelper::m_masterMacAddress),
                   MakeStringChecker ())
    .AddAttribute ("LogFile",
                   "Set this attribute to redirect NS_LOG to std::ofstream",
                   StringValue (""),
                   MakeStringAccessor (&TapEpcHelper::m_logFile),
                   MakeStringChecker ())
    ;
  return tid;
}

void
TapEpcHelper::DoInitialize ()
{
  
  if (m_logFile.compare ("") != 0)
    {
      const unsigned int length = 8192;
      char buffer[length];
      m_fileStream.open (m_logFile.c_str ());
      m_fileStream.rdbuf()->pubsetbuf(buffer, length);
      std::clog.rdbuf (m_fileStream.rdbuf ());
      std::cerr.rdbuf (m_fileStream.rdbuf ());
    }

  if (m_mode == Master)
    {
      MasterInitialize ();
    }
  else if (m_mode == Slave)
    {
      SlaveInitialize ();
    }
}

void
TapEpcHelper::MasterInitialize ()
{
  int retval;

  // we use a /8 net for all UEs
  m_ueAddressHelper.SetBase ("7.0.0.0", "255.0.0.0", m_masterUeIpAddressBase.c_str ());
  
  InternetStackHelper internet;
  internet.SetIpv6StackInstall (false);
  
  // create SgwPgwNode
  m_sgwPgw = CreateObject<Node> ();
  internet.Install (m_sgwPgw);
  
  // create MmeNode
  m_mmeNode = CreateObject<Node> ();
  internet.Install (m_mmeNode);

  m_master = CreateObject<Node> ();
  internet.Install (m_master);
  
  // create TUN device implementing tunneling of user data over GTP-U/UDP/IP
  m_tunDevice = CreateObject<VirtualNetDevice> ();
  // allow jumbo packets
  m_tunDevice->SetAttribute ("Mtu", UintegerValue (30000));

  // allocate a Mac48Address for TUN device
  m_tunDevice->SetAddress (Mac48Address (m_tunMacAddress.c_str ()));
  NS_LOG_LOGIC ("MAC address of TUN device: " << m_tunMacAddress.c_str());
  m_ueDefaultGatewayMacAddress = m_tunMacAddress;

  m_sgwPgw->AddDevice (m_tunDevice);
  NetDeviceContainer tunDeviceContainer;
  tunDeviceContainer.Add (m_tunDevice);
  
  // the TUN device is on the same subnet as the UEs, so when a packet
  // addressed to an UE arrives at the intenet to the WAN interface of
  // the PGW it will be forwarded to the TUN device.
  Ipv4InterfaceContainer tunDeviceIpv4IfContainer = m_ueAddressHelper.Assign (tunDeviceContainer);
  NS_LOG_LOGIC ("IP address of TUN device: " << tunDeviceIpv4IfContainer.GetAddress (0));
  m_ueDefaultGatewayAddress = tunDeviceIpv4IfContainer.GetAddress (0);
  
  // create TapFdNetDevice for SGW
  TapFdNetDeviceHelper tap;
  NS_LOG_LOGIC ("SGW device: " << m_sgwDeviceName);
  tap.SetDeviceName (m_sgwDeviceName);
  tap.SetTapMacAddress (Mac48Address (m_sgwMacAddress.c_str ()));
  tap.SetDscpMarking (FdNetDevice::MarkDscp);
  NetDeviceContainer sgwDevices = tap.Install (m_sgwPgw);
  sgwDevices.Get (0)->SetAddress (Mac48Address (m_sgwMacAddress.c_str ()));
  NS_LOG_LOGIC ("MAC address of SGW: " << sgwDevices.Get (0)->GetAddress ());
  
  // create TapFdNetDevice for MME
  NS_LOG_LOGIC ("MME device: " << m_mmeDeviceName);
  tap.SetDeviceName (m_mmeDeviceName);
  tap.SetTapMacAddress (Mac48Address (m_mmeMacAddress.c_str ()));
  NetDeviceContainer mmeDevices = tap.Install (m_mmeNode);
  mmeDevices.Get (0)->SetAddress (Mac48Address (m_mmeMacAddress.c_str ()));
  NS_LOG_LOGIC ("MAC address of MME: " << mmeDevices.Get (0)->GetAddress ());
  
  // create TapFdNetDevice for EpcMaster
  NS_LOG_LOGIC ("Master device: " << m_masterDeviceName);
  tap.SetDeviceName (m_masterDeviceName);
  tap.SetTapMacAddress (Mac48Address (m_masterMacAddress.c_str()));
  NetDeviceContainer masterDevices = tap.Install (m_master);
  masterDevices.Get (0)->SetAddress (Mac48Address (m_masterMacAddress.c_str ()));
  NS_LOG_LOGIC ("MAC address of Master: " << masterDevices.Get (0)->GetAddress ());
  
  // we use a /8 subnet so the SGW and the eNBs can talk directly to each other
  m_epcIpv4AddressHelper.SetBase ("10.0.0.0", "255.255.0.0", m_masterIpAddressBase.c_str ());
  Ipv4InterfaceContainer sgwIpIfaces = m_epcIpv4AddressHelper.Assign (sgwDevices.Get (0));
  m_sgwIpv4Address = sgwIpIfaces.GetAddress (0);
  NS_LOG_LOGIC ("IP address of SGW: " << m_sgwIpv4Address);
  Ipv4InterfaceContainer mmeIpIfaces = m_epcIpv4AddressHelper.Assign (mmeDevices.Get (0));
  m_mmeIpv4Address = mmeIpIfaces.GetAddress (0);
  NS_LOG_LOGIC ("IP address of MME: " << m_mmeIpv4Address);
  Ipv4InterfaceContainer masterIpIfaces = m_epcIpv4AddressHelper.Assign (masterDevices.Get (0));
  m_masterIpv4Address = masterIpIfaces.GetAddress (0);
  NS_LOG_LOGIC ("IP address of EpcMaster: " << m_masterIpv4Address);
  // m_epcIpv4AddressHelper.SetBase ("10.0.0.0", "255.0.0.0", "0.0.0.101");
  
  // create S1-U socket
  Ptr<Socket> sgwPgwS1uSocket = Socket::CreateSocket (m_sgwPgw, TypeId::LookupByName ("ns3::UdpSocketFactory"));
  retval = sgwPgwS1uSocket->Bind (InetSocketAddress (Ipv4Address::GetAny (), m_gtpuUdpPort));
  NS_ASSERT (retval == 0);
  
  // create S1-AP socket
  Ptr<Socket> mmeS1apSocket = Socket::CreateSocket (m_mmeNode, TypeId::LookupByName ("ns3::TcpSocketFactory"));
  retval = mmeS1apSocket->Bind (InetSocketAddress (Ipv4Address::GetAny (), m_s1apTcpPort));
  NS_ASSERT (retval == 0);
  mmeS1apSocket->Listen ();
  mmeS1apSocket->SetAcceptCallback (MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
                                    MakeCallback (&TapEpcHelper::HandleS1apConnection, this));
                                    
  // create EpcMaster socket
  Ptr<Socket> masterSocket = Socket::CreateSocket (m_master, TypeId::LookupByName ("ns3::TcpSocketFactory"));
  retval = masterSocket->Bind (InetSocketAddress (Ipv4Address::GetAny (), m_masterTcpPort));
  NS_ASSERT (retval == 0);
  masterSocket->Listen ();
  masterSocket->SetAcceptCallback (MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
                                   MakeCallback (&TapEpcHelper::HandleMasterConnection, this));

  // create EpcSgwPgwApplication
  m_sgwPgwApp = CreateObject<EpcSgwPgwApplication> (m_tunDevice, sgwPgwS1uSocket);
  m_sgwPgw->AddApplication (m_sgwPgwApp);
  
  // connect SgwPgwApplication and virtual net device for tunneling
  m_tunDevice->SetSendCallback (MakeCallback (&EpcSgwPgwApplication::RecvFromTunDevice, m_sgwPgwApp));
  
  // create MME and connect with SGW via S11 interface
  m_mme = CreateObject<TapEpcMme> ();
  m_mme->SetS11SapSgw (m_sgwPgwApp->GetS11SapSgw ());
  m_sgwPgwApp->SetS11SapMme (m_mme->GetS11SapMme ());
  
  EpcHelper::DoInitialize ();
}

void
TapEpcHelper::SlaveInitialize ()
{
  
  // we use a /8 net for all UEs
  m_ueAddressHelper.SetBase ("7.0.0.0", "255.0.0.0", m_masterUeIpAddressBase.c_str ());
  m_ueDefaultGatewayAddress = m_ueAddressHelper.NewAddress ();
  m_ueAddressHelper.SetBase ("7.0.0.0", "255.0.0.0", m_slaveUeIpAddressBase.c_str ());
  
  m_epcIpv4AddressHelper.SetBase ("10.0.0.0", "255.255.0.0", m_masterIpAddressBase.c_str ());
  m_sgwIpv4Address = m_epcIpv4AddressHelper.NewAddress ();
  NS_LOG_LOGIC ("IP address of SGW: " << m_sgwIpv4Address);
  m_mmeIpv4Address = m_epcIpv4AddressHelper.NewAddress ();
  NS_LOG_LOGIC ("IP address of MME: " << m_mmeIpv4Address);
  m_masterIpv4Address = m_epcIpv4AddressHelper.NewAddress ();
  NS_LOG_LOGIC ("IP address of EpcMaster: " << m_masterIpv4Address);
  
  m_ueDefaultGatewayMacAddress = m_tunMacAddress;
  
  InternetStackHelper internet;
  internet.SetIpv6StackInstall (false);

  m_slave = CreateObject<Node> ();
  internet.Install (m_slave);
  
  // create TapFdNetDevice for EpcSlave
  NS_LOG_LOGIC ("EpcSlave device: " << m_slaveDeviceName);
  TapFdNetDeviceHelper tap;
  tap.SetDeviceName (m_slaveDeviceName);
  uint64_t key = 0;
  for (const char *base = m_slaveIpAddressBase.c_str (); *base != '\0'; ++base)
    {
      key = key + *base;
    }
  tap.SetTapMacAddress (TapEpcHelper::AllocateMac48Address (static_cast<uint64_t> (key)));
  NetDeviceContainer slaveDevices = tap.Install (m_slave);
  slaveDevices.Get (0)->SetAddress (TapEpcHelper::AllocateMac48Address (static_cast<uint64_t> (key)));
  
  m_epcIpv4AddressHelper.SetBase ("10.0.0.0", "255.255.0.0", m_slaveIpAddressBase.c_str ());
  Ipv4InterfaceContainer slaveIpIfaces = m_epcIpv4AddressHelper.Assign (slaveDevices.Get (0));
  
  NS_LOG_LOGIC ("IP address of Slave: " << slaveIpIfaces.GetAddress (0));

  // create EpcSlave socket
  m_slaveSocket = Socket::CreateSocket (m_slave, TypeId::LookupByName ("ns3::TcpSocketFactory"));
  int retval = m_slaveSocket->Connect (InetSocketAddress (m_masterIpv4Address, m_masterTcpPort));
  NS_ASSERT (retval == 0);
  m_slaveSocket->SetRecvCallback (MakeCallback (&TapEpcHelper::RecvFromSlaveSocket, this));
  
  m_slaveMme = CreateObject<TapEpcMme> ();

  EpcHelper::DoInitialize ();
}

void
TapEpcHelper::HandleS1apConnection (Ptr<Socket> socket, const Address &addr)
{
  NS_LOG_FUNCTION (this << socket << addr);
  m_mme->AddS1apSocket (addr, socket);
  socket->SetRecvCallback (MakeCallback (&TapEpcHelper::RecvFromS1apSocket, this));
}

void
TapEpcHelper::RecvFromSlaveSocket (Ptr<Socket> socket)
{
  Ptr<Packet> packet = socket->Recv ();
  
  EpcHelperHeader epcHelperHeader;
  packet->RemoveHeader (epcHelperHeader);
  
  uint8_t procedureCode = epcHelperHeader.GetProcedureCode ();
  uint8_t typeOfMessage = epcHelperHeader.GetTypeOfMessage ();
  
  if (procedureCode == EpcHelperHeader::ActivateEpsBearer)
    {
      if (typeOfMessage == EpcHelperHeader::SuccessfulOutcome)
        {
          NS_LOG_LOGIC ("TapEpcHelper::RecvFromSlaveSocket ActivateEpsBearerResponse");
          ActivateEpsBearerResponseHeader activateEpsBearerResponseHeader;
          packet->RemoveHeader (activateEpsBearerResponseHeader);
          uint8_t bearerId = activateEpsBearerResponseHeader.GetBearerId ();
          NS_LOG_LOGIC ("bearerId = " << (int) bearerId);
        }
    }
}

void
TapEpcHelper::SendToSlaveSocket (Ptr<Packet> packet)
{
  if (Simulator::Now () == Seconds (0.0))
    {
      Simulator::Schedule (m_scheduleTime, static_cast<int (Socket::*)(Ptr<Packet>)>(&Socket::Send), m_slaveSocket, packet);
      m_scheduleTime = m_scheduleTime + Seconds (0.1);
    }
  else
    {
      Simulator::ScheduleNow (static_cast<int (Socket::*)(Ptr<Packet>)>(&Socket::Send), m_slaveSocket, packet);
    }
}

void
TapEpcHelper::RecvFromMasterSocket (Ptr<Socket> socket)
{
  Ptr<Packet> packet = socket->Recv ();
  HandleMasterPacket (socket, packet);
}

void
TapEpcHelper::HandleMasterConnection (Ptr<Socket> socket, const Address &addr)
{
  socket->SetRecvCallback (MakeCallback (&TapEpcHelper::RecvFromMasterSocket, this));
}

void
TapEpcHelper::HandleMasterPacket (Ptr<Socket> socket, Ptr<Packet> packet)
{
  NS_LOG_LOGIC ("TapEpcHelper::HandleMasterPacket packet->GetSize ():" << packet->GetSize ());
  EpcHelperHeader epcHelperHeader;
  packet->RemoveHeader (epcHelperHeader);
  
  uint8_t procedureCode = epcHelperHeader.GetProcedureCode ();
  uint8_t typeOfMessage = epcHelperHeader.GetTypeOfMessage ();
  
  if (procedureCode == EpcHelperHeader::AddUe)
    {
      if (typeOfMessage == EpcHelperHeader::InitiatingMessage)
        {
          NS_LOG_LOGIC ("TapEpcHelper::HandleMasterPacket AddUeRequest");
          AddUeRequestHeader addUeRequestHeader;
          packet->RemoveHeader (addUeRequestHeader);
          
          uint64_t imsi = addUeRequestHeader.GetImsi ();
          m_mme->AddUe (imsi);
          m_sgwPgwApp->AddUe (imsi);
        }
    }
  else if (procedureCode == EpcHelperHeader::AddEnb)
    {
      if (typeOfMessage == EpcHelperHeader::InitiatingMessage)
        {
          NS_LOG_LOGIC ("TapEpcHelper::HandleMasterPacket AddEnbRequest");
          AddEnbRequestHeader addEnbRequestHeader;
          packet->RemoveHeader (addEnbRequestHeader);
          
          uint16_t cellId = addEnbRequestHeader.GetCellId ();
          Ipv4Address enbAddress = addEnbRequestHeader.GetEnbAddress ();
          Ipv4Address sgwAddress = addEnbRequestHeader.GetSgwAddress ();
      
          m_mme->AddEnb (cellId, enbAddress, enbAddress);
          m_sgwPgwApp->AddEnb (cellId, enbAddress, sgwAddress);
        }
    }
  else if (procedureCode == EpcHelperHeader::ActivateEpsBearer)
    {
      if (typeOfMessage == EpcHelperHeader::InitiatingMessage)
        {
          NS_LOG_LOGIC ("TapEpcHelper::HandleMasterPacket ActivateEpsBearerRequest");
          ActivateEpsBearerRequestHeader activateEpsBearerRequestHeader;
          packet->RemoveHeader (activateEpsBearerRequestHeader);
          
          uint64_t imsi = activateEpsBearerRequestHeader.GetImsi ();
          Ipv4Address ueAddr = activateEpsBearerRequestHeader.GetUeAddress ();
          EpcTft epcTft = activateEpsBearerRequestHeader.GetEpcTft ();
          EpsBearer bearer = activateEpsBearerRequestHeader.GetEpsBearer ();
          
          Ptr<EpcTft> tft = Create<EpcTft> (epcTft);
          NS_LOG_LOGIC ("imsi = " << imsi << " ueAddr = " << ueAddr);
          m_sgwPgwApp->SetUeAddress (imsi, ueAddr);
          uint8_t bearerId = m_mme->AddBearer (imsi, tft, bearer);
          NS_LOG_LOGIC ("bearerId = " << (int) bearerId);
          
          epcHelperHeader.SetProcedureCode (EpcHelperHeader::ActivateEpsBearer);
          epcHelperHeader.SetTypeOfMessage (EpcHelperHeader::SuccessfulOutcome);

          ActivateEpsBearerResponseHeader activateEpsBearerResponseHeader;
          activateEpsBearerResponseHeader.SetBearerId (bearerId);

          Ptr<Packet> responsePacket = Create<Packet> ();
          responsePacket->AddHeader (activateEpsBearerResponseHeader);
          responsePacket->AddHeader (epcHelperHeader);

          socket->Send (responsePacket);
        }
    }
    
  if (packet->GetSize () != 0)
    {
      HandleMasterPacket (socket, packet);
    }
}

void
TapEpcHelper::RecvFromS1apSocket (Ptr<Socket> socket)
{
  Ptr<Packet> packet = socket->Recv ();
  HandleS1apPacket (socket, packet);
}

void
TapEpcHelper::HandleS1apPacket (Ptr<Socket> socket, Ptr<Packet> packet)
{
  EpcS1apHeader EpcS1apHeader;
  packet->RemoveHeader (EpcS1apHeader);
  
  uint8_t procedureCode = EpcS1apHeader.GetProcedureCode ();
  uint8_t typeOfMessage = EpcS1apHeader.GetTypeOfMessage ();
  
  if (procedureCode == EpcS1apHeader::InitialUeMessage)
    {
      if (typeOfMessage == EpcS1apHeader::InitiatingMessage)
        {
          NS_LOG_LOGIC ("TapEpcHelper::HandleS1apPacket InitialUeMessage");
          InitialUeRequestHeader initialUeRequestHeader;
          packet->RemoveHeader (initialUeRequestHeader);
          
          uint64_t mmeUeS1Id = initialUeRequestHeader.GetMmeUeS1Id ();
          uint16_t enbUeS1Id = initialUeRequestHeader.GetEnbUeS1Id ();
          uint64_t imsi = initialUeRequestHeader.GetStmsi ();
          uint16_t ecgi = initialUeRequestHeader.GetEcgi ();
          
          m_mme->InitialUeMessage (mmeUeS1Id, enbUeS1Id, imsi, ecgi);
        }
    }
  else if (procedureCode == EpcS1apHeader::PathSwitchRequest)
    {
      if (typeOfMessage == EpcS1apHeader::InitiatingMessage)
        {
          NS_LOG_LOGIC ("TapEpcHelper::HandleS1apPacket PathSwitchRequest");
          PathSwitchRequestHeader pathSwitchRequestHeader;
          packet->RemoveHeader (pathSwitchRequestHeader);
          
          uint64_t enbUeS1Id = pathSwitchRequestHeader.GetEnbUeS1Id ();
          uint64_t mmeUeS1Id = pathSwitchRequestHeader.GetMmeUeS1Id ();
          uint16_t cgi = pathSwitchRequestHeader.GetCgi ();
          
          std::list<EpcS1apSapMme::ErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList;
          erabToBeSwitchedInDownlinkList = pathSwitchRequestHeader.GetErabToBeSwitchedInDownlinkList ();
          
          m_mme->PathSwitchRequest (enbUeS1Id, mmeUeS1Id, cgi, erabToBeSwitchedInDownlinkList);
        }
    }
  else if (procedureCode == EpcS1apHeader::ErabRelease)
    {
      if (typeOfMessage == EpcS1apHeader::InitiatingMessage)
        {
          NS_LOG_LOGIC ("TapEpcHelper::HandleS1apPacket ErabRelease");
          ErabReleaseIndicationHeader erabReleaseIndicationHeader;
          packet->RemoveHeader (erabReleaseIndicationHeader);
          
          uint64_t imsi = erabReleaseIndicationHeader.GetMmeUeS1Id ();
          uint16_t rnti = erabReleaseIndicationHeader.GetEnbUeS1Id ();
          
          std::list<EpcS1apSapMme::ErabToBeReleasedIndication> erabToBeReleaseIndication;
          erabToBeReleaseIndication =  erabReleaseIndicationHeader.GetErabToBeReleaseIndication ();
          
          m_mme->ErabReleaseIndication (imsi, rnti, erabToBeReleaseIndication);
        }
    }
  
  if (packet->GetSize () != 0)
    {
      HandleS1apPacket (socket, packet);
    }
}

void
TapEpcHelper::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_tunDevice->SetSendCallback (MakeNullCallback<bool, Ptr<Packet>, const Address &, const Address &, uint16_t> ());
  m_tunDevice = 0;
  m_sgwPgwApp = 0;  
  m_sgwPgw->Dispose ();
  if (m_logFile.compare ("") != 0)
    {
      m_fileStream.close ();
    }
}

void
TapEpcHelper::AddEnb (Ptr<Node> enb, Ptr<NetDevice> lteEnbNetDevice, uint16_t cellId)
{
  NS_LOG_FUNCTION (this << enb << lteEnbNetDevice << cellId);

  Initialize ();

  NS_ASSERT (enb == lteEnbNetDevice->GetNode ());  

  // add an IPv4 stack to the previously created eNB
  InternetStackHelper internet;
  internet.SetIpv6StackInstall (false);
  internet.Install (enb);
  
  // create an TapFdNetDevice for the eNB to connect with the SGW and other eNBs
  std::ostringstream enbDevNameWithCount;
  enbDevNameWithCount << m_enbDeviceName << cellId;
  TapFdNetDeviceHelper tap;
  NS_LOG_LOGIC ("eNB device: " << enbDevNameWithCount.str());
  tap.SetDeviceName (enbDevNameWithCount.str());
  tap.SetTapMacAddress (TapEpcHelper::AllocateMac48Address (static_cast<uint64_t> (cellId)));
  tap.SetDscpMarking (FdNetDevice::MarkDscp);
  NetDeviceContainer enbDevices = tap.Install (enb);
  enbDevices.Get (0)->SetAddress (TapEpcHelper::AllocateMac48Address (static_cast<uint64_t> (cellId)));
  NS_LOG_LOGIC ("MAC address of eNB: " << enbDevices.Get (0)->GetAddress ());

  Ipv4InterfaceContainer enbIpIfaces = m_epcIpv4AddressHelper.Assign (enbDevices.Get (0));
  Ipv4Address enbAddress = enbIpIfaces.GetAddress (0);
  NS_LOG_LOGIC ("IP address of eNB: " << enbAddress);
  
  // setup ARP cache
  NS_LOG_LOGIC ("Number of IPv4 Interfaces on eNB: " << enb->GetObject<Ipv4L3Protocol> ()->GetNInterfaces ());
  Ptr<Ipv4Interface> ipIface = enb->GetObject<Ipv4L3Protocol> ()->GetInterface (1);
  Ptr<ArpCache> arpCache = CreateObject<ArpCache> ();
  arpCache->SetAliveTimeout (Seconds(3600 * 24));
  
  ArpCache::Entry *entry = arpCache->Add (m_sgwIpv4Address);
  entry->MarkWaitReply (0);
  entry->MarkAlive (Mac48Address (m_sgwMacAddress.c_str ()));
  
  entry = arpCache->Add (m_mmeIpv4Address);
  entry->MarkWaitReply(0);
  entry->MarkAlive (Mac48Address (m_mmeMacAddress.c_str ()));
  
  entry = arpCache->Add (m_masterIpv4Address);
  entry->MarkWaitReply (0);
  entry->MarkAlive (Mac48Address (m_masterMacAddress.c_str ()));
  
  ipIface->SetArpCache (arpCache);
  
  // create S1-U socket for the ENB
  Ptr<Socket> enbS1uSocket = Socket::CreateSocket (enb, TypeId::LookupByName ("ns3::UdpSocketFactory"));
  int retval = enbS1uSocket->Bind (InetSocketAddress (enbAddress, m_gtpuUdpPort));
  NS_ASSERT (retval == 0);
  
  // create S1-AP socket for the ENB
  Ptr<Socket> enbS1apSocket = Socket::CreateSocket (enb, TypeId::LookupByName ("ns3::TcpSocketFactory"));
  retval = enbS1apSocket->Connect (InetSocketAddress (m_mmeIpv4Address, m_s1apTcpPort));
  NS_ASSERT (retval == 0);
    
  // create LTE socket for the ENB 
  Ptr<Socket> enbLteSocket = Socket::CreateSocket (enb, TypeId::LookupByName ("ns3::PacketSocketFactory"));
  PacketSocketAddress enbLteSocketBindAddress;
  enbLteSocketBindAddress.SetSingleDevice (lteEnbNetDevice->GetIfIndex ());
  enbLteSocketBindAddress.SetProtocol (Ipv4L3Protocol::PROT_NUMBER);
  retval = enbLteSocket->Bind (enbLteSocketBindAddress);
  NS_ASSERT (retval == 0);  
  PacketSocketAddress enbLteSocketConnectAddress;
  enbLteSocketConnectAddress.SetPhysicalAddress (Mac48Address::GetBroadcast ());
  enbLteSocketConnectAddress.SetSingleDevice (lteEnbNetDevice->GetIfIndex ());
  enbLteSocketConnectAddress.SetProtocol (Ipv4L3Protocol::PROT_NUMBER);
  retval = enbLteSocket->Connect (enbLteSocketConnectAddress);
  NS_ASSERT (retval == 0);
  
  NS_LOG_INFO ("Create EpcEnbApplication");
  Ptr<TapEpcEnbApplication> enbApp = CreateObject<TapEpcEnbApplication> (enbLteSocket, enbS1uSocket, enbS1apSocket, enbAddress, m_sgwIpv4Address, cellId);
  enb->AddApplication (enbApp);
  NS_ASSERT (enb->GetNApplications () == 1);
  NS_ASSERT_MSG (enb->GetApplication (0)->GetObject<TapEpcEnbApplication> () != 0, "cannot retrieve EpcEnbApplication");
  NS_LOG_LOGIC ("enb: " << enb << ", enb->GetApplication (0): " << enb->GetApplication (0));

  NS_LOG_INFO ("Create EpcX2 entity");
  Ptr<EpcX2> x2 = CreateObject<EpcX2> ();
  enb->AggregateObject (x2);

  NS_LOG_INFO ("Connect S1-AP interface");
  if (m_mode == Master)
    {
      m_mme->AddEnb (cellId, enbAddress, enbAddress);
      m_sgwPgwApp->AddEnb (cellId, enbAddress, m_sgwIpv4Address);
    }
  else if(m_mode == Slave)
    {
      EpcHelperHeader epcHelperHeader;
      epcHelperHeader.SetProcedureCode (EpcHelperHeader::AddEnb);
      epcHelperHeader.SetTypeOfMessage (EpcHelperHeader::InitiatingMessage);
    
      AddEnbRequestHeader addEnbRequestHeader;
      addEnbRequestHeader.SetCellId (cellId);
      addEnbRequestHeader.SetEnbAddress (enbAddress);
      addEnbRequestHeader.SetSgwAddress (m_sgwIpv4Address);
      
      Ptr<Packet> packet = Create<Packet> ();
      packet->AddHeader (addEnbRequestHeader);
      packet->AddHeader (epcHelperHeader);
      SendToSlaveSocket (packet);
    }
}

void
TapEpcHelper::AddX2Interface (Ptr<Node> enb1, Ptr<Node> enb2)
{
  NS_LOG_FUNCTION (this << enb1 << enb2);

  NS_LOG_WARN ("X2 support still untested");

  // for X2, we reuse the same device and IP address of the S1-U interface
  Ptr<Ipv4> enb1Ipv4 = enb1->GetObject<Ipv4> ();
  Ptr<Ipv4> enb2Ipv4 = enb2->GetObject<Ipv4> ();
  NS_LOG_LOGIC ("number of Ipv4 ifaces of the eNB #1: " << enb1Ipv4->GetNInterfaces ());
  NS_LOG_LOGIC ("number of Ipv4 ifaces of the eNB #2: " << enb2Ipv4->GetNInterfaces ());
  NS_LOG_LOGIC ("number of NetDevices of the eNB #1: " << enb1->GetNDevices ());
  NS_LOG_LOGIC ("number of NetDevices of the eNB #2: " << enb2->GetNDevices ());

  // 0 is the LTE device, 1 is localhost, 2 is the EPC NetDevice
  Ptr<NetDevice> enb1EpcDev = enb1->GetDevice (2);
  Ptr<NetDevice> enb2EpcDev = enb2->GetDevice (2);

  int32_t enb1Interface =  enb1Ipv4->GetInterfaceForDevice (enb1EpcDev);
  int32_t enb2Interface =  enb2Ipv4->GetInterfaceForDevice (enb2EpcDev);
  NS_ASSERT (enb1Interface >= 0);
  NS_ASSERT (enb2Interface >= 0);
  NS_ASSERT (enb1Ipv4->GetNAddresses (enb1Interface) == 1);
  NS_ASSERT (enb2Ipv4->GetNAddresses (enb2Interface) == 1);
  Ipv4Address enb1Addr = enb1Ipv4->GetAddress (enb1Interface, 0).GetLocal (); 
  Ipv4Address enb2Addr = enb2Ipv4->GetAddress (enb2Interface, 0).GetLocal (); 
  NS_LOG_LOGIC (" eNB 1 IP address: " << enb1Addr); 
  NS_LOG_LOGIC (" eNB 2 IP address: " << enb2Addr);
  
  // Add X2 interface to both eNBs' X2 entities
  Ptr<EpcX2> enb1X2 = enb1->GetObject<EpcX2> ();
  Ptr<LteEnbNetDevice> enb1LteDev = enb1->GetDevice (0)->GetObject<LteEnbNetDevice> ();
  uint16_t enb1CellId = enb1LteDev->GetCellId ();
  NS_LOG_LOGIC ("LteEnbNetDevice #1 = " << enb1LteDev << " - CellId = " << enb1CellId);

  Ptr<EpcX2> enb2X2 = enb2->GetObject<EpcX2> ();
  Ptr<LteEnbNetDevice> enb2LteDev = enb2->GetDevice (0)->GetObject<LteEnbNetDevice> ();
  uint16_t enb2CellId = enb2LteDev->GetCellId ();
  NS_LOG_LOGIC ("LteEnbNetDevice #2 = " << enb2LteDev << " - CellId = " << enb2CellId);

  enb1X2->AddX2Interface (enb1CellId, enb1Addr, enb2CellId, enb2Addr);
  enb2X2->AddX2Interface (enb2CellId, enb2Addr, enb1CellId, enb1Addr);

  enb1LteDev->GetRrc ()->AddX2Neighbour (enb2LteDev->GetCellId ());
  enb2LteDev->GetRrc ()->AddX2Neighbour (enb1LteDev->GetCellId ());
}

void 
TapEpcHelper::AddUe (Ptr<NetDevice> ueDevice, uint64_t imsi)
{
  NS_LOG_FUNCTION (this << imsi << ueDevice);
  
  if (m_mode == Master)
    {
      m_mme->AddUe (imsi);
      m_sgwPgwApp->AddUe (imsi);
    }
  else if(m_mode == Slave)
    {
      EpcHelperHeader epcHelperHeader;
      epcHelperHeader.SetProcedureCode (EpcHelperHeader::AddUe);
      epcHelperHeader.SetTypeOfMessage (EpcHelperHeader::InitiatingMessage);
    
      AddUeRequestHeader addUeRequestHeader;
      addUeRequestHeader.SetImsi (imsi);
      
      Ptr<Packet> packet = Create<Packet> ();
      packet->AddHeader (addUeRequestHeader);
      packet->AddHeader (epcHelperHeader);
      SendToSlaveSocket (packet);

      m_slaveMme->AddUe (imsi);
    }
}

uint8_t
TapEpcHelper::ActivateEpsBearer (Ptr<NetDevice> ueDevice, uint64_t imsi, Ptr<EpcTft> tft, EpsBearer bearer)
{
  NS_LOG_FUNCTION (this << ueDevice << imsi);

  // we now retrieve the IPv4 address of the UE and notify it to the SGW;
  // we couldn't do it before since address assignment is triggered by
  // the user simulation program, rather than done by the EPC   
  Ptr<Node> ueNode = ueDevice->GetNode (); 
  Ptr<Ipv4> ueIpv4 = ueNode->GetObject<Ipv4> ();
  NS_ASSERT_MSG (ueIpv4 != 0, "UEs need to have IPv4 installed before EPS bearers can be activated");
  int32_t interface =  ueIpv4->GetInterfaceForDevice (ueDevice);
  NS_ASSERT (interface >= 0);
  NS_ASSERT (ueIpv4->GetNAddresses (interface) == 1);
  Ipv4Address ueAddr = ueIpv4->GetAddress (interface, 0).GetLocal ();
  NS_LOG_LOGIC (" UE IP address: " << ueAddr);
  
  uint8_t bearerId = 0;
  if (m_mode == Master)
    {
      m_sgwPgwApp->SetUeAddress (imsi, ueAddr);
      bearerId = m_mme->AddBearer (imsi, tft, bearer);
    }
  else if(m_mode == Slave)
    {
      EpcTft epcTft = *tft;

      EpcHelperHeader epcHelperHeader;
      epcHelperHeader.SetProcedureCode (EpcHelperHeader::ActivateEpsBearer);
      epcHelperHeader.SetTypeOfMessage (EpcHelperHeader::InitiatingMessage);

      ActivateEpsBearerRequestHeader activateEpsBearerRequestHeader;
      activateEpsBearerRequestHeader.SetImsi (imsi);
      activateEpsBearerRequestHeader.SetUeAddress (ueAddr);
      activateEpsBearerRequestHeader.SetEpcTft (epcTft);
      activateEpsBearerRequestHeader.SetEpsBearer (bearer);

      Ptr<Packet> packet = Create<Packet> ();
      packet->AddHeader (activateEpsBearerRequestHeader);
      packet->AddHeader (epcHelperHeader);
      SendToSlaveSocket (packet);

      bearerId = m_slaveMme->AddBearer (imsi, tft, bearer);
    }
  NS_ASSERT (bearerId != 0);
  NS_LOG_LOGIC ("bearerId = " << (int) bearerId);

  Ptr<LteUeNetDevice> ueLteDevice = ueDevice->GetObject<LteUeNetDevice> ();
  if (ueLteDevice)
    {
      Simulator::ScheduleNow (&EpcUeNas::ActivateEpsBearer, ueLteDevice->GetNas (), bearer, tft);
    }
  return bearerId;
}

Ptr<Node>
TapEpcHelper::GetPgwNode ()
{
  return m_sgwPgw;
}

Ipv4InterfaceContainer 
TapEpcHelper::AssignUeIpv4Address (NetDeviceContainer ueDevices)
{
  return m_ueAddressHelper.Assign (ueDevices);
}

Ipv4Address
TapEpcHelper::GetUeDefaultGatewayAddress ()
{
  // return the address of the tun device
  return m_ueDefaultGatewayAddress;
}

Mac48Address
TapEpcHelper::GetUeDefaultGatewayMacAddress ()
{
  return Mac48Address (m_ueDefaultGatewayMacAddress.c_str ());
}

Mac48Address
TapEpcHelper::AllocateMac48Address (uint64_t key)
{
  uint8_t buffer[6];
  
  Mac48Address address;
  buffer[0] = (key >> 40) & 0xff;
  buffer[1] = (key >> 32) & 0xff;
  buffer[2] = (key >> 24) & 0xff;
  buffer[3] = ((key >> 16) & 0xff) | 0xeb;
  buffer[4] = (key >> 8) & 0xff;
  buffer[5] = (key >> 0) & 0xff;
  address.CopyFrom (buffer);

  return address;
}

} // namespace ns3
