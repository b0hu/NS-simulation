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
// #include "ns3/opengym-module.h"
#include "ns3/node-list.h"
#include "ns3/netanim-module.h"

using namespace ns3;

// int64_t randomStream = 1;

// uint32_t gNbNum = 4;
// uint32_t ueNum = 8;
// double gNbHeight = 10.0;
// double ueHeight = 1.5;

// uint32_t embbPacketSize = 1000;
// uint32_t simTimeMs = 1800;
// uint32_t envStepTime = 200;
// uint32_t eMBBStartTimeMs = 400;
// double centralFrequencyBand = 28e9;
// double bandwidthBand1 = 100e6;
// //double totalTxPower = 4;
// uint16_t PorteMBB = 1234;
// uint32_t simSeed = 1;
// uint32_t testArg = 0;

// double averageFlowThroughput = 0.0;
// double averageFlowDelay = 0.0;
//
uint32_t rbg = 0;
uint8_t sym = 0;

//struct PointInFTPlane temp = new PointInFTPlane(rbg, sym);


CustomScheduler* temp;
NrMacSchedulerNs3 * temp2;
NrMacSchedulerUeInfo * u;
//temp->PointInFTPlane(rbg, sym);
//CustomScheduler:: FTResources ft = {rbg, sym};
NrMacSchedulerNs3::PointInFTPlane * ft = new NrMacSchedulerNs3::PointInFTPlane(rbg, sym);

NS_LOG_COMPONENT_DEFINE ("eMBB NS Simulation Test");

int 
main (int argc, char *argv[])
{
  NS_LOG_UNCOND ("eMBB NS Test");

  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (999999999));

std::cout << temp->GetTypeId() << std::endl;


  CommandLine cmd;

  cmd.AddValue ("PorteMBB", "Port number for OpenGym env. Default: 5555", PorteMBB);
  cmd.AddValue ("simSeed", "Seed for random generator. Default: 1", simSeed);
  // optional parameters
  cmd.AddValue ("simTime", "Simulation time in seconds. Default: 10s", simTimeMs);
  cmd.AddValue ("testArg", "Extra simulation argument. Default: 0", testArg);
  cmd.Parse (argc, argv);

  NS_LOG_UNCOND ("simTimeMs: " << simTimeMs);
  NS_LOG_UNCOND ("envStepTime" << envStepTime);

  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (simSeed);

 /* Ptr<OpenGymInterface> openGymInterface = CreateObject<OpenGymInterface> (1234);
  Ptr<MyGym> myGymEnv = CreateObject<MyGym> ();
  myGymEnv->SetOpenGymInterface(openGymInterface);*/

  MobilityHelper enbmobility;
  enbmobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  Ptr<ListPositionAllocator> bsPositionAlloc = CreateObject<ListPositionAllocator>();
  //enbmobility.SetPositionAllocator("ns3::GridPositionAllocator","MinX",DoubleValue(0.0),"MinY",DoubleValue(0.0),"GridWidth",UintegerValue(30),"LayoutType",StringValue("RowFirst"));
  
  for (uint32_t i = 0; i < gNbNum; i++)
  {
    uint32_t x = i%2;
    uint32_t y = i/2;
    bsPositionAlloc->Add(Vector(x * 30 + 30, y * 30 + 30, gNbHeight));
  }
  enbmobility.SetPositionAllocator(bsPositionAlloc);
  gNbNodes.Create(gNbNum);
  enbmobility.Install(gNbNodes);

  MobilityHelper uemobility;
  Ptr<ListPositionAllocator> uePositionAlloc = CreateObject<ListPositionAllocator>();
  
  uemobility.SetPositionAllocator ("ns3::RandomBoxPositionAllocator",
                                 "X", StringValue ("ns3::UniformRandomVariable[Min=0|Max=60]"),
                                 "Y", StringValue ("ns3::UniformRandomVariable[Min=0|Max=60]"),
                                 "Z", StringValue ("ns3::UniformRandomVariable[Min=0|Max=2]"));
  uemobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel", "Bounds",RectangleValue(Rectangle(0.0, 60.0, 0.0, 60.0)));
  // ueNodes.Create(ueNum);
  // uemobility.Install(ueNodes);

  eMBBueNodes.Create(ueNum);
  uemobility.Install(eMBBueNodes);
  mMTCueNodes.Create(ueNum);
  uemobility.Install(mMTCueNodes);
  URLLCueNodes.Create(ueNum);
  uemobility.Install(URLLCueNodes);


  Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper> ();
  Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  //nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::NrMacSchedulerOfdmaRR"));
  //nrHelper->SetSchedulerTypeId(temp2->GetTypeId());

  nrHelper->SetBeamformingHelper (idealBeamformingHelper);
  nrHelper->SetEpcHelper (epcHelper);

  BandwidthPartInfoPtrVector allBwps;
  CcBwpCreator ccBwpCreator;
  const uint8_t numCcPerBand = 1;

  CcBwpCreator::SimpleOperationBandConf bandConfTdd (centralFrequencyBand, bandwidthBand1, numCcPerBand, BandwidthPartInfo::UMi_StreetCanyon);

  bandConfTdd.m_numBwp = 2;

  OperationBandInfo bandTdd = ccBwpCreator.CreateOperationBandContiguousCc (bandConfTdd);
  // By using the configuration created, it is time to make the operation bands

  Config::SetDefault ("ns3::ThreeGppChannelModel::UpdatePeriod",TimeValue (MilliSeconds (0)));
  nrHelper->SetChannelConditionModelAttribute ("UpdatePeriod", TimeValue (MilliSeconds (0)));
  nrHelper->SetPathlossAttribute ("ShadowingEnabled", BooleanValue (false));
  //nrHelper->SetSchedulerattribute();

  nrHelper->InitializeOperationBand (&bandTdd);
  allBwps = CcBwpCreator::GetAllBwps ({bandTdd});

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

  uint32_t bwpIdForeMBB = 0;

  // nrHelper->SetGnbBwpManagerAlgorithmAttribute ("NGBR_LOW_LAT_EMBB", UintegerValue (bwpIdForeMBB));

  // nrHelper->SetUeBwpManagerAlgorithmAttribute ("NGBR_LOW_LAT_EMBB", UintegerValue (bwpIdForeMBB));

  enbNetDev = nrHelper->InstallGnbDevice(gNbNodes, allBwps);
  // ueNetDev = nrHelper->InstallUeDevice(ueNodes, allBwps);
  eMBBNetDev = nrHelper->InstallUeDevice(eMBBueNodes, allBwps);
  mMTCNetDev = nrHelper->InstallUeDevice(mMTCueNodes, allBwps);
  URLLCNetDev = nrHelper->InstallUeDevice(URLLCueNodes, allBwps);

  randomStream += nrHelper->AssignStreams (enbNetDev, randomStream);
  // randomStream += nrHelper->AssignStreams (ueNetDev, randomStream);
  randomStream += nrHelper->AssignStreams (eMBBNetDev, randomStream);
  randomStream += nrHelper->AssignStreams (mMTCNetDev, randomStream);
  randomStream += nrHelper->AssignStreams (URLLCNetDev, randomStream);
  
  for(uint16_t i = 0; i <gNbNum ; i++){
	  nrHelper->GetGnbPhy (enbNetDev.Get (i), 0)->SetAttribute ("Numerology", UintegerValue (0));
  	//nrHelper->GetGnbPhy (enbNetDev.Get (i), 0)->SetAttribute ("Pattern", StringValue ("F|F|F|F|F|F|F|F|F|F|"));
  	nrHelper->GetGnbPhy (enbNetDev.Get (i), 0)->SetAttribute ("TxPower", DoubleValue (4.0));

  }

  for (auto it = enbNetDev.Begin (); it != enbNetDev.End (); ++it)
    {
      DynamicCast<NrGnbNetDevice> (*it)->UpdateConfig ();
    }

  // for (auto it = ueNetDev.Begin (); it != ueNetDev.End (); ++it)
  //   {
  //     DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
  //   }
  
  for (auto it = eMBBNetDev.Begin (); it != eMBBNetDev.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }
  
  for (auto it = mMTCNetDev.Begin (); it != mMTCNetDev.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }
  
  for (auto it = URLLCNetDev.Begin (); it != URLLCNetDev.End (); ++it)
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
  internet.Install(eMBBueNodes);
  internet.Install(mMTCueNodes);
  internet.Install(URLLCueNodes);
  

  // Ipv4InterfaceContainer ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueNetDev));
  Ipv4InterfaceContainer ueIpIface1 = epcHelper->AssignUeIpv4Address (NetDeviceContainer (eMBBNetDev));
  Ipv4InterfaceContainer ueIpIface2 = epcHelper->AssignUeIpv4Address (NetDeviceContainer (mMTCNetDev));
  Ipv4InterfaceContainer ueIpIface3 = epcHelper->AssignUeIpv4Address (NetDeviceContainer (URLLCNetDev));
  
  // for (uint32_t j = 0; j < ueNodes.GetN (); ++j)
  //   {
  //     Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNodes.Get (j)->GetObject<Ipv4> ());
  //     ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
  //   }

  for (uint32_t j = 0; j < eMBBueNodes.GetN (); ++j)
    {
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (eMBBueNodes.Get (j)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }
  for (uint32_t j = 0; j < mMTCueNodes.GetN (); ++j)
    {
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (mMTCueNodes.Get (j)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }
  
  for (uint32_t j = 0; j < URLLCueNodes.GetN (); ++j)
    {
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (URLLCueNodes.Get (j)->GetObject<Ipv4> ());
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
  nrHelper->AttachToClosestEnb(eMBBNetDev, enbNetDev);
  nrHelper->AttachToClosestEnb(mMTCNetDev, enbNetDev);
  nrHelper->AttachToClosestEnb(URLLCNetDev, enbNetDev);

  /*
   * Traffic part. Install two kind of traffic: low-latency and voice, each
   * identified by a particular source port.
   */

  
  ApplicationContainer serverApps;
  // The sink will always listen to the specified ports
  UdpServerHelper PacketSinkeMBB (PorteMBB);
  UdpServerHelper PacketSinkmMTC (PortmMTC);
  UdpServerHelper PacketSinkURLLC (PortURLLC);

  // The server, that is the application which is listening, is installed in the UE
  // for the DL traffic, and in the remote host for the UL traffic
  // serverApps.Add (PacketSinkeMBB.Install (gridScenario.GetUserTerminals ()));
  // serverApps.Add (PacketSinkeMBB.Install (ueNodes));
  // serverApps.Add (PacketSinkeMBB.Install (remoteHost));
  serverApps.Add (PacketSinkeMBB.Install (eMBBueNodes));
  serverApps.Add (PacketSinkeMBB.Install (remoteHost));
  serverApps.Add (PacketSinkmMTC.Install (mMTCueNodes));
  serverApps.Add (PacketSinkmMTC.Install (remoteHost));
  serverApps.Add (PacketSinkURLLC.Install (URLLCueNodes));
  serverApps.Add (PacketSinkURLLC.Install (remoteHost));


  /*
   * Configure attributes for the different generators, using user-provided
   * parameters for generating a CBR traffic
   *
   * Low-Latency configuration and object creation:
   */
  UdpClientHelper dlClienteMBB;
  dlClienteMBB.SetAttribute ("RemotePort", UintegerValue (PorteMBB));
  dlClienteMBB.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
  dlClienteMBB.SetAttribute ("PacketSize", UintegerValue (embbPacketSize));
  // dlClienteMBB.SetAttribute ("DataRate",StringValue ("2Mbps"));
  // dlClientVideo.SetAttribute ("Interval", TimeValue (Seconds (1.0 / lambdaVideo)));

  UdpClientHelper dlClientmMTC;
  dlClientmMTC.SetAttribute ("RemotePort", UintegerValue (PortmMTC));
  dlClientmMTC.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
  dlClientmMTC.SetAttribute ("PacketSize", UintegerValue (mmtcPacketSize));

  UdpClientHelper dlClientURLLC;
  dlClientURLLC.SetAttribute ("RemotePort", UintegerValue (PortURLLC));
  dlClientURLLC.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
  dlClientURLLC.SetAttribute ("PacketSize", UintegerValue (urllcPacketSize));
  
  EpsBearer eMBBBearer (EpsBearer::NGBR_LOW_LAT_EMBB);
  EpsBearer mMTCBearer (EpsBearer::NGBR_V2X);
  EpsBearer URLLCBearer (EpsBearer::DGBR_ELECTRICITY );

  Ptr<EpcTft> eMBBTft = Create<EpcTft> ();
  EpcTft::PacketFilter pfeMBB;
  pfeMBB.localPortStart = PorteMBB;
  pfeMBB.localPortEnd = PorteMBB;
  eMBBTft->Add (pfeMBB);

  Ptr<EpcTft> mMTCTft = Create<EpcTft> ();
  EpcTft::PacketFilter pfmMTC;
  pfmMTC.localPortStart = PortmMTC;
  pfmMTC.localPortEnd = PortmMTC;
  mMTCTft->Add (pfmMTC);

  Ptr<EpcTft> URLLCTft = Create<EpcTft> ();
  EpcTft::PacketFilter pfURLLC;
  pfURLLC.localPortStart = PortURLLC;
  pfURLLC.localPortEnd = PortURLLC;
  URLLCTft->Add (pfURLLC);

  /*
   * Let's install the applications!
   */
  ApplicationContainer clientApps;

  // for (uint32_t i = 0; i < ueNodes.GetN (); ++i)
  //   {
  //     Ptr<Node> ue = ueNodes.Get (i);
  //     Ptr<NetDevice> ueDevice = ueNetDev.Get (i);
  //     Address ueAddress = ueIpIface.GetAddress (i);

  //     // The client, who is transmitting, is installed in the remote host,
  //     // with destination address set to the address of the UE
  //     dlClienteMBB.SetAttribute ("RemoteAddress", AddressValue (ueAddress));
  //     clientApps.Add (dlClienteMBB.Install (remoteHost));
  //     nrHelper->ActivateDedicatedEpsBearer(ueDevice, eMBBBearer, eMBBTft);
  //   }
  for (uint32_t i = 0; i < eMBBueNodes.GetN (); ++i)
    {
      Ptr<Node> ue = eMBBueNodes.Get (i);
      Ptr<NetDevice> ueDevice = eMBBNetDev.Get (i);
      Address ueAddress = ueIpIface1.GetAddress (i);

      // The client, who is transmitting, is installed in the remote host,
      // with destination address set to the address of the UE
      dlClienteMBB.SetAttribute ("RemoteAddress", AddressValue (ueAddress));
      clientApps.Add (dlClienteMBB.Install (remoteHost));
      nrHelper->ActivateDedicatedEpsBearer(ueDevice, eMBBBearer, eMBBTft);
    }
  
  for (uint32_t i = 0; i < mMTCueNodes.GetN (); ++i)
    {
      Ptr<Node> ue = mMTCueNodes.Get (i);
      Ptr<NetDevice> ueDevice = mMTCNetDev.Get (i);
      Address ueAddress = ueIpIface2.GetAddress (i);

      // The client, who is transmitting, is installed in the remote host,
      // with destination address set to the address of the UE
      dlClientmMTC.SetAttribute ("RemoteAddress", AddressValue (ueAddress));
      clientApps.Add (dlClientmMTC.Install (remoteHost));
      nrHelper->ActivateDedicatedEpsBearer(ueDevice, mMTCBearer, mMTCTft);
    }
  
  for (uint32_t i = 0; i < URLLCueNodes.GetN (); ++i)
    {
      Ptr<Node> ue = URLLCueNodes.Get (i);
      Ptr<NetDevice> ueDevice = URLLCNetDev.Get (i);
      Address ueAddress = ueIpIface3.GetAddress (i);

      // The client, who is transmitting, is installed in the remote host,
      // with destination address set to the address of the UE
      dlClientURLLC.SetAttribute ("RemoteAddress", AddressValue (ueAddress));
      clientApps.Add (dlClientURLLC.Install (remoteHost));
      nrHelper->ActivateDedicatedEpsBearer(ueDevice, URLLCBearer, URLLCTft);
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
      app.Start (MilliSeconds (eMBBStartTimeMs));
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
      app.Start (MilliSeconds (eMBBStartTimeMs));
      app.Stop (MilliSeconds (simTimeMs));

      nrHelper->ActivateDedicatedEpsBearer(ueDevice, eMBBBearer, eMBBTft);
    }*/

  // start UDP server and client apps
  serverApps.Start (MilliSeconds (eMBBStartTimeMs));
  clientApps.Start (MilliSeconds (eMBBStartTimeMs));
  serverApps.Stop (MilliSeconds (simTimeMs));
  clientApps.Stop (MilliSeconds (simTimeMs));

  // enable the traces provided by the nr module
  nrHelper->EnableTraces();

  //FlowMonitorHelper flowmonHelper;
  //NodeContainer endpointNodes;
  endpointNodes.Add (remoteHost);
  // endpointNodes.Add (ueNodes);
  endpointNodes.Add (eMBBueNodes);
  endpointNodes.Add (mMTCueNodes);
  endpointNodes.Add (URLLCueNodes);


  //Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install (endpointNodes);
  monitor = flowmonHelper.Install (endpointNodes);
  monitor->SetAttribute ("DelayBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("JitterBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("PacketSizeBinWidth", DoubleValue (20));

  //Simulator::Schedule (Seconds(0.0), &ScheduleNextStateRead, envStepTime, openGymInterface);

  Simulator::Stop (MilliSeconds (simTimeMs));

  AnimationInterface anim("ns.xml");
  Simulator::Run ();

  /*
   * To check what was installed in the memory, i.e., BWPs of eNb Device, and its configuration.
   * Example is: Node 1 -> Device 0 -> BandwidthPartMap -> {0,1} BWPs -> NrGnbPhy -> Numerology,
  GtkConfigStore config;
  config.ConfigureAttributes ();
  */

  // Print per-flow statistics
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

  //double averageFlowThroughput = 0.0;
  //double averageFlowDelay = 0.0;

  uint32_t n_num = NodeList::GetNNodes ();
  std::cout << "node num:" << n_num << std::endl;
  /*for( uint32_t i = 0 ; i < n_num ; i++){
    Ptr<Node> dev = NodeList::GetNode(i);
    for (uint32_t i = 0; i < dev->GetNDevices(); i++)
    {
      cout << dev.GetDevice(i)->
    }
    
    //Ptr<NetDevice> netdev = dev->GetDevice(0); 
    std::cout << i << " " << dev->GetNDevices() << std::endl;
  }*/

  for (auto it = enbNetDev.Begin (); it != enbNetDev.End (); ++it)
    {
      //DynamicCast<NrGnbNetDevice> (*it)->UpdateConfig ();
      uint32_t n = DynamicCast<NrGnbNetDevice>(*it)->GetCellId();
      Ptr <Node> node = DynamicCast<NrGnbNetDevice>(*it)->GetNode();
      std::cout << "id of node " <<  node->GetId() << std::endl;
      std::cout << n << std::endl;
    }
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
  
  // std::ofstream outFile;
  // std::string filename = outputDir + "/" + simTag;
  // outFile.open (filename.c_str (), std::ofstream::out | std::ofstream::trunc);

  // if (!outFile.is_open ())
  //   {
  //     std::cerr << "Can't open file " << filename << std::endl;
  //     return 1;
  //   }

  // outFile.setf (std::ios_base::fixed);

  // for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
  //   {
  //     Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
  //     std::stringstream protoStream;
  //     protoStream << (uint16_t) t.protocol;
  //     if (t.protocol == 6)
  //       {
  //         protoStream.str ("TCP");
  //       }
  //     if (t.protocol == 17)
  //       {
  //         protoStream.str ("UDP");
  //       }

  //     outFile << "Flow " << i->first << " (" << t.sourceAddress << ":" << t.sourcePort << " -> " << t.destinationAddress << ":" << t.destinationPort << ") proto " << protoStream.str () << "\n";
  //     outFile << "  Tx Packets: " << i->second.txPackets << "\n";
  //     outFile << "  Tx Bytes:   " << i->second.txBytes << "\n";
  //     outFile << "  TxOffered:  " << i->second.txBytes * 8.0 / ((simTimeMs - eMBBStartTimeMs) / 1000.0) / 1000.0 / 1000.0  << " Mbps\n";
  //     outFile << "  Rx Bytes:   " << i->second.rxBytes << "\n";


  //     if (i->second.rxPackets > 0)
  //       {
  //         // Measure the duration of the flow from receiver's perspective
  //         //double rxDuration = i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ();
  //         double rxDuration = (simTimeMs - eMBBStartTimeMs) / 1000.0;

  //         averageFlowThroughput += i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
  //         averageFlowDelay += 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets;

	//   outFile << "  Throughput: " << i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000  << " Mbps\n";
  //         outFile << "  Mean delay:  " << 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets << " ms\n";
  //         //outFile << "  Mean upt:  " << i->second.uptSum / i->second.rxPackets / 1000/1000 << " Mbps \n";
  //         outFile << "  Mean jitter:  " << 1000 * i->second.jitterSum.GetSeconds () / i->second.rxPackets  << " ms\n";


  //       }
  //     else
  //       {
  //         outFile << "  Throughput:  0 Mbps\n";
  //         outFile << "  Mean delay:  0 ms\n";
  //         outFile << "  Mean jitter: 0 ms\n";
  //       }
  //     outFile << "  Rx Packets: " << i->second.rxPackets << "\n";

  //   } 
  //   outFile << "\n\n  Mean flow throughput: " << averageFlowThroughput / stats.size () << "\n";
  //   outFile << "  Mean flow delay: " << averageFlowDelay / stats.size () << "\n";

  //   outFile.close ();

  //   std::ifstream f (filename.c_str ());

  //   if (f.is_open ())
  //     {
  //       std::cout << f.rdbuf ();
  //     }

    //openGymInterface->NotifySimulationEnd();
    Simulator::Destroy ();

  return 0;


}
