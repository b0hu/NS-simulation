/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include "mygym.h"
#include "ns3/core-module.h"
#include "ns3/config-store.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/nr-module.h"
#include "ns3/nr-mac-scheduler-ns3.h"
#include "ns3/nr-mac-scheduler-tdma-rr.h"
#include "ns3/config-store-module.h"
#include "ns3/antenna-module.h"
#include "ns3/node-list.h"
#include "ns3/netanim-module.h"
#include <ctime>

using namespace ns3;

//struct PointInFTPlane temp = new PointInFTPlane(rbg, sym);

//CustomScheduler:: FTResources ft = {rbg, sym};
//NrMacSchedulerNs3::PointInFTPlane * ft = new NrMacSchedulerNs3::PointInFTPlane(rbg, sym);

NS_LOG_COMPONENT_DEFINE ("NS Simulation Test");

int 
main (int argc, char *argv[])
{
  NS_LOG_UNCOND ("eMBB NS Test");

  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (999999999));
  myfile.open ("./lstd1/all.txt");
  embbfile.open ("./lstd1/embb.txt");
  urllcfile.open ("./lstd1/urllc.txt");
  mmtcfile.open ("./lstd1/mmtc.txt");
  rewardfile.open ("./lstd1/reward.txt");

  time_t start = time(NULL);
  // tm *ltm = localtime(&now);

  CommandLine cmd;

  // cmd.AddValue ("PorteMBB", "Port number for OpenGym env. Default: 5555", PorteMBB);
  cmd.AddValue ("simSeed", "Seed for random generator. Default: 1", simSeed);
  // optional parameters
  cmd.AddValue ("simTime", "Simulation time in seconds. Default: 10s", simTimeMs);
  cmd.AddValue ("testArg", "Extra simulation argument. Default: 0", testArg);
  cmd.Parse (argc, argv);

  NS_LOG_UNCOND ("simTimeMs: " << simTimeMs);
  NS_LOG_UNCOND ("envStepTime" << envStepTime);

  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (simSeed);

  // Ptr<OpenGymInterface> openGymInterface = CreateObject<OpenGymInterface> (1234);
  // Ptr<MyGym> myGymEnv = CreateObject<MyGym> ();
  // myGymEnv->SetOpenGymInterface(openGymInterface);

  MobilityHelper enbmobility;
  enbmobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  Ptr<ListPositionAllocator> bsPositionAlloc = CreateObject<ListPositionAllocator>();
  //enbmobility.SetPositionAllocator("ns3::GridPositionAllocator","MinX",DoubleValue(0.0),"MinY",DoubleValue(0.0),"GridWidth",UintegerValue(30),"LayoutType",StringValue("RowFirst"));
  
  for (uint32_t i = 0; i < gNbNum; i++)
  {
    uint32_t x = i%3;
    uint32_t y = i/3;
    bsPositionAlloc->Add(Vector(x * 30 + 30, y * 30 + 30, gNbHeight));
  }
  enbmobility.SetPositionAllocator(bsPositionAlloc);
  gNbNodes.Create(gNbNum);
  enbmobility.Install(gNbNodes);

  MobilityHelper uemobility1;
  MobilityHelper uemobility2;
  // Ptr<ListPositionAllocator> uePositionAlloc = CreateObject<ListPositionAllocator>();
  
  uemobility1.SetPositionAllocator ("ns3::RandomBoxPositionAllocator",
                                 "X", StringValue ("ns3::UniformRandomVariable[Min=0|Max=90]"),
                                 "Y", StringValue ("ns3::UniformRandomVariable[Min=0|Max=90]"),
                                 "Z", StringValue ("ns3::UniformRandomVariable[Min=0|Max=2]"));
  uemobility1.SetMobilityModel("ns3::RandomWalk2dMobilityModel", "Bounds",RectangleValue(Rectangle(0.0, 90.0, 0.0, 90.0)));

  uemobility2.SetPositionAllocator ("ns3::RandomBoxPositionAllocator",
                                 "X", StringValue ("ns3::UniformRandomVariable[Min=10|Max=70]"),
                                 "Y", StringValue ("ns3::UniformRandomVariable[Min=10|Max=70]"),
                                 "Z", StringValue ("ns3::UniformRandomVariable[Min=0|Max=2]"));
  uemobility2.SetMobilityModel("ns3::RandomWalk2dMobilityModel", "Bounds",RectangleValue(Rectangle(10.0, 70.0, 10.0, 70.0)));

  eMBBueNodes1.Create(ueNum);
  uemobility1.Install(eMBBueNodes1);
  mMTCueNodes1.Create(ueNum);
  uemobility2.Install(mMTCueNodes1);
  URLLCueNodes1.Create(ueNum);
  uemobility1.Install(URLLCueNodes1);

  eMBBueNodes2.Create(ueNum);
  uemobility1.Install(eMBBueNodes2);
  mMTCueNodes2.Create(ueNum);
  uemobility2.Install(mMTCueNodes2);
  URLLCueNodes2.Create(ueNum);
  uemobility1.Install(URLLCueNodes2);


  Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper> ();
  Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::CustomScheduler"));
  //nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::NrMacSchedulerOfdmaRR"));

  nrHelper->SetBeamformingHelper (idealBeamformingHelper);
  nrHelper->SetEpcHelper (epcHelper);

  BandwidthPartInfoPtrVector allBwps;
  CcBwpCreator ccBwpCreator;
  const uint8_t numCcPerBand = 1;

  CcBwpCreator::SimpleOperationBandConf bandConfFdd (centralFrequencyBand, bandwidthBand1, numCcPerBand, BandwidthPartInfo::UMi_StreetCanyon);

  bandConfFdd.m_numBwp = 2;

  OperationBandInfo bandFdd = ccBwpCreator.CreateOperationBandContiguousCc (bandConfFdd);
  // By using the configuration created, it is time to make the operation bands

  Config::SetDefault ("ns3::ThreeGppChannelModel::UpdatePeriod",TimeValue (MilliSeconds (0)));
  nrHelper->SetChannelConditionModelAttribute ("UpdatePeriod", TimeValue (MilliSeconds (0)));
  nrHelper->SetPathlossAttribute ("ShadowingEnabled", BooleanValue (false));

  nrHelper->InitializeOperationBand (&bandFdd);
  allBwps = CcBwpCreator::GetAllBwps ({bandFdd});

  idealBeamformingHelper->SetAttribute ("BeamformingMethod", TypeIdValue (DirectPathBeamforming::GetTypeId ()));

  epcHelper->SetAttribute ("S1uLinkDelay", TimeValue (MilliSeconds (0)));

  // Antennas for all the UEs
  nrHelper->SetUeAntennaAttribute ("NumRows", UintegerValue (2));
  nrHelper->SetUeAntennaAttribute ("NumColumns", UintegerValue (4));
  nrHelper->SetUeAntennaAttribute ("AntennaElement", PointerValue (CreateObject<IsotropicAntennaModel> ()));

  // Antennas for all the gNbs
  nrHelper->SetGnbAntennaAttribute ("NumRows", UintegerValue (4));
  nrHelper->SetGnbAntennaAttribute ("NumColumns", UintegerValue (8));
  nrHelper->SetGnbAntennaAttribute ("AntennaElement", PointerValue (CreateObject<IsotropicAntennaModel> ()));

  nrHelper->SetGnbPhyAttribute ("TxPower", DoubleValue (4.0));

  //uint32_t bwpIdForeMBB = 0;

  // nrHelper->SetGnbBwpManagerAlgorithmAttribute ("NGBR_LOW_LAT_EMBB", UintegerValue (bwpIdForeMBB));

  // nrHelper->SetUeBwpManagerAlgorithmAttribute ("NGBR_LOW_LAT_EMBB", UintegerValue (bwpIdForeMBB));

  // ueNetDev = nrHelper->InstallUeDevice(ueNodes, allBwps);
  eMBBNetDev1 = nrHelper->InstallUeDevice(eMBBueNodes1, allBwps);
  eMBBNetDev2 = nrHelper->InstallUeDevice(eMBBueNodes2, allBwps);
  mMTCNetDev1 = nrHelper->InstallUeDevice(mMTCueNodes1, allBwps);
  mMTCNetDev2 = nrHelper->InstallUeDevice(mMTCueNodes2, allBwps);
  URLLCNetDev1 = nrHelper->InstallUeDevice(URLLCueNodes1, allBwps);
  URLLCNetDev2 = nrHelper->InstallUeDevice(URLLCueNodes2, allBwps);
  enbNetDev = nrHelper->InstallGnbDevice(gNbNodes, allBwps);

  randomStream += nrHelper->AssignStreams (enbNetDev, randomStream);
  // randomStream += nrHelper->AssignStreams (ueNetDev, randomStream);
  randomStream += nrHelper->AssignStreams (eMBBNetDev1, randomStream);
  randomStream += nrHelper->AssignStreams (mMTCNetDev1, randomStream);
  randomStream += nrHelper->AssignStreams (URLLCNetDev1, randomStream);

  randomStream += nrHelper->AssignStreams (eMBBNetDev2, randomStream);
  randomStream += nrHelper->AssignStreams (mMTCNetDev2, randomStream);
  randomStream += nrHelper->AssignStreams (URLLCNetDev2, randomStream);
  
  for(uint16_t i = 0; i <gNbNum ; i++){
	  nrHelper->GetGnbPhy (enbNetDev.Get (i), 0)->SetAttribute ("Numerology", UintegerValue (2));
  	nrHelper->GetGnbPhy (enbNetDev.Get (i), 0)->SetAttribute ("Pattern", StringValue ("F|F|F|F|F|F|F|F|F|F|"));
  	nrHelper->GetGnbPhy (enbNetDev.Get (i), 0)->SetAttribute ("TxPower", DoubleValue (4.0));

  }

  for (auto it = enbNetDev.Begin (); it != enbNetDev.End (); ++it)
    {
      DynamicCast<NrGnbNetDevice> (*it)->UpdateConfig ();
    }
  
  for (auto it = eMBBNetDev1.Begin (); it != eMBBNetDev1.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }
  
  for (auto it = mMTCNetDev1.Begin (); it != mMTCNetDev1.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }
  
  for (auto it = URLLCNetDev1.Begin (); it != URLLCNetDev1.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }

  for (auto it = eMBBNetDev2.Begin (); it != eMBBNetDev2.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }
  
  for (auto it = mMTCNetDev2.Begin (); it != mMTCNetDev2.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }
  
  for (auto it = URLLCNetDev2.Begin (); it != URLLCNetDev2.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }

  Ptr<Node> pgw = epcHelper->GetPgwNode ();
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (2500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.000)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);
  // internet.Install(ueNodes);
  internet.Install(eMBBueNodes1);
  internet.Install(mMTCueNodes1);
  internet.Install(URLLCueNodes1);

  internet.Install(eMBBueNodes2);
  internet.Install(mMTCueNodes2);
  internet.Install(URLLCueNodes2);

  // Ipv4InterfaceContainer ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueNetDev));
  Ipv4InterfaceContainer ueIpIface1 = epcHelper->AssignUeIpv4Address (NetDeviceContainer (eMBBNetDev1));
  Ipv4InterfaceContainer ueIpIface2 = epcHelper->AssignUeIpv4Address (NetDeviceContainer (mMTCNetDev1));
  Ipv4InterfaceContainer ueIpIface3 = epcHelper->AssignUeIpv4Address (NetDeviceContainer (URLLCNetDev1));

  Ipv4InterfaceContainer ueIpIface4 = epcHelper->AssignUeIpv4Address (NetDeviceContainer (eMBBNetDev2));
  Ipv4InterfaceContainer ueIpIface5 = epcHelper->AssignUeIpv4Address (NetDeviceContainer (mMTCNetDev2));
  Ipv4InterfaceContainer ueIpIface6 = epcHelper->AssignUeIpv4Address (NetDeviceContainer (URLLCNetDev2));
  
  // for (uint32_t j = 0; j < ueNodes.GetN (); ++j)
  //   {
  //     Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNodes.Get (j)->GetObject<Ipv4> ());
  //     ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
  //   }

  for (uint32_t j = 0; j < eMBBueNodes1.GetN (); ++j)
    {
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (eMBBueNodes1.Get (j)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }
  for (uint32_t j = 0; j < mMTCueNodes1.GetN (); ++j)
    {
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (mMTCueNodes1.Get (j)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }
  
  for (uint32_t j = 0; j < URLLCueNodes1.GetN (); ++j)
    {
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (URLLCueNodes1.Get (j)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  for (uint32_t j = 0; j < eMBBueNodes2.GetN (); ++j)
    {
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (eMBBueNodes2.Get (j)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }
  for (uint32_t j = 0; j < mMTCueNodes2.GetN (); ++j)
    {
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (mMTCueNodes2.Get (j)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }
  
  for (uint32_t j = 0; j < URLLCueNodes2.GetN (); ++j)
    {
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (URLLCueNodes2.Get (j)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  /*for (uint32_t i = 0; i < ueNetDev.GetN(); ++i){
	auto enbDev = DynamicCast<NrGnbNetDevice>(enbNetDev.Get(i));
	auto ueDev = DynamicCast<NrUeNetDevice>(ueNetDev.Get(i));
	NS_ASSERT(enbDev != nullptr);
	NS_ASSERT(ueDev != nullptr);
	nrHelper->AttachToEnb(ueDev, enbDev);
	}*/
  // nrHelper->AttachToClosestEnb(ueNetDev, enbNetDev);
  nrHelper->AttachToClosestEnb(eMBBNetDev1, enbNetDev);
  nrHelper->AttachToClosestEnb(mMTCNetDev1, enbNetDev);
  nrHelper->AttachToClosestEnb(URLLCNetDev1, enbNetDev);

  nrHelper->AttachToClosestEnb(eMBBNetDev2, enbNetDev);
  nrHelper->AttachToClosestEnb(mMTCNetDev2, enbNetDev);
  nrHelper->AttachToClosestEnb(URLLCNetDev2, enbNetDev);


  /*
   * Traffic part. Install two kind of traffic: low-latency and voice, each
   * identified by a particular source port.
   */
  
  ApplicationContainer serverApps;
  // The sink will always listen to the specified ports
  UdpServerHelper PacketSinkeMBB1 (PorteMBB1);
  UdpServerHelper PacketSinkmMTC1 (PortmMTC1);
  UdpServerHelper PacketSinkURLLC1 (PortURLLC1);

  UdpServerHelper PacketSinkeMBB2 (PorteMBB2);
  UdpServerHelper PacketSinkmMTC2 (PortmMTC2);
  UdpServerHelper PacketSinkURLLC2 (PortURLLC2);

  // The server, that is the application which is listening, is installed in the UE
  // for the DL traffic, and in the remote host for the UL traffic
  // serverApps.Add (PacketSinkeMBB.Install (gridScenario.GetUserTerminals ()));
  // serverApps.Add (PacketSinkeMBB.Install (ueNodes));
  // serverApps.Add (PacketSinkeMBB.Install (remoteHost));
  serverApps.Add (PacketSinkeMBB1.Install (eMBBueNodes1));
  serverApps.Add (PacketSinkeMBB1.Install (remoteHost));
  serverApps.Add (PacketSinkmMTC1.Install (mMTCueNodes1));
  serverApps.Add (PacketSinkmMTC1.Install (remoteHost));
  serverApps.Add (PacketSinkURLLC1.Install (URLLCueNodes1));
  serverApps.Add (PacketSinkURLLC1.Install (remoteHost));

  serverApps.Add (PacketSinkeMBB2.Install (eMBBueNodes2));
  serverApps.Add (PacketSinkeMBB2.Install (remoteHost));
  serverApps.Add (PacketSinkmMTC2.Install (mMTCueNodes2));
  serverApps.Add (PacketSinkmMTC2.Install (remoteHost));
  serverApps.Add (PacketSinkURLLC2.Install (URLLCueNodes2));
  serverApps.Add (PacketSinkURLLC2.Install (remoteHost));

  /*
   * Configure attributes for the different generators, using user-provided
   * parameters for generating a CBR traffic
   *
   * Low-Latency configuration and object creation:
   */
  UdpClientHelper dlClienteMBB1;
  dlClienteMBB1.SetAttribute ("RemotePort", UintegerValue (PorteMBB1));
  dlClienteMBB1.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
  dlClienteMBB1.SetAttribute ("PacketSize", UintegerValue (embbPacketSize1));
  // dlClienteMBB.SetAttribute ("DataRate",StringValue ("2Mbps"));
  // dlClientVideo.SetAttribute ("Interval", TimeValue (Seconds (1.0 / lambdaVideo)));

  UdpClientHelper dlClientmMTC1;
  dlClientmMTC1.SetAttribute ("RemotePort", UintegerValue (PortmMTC1));
  dlClientmMTC1.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
  dlClientmMTC1.SetAttribute ("PacketSize", UintegerValue (mmtcPacketSize1));

  UdpClientHelper dlClientURLLC1;
  dlClientURLLC1.SetAttribute ("RemotePort", UintegerValue (PortURLLC1));
  dlClientURLLC1.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
  dlClientURLLC1.SetAttribute ("PacketSize", UintegerValue (urllcPacketSize1));

  UdpClientHelper dlClienteMBB2;
  dlClienteMBB2.SetAttribute ("RemotePort", UintegerValue (PorteMBB2));
  dlClienteMBB2.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
  dlClienteMBB2.SetAttribute ("PacketSize", UintegerValue (embbPacketSize2));
  // dlClienteMBB.SetAttribute ("DataRate",StringValue ("2Mbps"));
  // dlClientVideo.SetAttribute ("Interval", TimeValue (Seconds (1.0 / lambdaVideo)));

  UdpClientHelper dlClientmMTC2;
  dlClientmMTC2.SetAttribute ("RemotePort", UintegerValue (PortmMTC2));
  dlClientmMTC2.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
  dlClientmMTC2.SetAttribute ("PacketSize", UintegerValue (mmtcPacketSize2));

  UdpClientHelper dlClientURLLC2;
  dlClientURLLC2.SetAttribute ("RemotePort", UintegerValue (PortURLLC2));
  dlClientURLLC2.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
  dlClientURLLC2.SetAttribute ("PacketSize", UintegerValue (urllcPacketSize2));
  
  EpsBearer eMBBBearer (EpsBearer::NGBR_LOW_LAT_EMBB);
  // std::cout << eMBBBearer.GetPacketErrorLossRate() << std::endl;
  EpsBearer mMTCBearer1 (EpsBearer::NGBR_V2X);
  EpsBearer mMTCBearer2 (EpsBearer::DGBR_DISCRETE_AUT_SMALL);
  EpsBearer URLLCBearer1 (EpsBearer::DGBR_ELECTRICITY);
  EpsBearer URLLCBearer2 (EpsBearer::NGBR_MC_DATA);

  Ptr<EpcTft> eMBBTft1 = Create<EpcTft> ();
  EpcTft::PacketFilter pfeMBB1;
  pfeMBB1.localPortStart = PorteMBB1;
  pfeMBB1.localPortEnd = PorteMBB1;
  eMBBTft1->Add (pfeMBB1);

  Ptr<EpcTft> mMTCTft1 = Create<EpcTft> ();
  EpcTft::PacketFilter pfmMTC1;
  pfmMTC1.localPortStart = PortmMTC1;
  pfmMTC1.localPortEnd = PortmMTC1;
  mMTCTft1->Add (pfmMTC1);

  Ptr<EpcTft> URLLCTft1 = Create<EpcTft> ();
  EpcTft::PacketFilter pfURLLC1;
  pfURLLC1.localPortStart = PortURLLC1;
  pfURLLC1.localPortEnd = PortURLLC1;
  URLLCTft1->Add (pfURLLC1);

  Ptr<EpcTft> eMBBTft2 = Create<EpcTft> ();
  EpcTft::PacketFilter pfeMBB2;
  pfeMBB2.localPortStart = PorteMBB2;
  pfeMBB2.localPortEnd = PorteMBB2;
  eMBBTft2->Add (pfeMBB2);

  Ptr<EpcTft> mMTCTft2 = Create<EpcTft> ();
  EpcTft::PacketFilter pfmMTC2;
  pfmMTC2.localPortStart = PortmMTC2;
  pfmMTC2.localPortEnd = PortmMTC2;
  mMTCTft2->Add (pfmMTC2);

  Ptr<EpcTft> URLLCTft2 = Create<EpcTft> ();
  EpcTft::PacketFilter pfURLLC2;
  pfURLLC2.localPortStart = PortURLLC2;
  pfURLLC2.localPortEnd = PortURLLC2;
  URLLCTft2->Add (pfURLLC2);

  /*
   * Let's install the applications!
   */
  ApplicationContainer clientApps;

  for (uint32_t i = 0; i < eMBBueNodes1.GetN (); ++i)
    {
      Ptr<Node> ue = eMBBueNodes1.Get (i);
      Ptr<NetDevice> ueDevice = eMBBNetDev1.Get (i);
      Address ueAddress = ueIpIface1.GetAddress (i);

      // The client, who is transmitting, is installed in the remote host,
      // with destination address set to the address of the UE
      dlClienteMBB1.SetAttribute ("RemoteAddress", AddressValue (ueAddress));
      clientApps.Add (dlClienteMBB1.Install (remoteHost));
      nrHelper->ActivateDedicatedEpsBearer(ueDevice, eMBBBearer, eMBBTft1);
    }
  
  for (uint32_t i = 0; i < mMTCueNodes1.GetN (); ++i)
    {
      Ptr<Node> ue = mMTCueNodes1.Get (i);
      Ptr<NetDevice> ueDevice = mMTCNetDev1.Get (i);
      Address ueAddress = ueIpIface2.GetAddress (i);

      // The client, who is transmitting, is installed in the remote host,
      // with destination address set to the address of the UE
      dlClientmMTC1.SetAttribute ("RemoteAddress", AddressValue (ueAddress));
      clientApps.Add (dlClientmMTC1.Install (remoteHost));
      nrHelper->ActivateDedicatedEpsBearer(ueDevice, mMTCBearer1, mMTCTft1);
    }
  
  for (uint32_t i = 0; i < URLLCueNodes1.GetN (); ++i)
    {
      Ptr<Node> ue = URLLCueNodes1.Get (i);
      Ptr<NetDevice> ueDevice = URLLCNetDev1.Get (i);
      Address ueAddress = ueIpIface3.GetAddress (i);

      // The client, who is transmitting, is installed in the remote host,
      // with destination address set to the address of the UE
      dlClientURLLC1.SetAttribute ("RemoteAddress", AddressValue (ueAddress));
      clientApps.Add (dlClientURLLC1.Install (remoteHost));
      nrHelper->ActivateDedicatedEpsBearer(ueDevice, URLLCBearer1, URLLCTft1);
    }
  
  for (uint32_t i = 0; i < eMBBueNodes2.GetN (); ++i)
    {
      Ptr<Node> ue = eMBBueNodes2.Get (i);
      Ptr<NetDevice> ueDevice = eMBBNetDev2.Get (i);
      Address ueAddress = ueIpIface4.GetAddress (i);

      // The client, who is transmitting, is installed in the remote host,
      // with destination address set to the address of the UE
      dlClienteMBB2.SetAttribute ("RemoteAddress", AddressValue (ueAddress));
      clientApps.Add (dlClienteMBB2.Install (remoteHost));
      nrHelper->ActivateDedicatedEpsBearer(ueDevice, eMBBBearer, eMBBTft2);
    }
  
  for (uint32_t i = 0; i < mMTCueNodes2.GetN (); ++i)
    {
      Ptr<Node> ue = mMTCueNodes2.Get (i);
      Ptr<NetDevice> ueDevice = mMTCNetDev2.Get (i);
      Address ueAddress = ueIpIface5.GetAddress (i);

      // The client, who is transmitting, is installed in the remote host,
      // with destination address set to the address of the UE
      dlClientmMTC2.SetAttribute ("RemoteAddress", AddressValue (ueAddress));
      clientApps.Add (dlClientmMTC2.Install (remoteHost));
      nrHelper->ActivateDedicatedEpsBearer(ueDevice, mMTCBearer2, mMTCTft2);
    }
  
  for (uint32_t i = 0; i < URLLCueNodes2.GetN (); ++i)
    {
      Ptr<Node> ue = URLLCueNodes2.Get (i);
      Ptr<NetDevice> ueDevice = URLLCNetDev2.Get (i);
      Address ueAddress = ueIpIface6.GetAddress (i);

      // The client, who is transmitting, is installed in the remote host,
      // with destination address set to the address of the UE
      dlClientURLLC2.SetAttribute ("RemoteAddress", AddressValue (ueAddress));
      clientApps.Add (dlClientURLLC2.Install (remoteHost));
      nrHelper->ActivateDedicatedEpsBearer(ueDevice, URLLCBearer2, URLLCTft2);
    }


  /*for (uint32_t i = 0; i < gridScenario.GetUserTerminals ().GetN (); ++i)
    {
      Ptr<Node> ue = gridScenario.GetUserTerminals ().Get (i);
      Ptr<NetDevice> ueDevice = ueNetDev.Get (i);
      Address ueAddress = ueIpIface.GetAddress (i);

      // The client, who is transmitting, is installed in the remote host,
      // with destination address set to the address of the UE
      // dlClienteMBB.SetAttribute ("RemoteAddress", AddressValue (ueAddress));
      // clientApps.Add (dlClienteMBB.Install (remoteHost));

      // nrHelper->ActivateDedicatedEpsBearer(ueDevice, eMBBBearer, eMBBTft);

      OnOffHelper onOffHelper ("ns3::TcpSocketFactory", ueAddress);
      onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
      onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
      onOffHelper.SetAttribute ("DataRate",StringValue ("2Mbps"));
      onOffHelper.SetAttribute ("PacketSize",UintegerValue(1280));
      onOffHelper.SetAttribute ("Remote", AddressValue(InetSocketAddress (Ipv4Address ("7.0.0.0"), PorteMBB)));

      ApplicationContainer app = onOffHelper.Install (ue);
      app.Start (MilliSeconds (StartTimeMs));
      app.Stop (MilliSeconds (simTimeMs));

      nrHelper->ActivateDedicatedEpsBearer(ueDevice, eMBBBearer, eMBBTft);
    }*/
  
  /*for (uint32_t i = 0; i < gridScenario.GetUserTerminals ().GetN (); ++i)
    {
      Ptr<Node> ue = gridScenario.GetUserTerminals ().Get (i);
      Ptr<NetDevice> ueDevice = ueNetDev.Get (i);
      Address ueAddress = ueIpIface.GetAddress (i);

      // The client, who is transmitting, is installed in the remote host,
      // with destination address set to the address of the UE
      // dlClienteMBB.SetAttribute ("RemoteAddress", AddressValue (ueAddress));
      // clientApps.Add (dlClienteMBB.Install (remoteHost));

      // nrHelper->ActivateDedicatedEpsBearer(ueDevice, eMBBBearer, eMBBTft);
      PacketSinkHelper sink ("ns3::TcpSocketFactory", ueAddress);

      ApplicationContainer app = sink.Install (ue);
      app.Start (MilliSeconds (StartTimeMs));
      app.Stop (MilliSeconds (simTimeMs));

      nrHelper->ActivateDedicatedEpsBearer(ueDevice, eMBBBearer, eMBBTft);
    }*/

  // start UDP server and client apps
  serverApps.Start (MilliSeconds (StartTimeMs));
  clientApps.Start (MilliSeconds (StartTimeMs));
  serverApps.Stop (MilliSeconds (simTimeMs));
  clientApps.Stop (MilliSeconds (simTimeMs));

  // enable the traces provided by the nr module
  nrHelper->EnableTraces();

  //FlowMonitorHelper flowmonHelper;
  //NodeContainer endpointNodes;
  endpointNodes.Add (remoteHost);
  endpointNodes.Add (eMBBueNodes1);
  endpointNodes.Add (mMTCueNodes1);
  endpointNodes.Add (URLLCueNodes1);

  endpointNodes.Add (eMBBueNodes2);
  endpointNodes.Add (mMTCueNodes2);
  endpointNodes.Add (URLLCueNodes2);

  //Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install (endpointNodes);
  monitor = flowmonHelper.Install (endpointNodes);
  monitor->SetAttribute ("DelayBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("JitterBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("PacketSizeBinWidth", DoubleValue (20));

  //Simulator::Schedule (Seconds(0.0), &ScheduleNextStateRead, MilliSeconds(envStepTime), openGymInterface);
  
  // AnimationInterface anim("ns.xml");
  // for (int i = 0; i < 5; i++)
  // {
  //   std::cout << i << "----------------------------------------------------------" << std::endl;
  //   Simulator::Schedule (MilliSeconds(StartTimeMs), &save_data);
  //   Simulator::Schedule (MilliSeconds(simTimeMs), &flow_monitor);

  //   Simulator::Stop (MilliSeconds (simTimeMs));
  //   Simulator::Run ();
  // }
  
  Simulator::Schedule (MilliSeconds(StartTimeMs), &save_data);
  Simulator::Schedule (MilliSeconds(simTimeMs), &flow_monitor);

  Simulator::Stop (MilliSeconds (simTimeMs));

  AnimationInterface anim("ns.xml");
  Simulator::Run ();
  /*
   * To check what was installed in the memory, i.e., BWPs of eNb Device, and its configuration.
   * Example is: Node 1 -> Device 0 -> BandwidthPartMap -> {0,1} BWPs -> NrGnbPhy -> Numerology,
  GtkConfigStore config;
  config.ConfigureAttributes ();
  */

  // uint32_t n_num = NodeList::GetNNodes ();
  // std::cout << "node num:" << n_num << std::endl;
  /*for( uint32_t i = 0 ; i < n_num ; i++){
    Ptr<Node> dev = NodeList::GetNode(i);
    for (uint32_t i = 0; i < dev->GetNDevices(); i++)
    {
      cout << dev.GetDevice(i)->
    }
    
    //Ptr<NetDevice> netdev = dev->GetDevice(0); 
    std::cout << i << " " << dev->GetNDevices() << std::endl;
  }*/

  // for (auto it = enbNetDev.Begin (); it != enbNetDev.End (); ++it)
  //   {
  //     //DynamicCast<NrGnbNetDevice> (*it)->UpdateConfig ();
  //     uint32_t n = DynamicCast<NrGnbNetDevice>(*it)->GetCellId();
  //     Ptr <Node> node = DynamicCast<NrGnbNetDevice>(*it)->GetNode();
  //     std::cout << "id of node " <<  node->GetId() << std::endl;
  //     std::cout << n << std::endl;
  //   }
  /*for (auto it = ueNetDev.Begin (); it != ueNetDev.End (); ++it)
    {
      //DynamicCast<NrGnbNetDevice> (*it)->UpdateConfig ();
      std::map<uint8_t,Ptr<BandwidthPartUe>> part = DynamicCast<NrUeNetDevice>(*it)->GetCcMap();
      Ptr <const NrGnbNetDevice> target = DynamicCast<NrUeNetDevice>(*it)->GetTargetEnb();
      //Ptr <Node> node = DynamicCast<NrUeNetDevice>(*it)->GetNode();
      std::cout <<  part[0] << " " << part[1] << std::endl;
      // Ptr <BandwidthPartUe> uemap = part[1];
      std::cout << "Cell id of enb " <<  target->GetCellId() << std::endl;
      //std::cout << "id of node " <<  node->GetId() << std::endl;
    }*/

    //openGymInterface->NotifySimulationEnd();
    Simulator::Destroy ();
    myfile.close();
    embbfile.close();
    urllcfile.close();
    mmtcfile.close();
    rewardfile.close();

    time_t end = time(NULL);
    // tm *ftm = localtime(&finish);

    double diff = difftime(end, start);
    std::cout << "The simulation time is: " << diff << std::endl;

  return 0;


}
