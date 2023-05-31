#include "mygym.h"
#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/node-list.h"
#include "ns3/log.h"
#include "ns3/nr-module.h"
#include "ns3/nr-mac-scheduler-tdma.h"
#include "ns3/nr-mac-scheduler-ns3.h"
#include "ns3/flow-monitor-module.h"

#include <iostream>

namespace ns3{

    uint32_t gNbNum = 4;
    uint32_t ueNum = 8;
    double gNbHeight = 10.0;
    double ueHeight = 1.5;

    uint32_t embbPacketSize = 1000;
    uint32_t mmtcPacketSize = 2000;
    uint32_t urllcPacketSize = 500;

    uint32_t simTimeMs = 1800;
    uint32_t envStepTime = 200;
    uint32_t eMBBStartTimeMs = 400;
    double centralFrequencyBand = 28e9;
    double bandwidthBand1 = 100e6;
    //double totalTxPower = 4;
    uint16_t PorteMBB = 1234;
    uint16_t PortmMTC = 1235;
    uint16_t PortURLLC = 1236;

    uint32_t simSeed = 1;
    uint32_t testArg = 0;

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;

    uint64_t randomStream = 1;

    NetDeviceContainer enbNetDev;
    // NetDeviceContainer ueNetDev;
    NetDeviceContainer eMBBNetDev;
    NetDeviceContainer mMTCNetDev;
    NetDeviceContainer URLLCNetDev;

    NodeContainer gNbNodes;
    // NodeContainer ueNodes;
    NodeContainer eMBBueNodes;
    NodeContainer mMTCueNodes;
    NodeContainer URLLCueNodes;


    FlowMonitorHelper flowmonHelper;
    Ptr<ns3::FlowMonitor> monitor;
    NodeContainer endpointNodes;

    std::string simTag = "default";
    std::string outputDir = "./";

    // NS_LOG_COMPONENT_DEFINE ("MyGym");

    // NS_OBJECT_ENSURE_REGISTERED (MyGym);

    // MyGym::MyGym (){
    //     NS_LOG_FUNCTION (this);
    //     Simulator::Schedule (MilliSeconds(envStepTime), &MyGym::ScheduleNextStateRead,this);
    // }
    // MyGym::~MyGym (){
    //     NS_LOG_FUNCTION (this);
    // }

    // TypeId
    // MyGym::GetTypeId (void){
    //     static TypeId tid = TypeId ("MyGym")
    //         .SetParent<OpenGymEnv> ()
    //         .SetGroupName ("OpenGym")
    //         .AddConstructor<MyGym> ()
    //     ;
    //     return tid;
    // }
    // /*
    // Define observation space
    // */
    // Ptr<OpenGymSpace>
    // MyGym::GetObservationSpace()
    // {
    //     //uint32_t nodeNum = 4;
    //     float low = 0.0;
    //     float high = 1000.0;
    //     // std::vector<uint32_t> shape = {2,};
    //     // std::string dtype = TypeNameGet<uint32_t> ();
    //     std::vector<uint32_t> shape = {2,};
    //     std::string dtype = TypeNameGet<double> ();
    //     //Ptr<OpenGymBoxSpace> space = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);
    //     //Ptr<OpenGymBoxSpace> space = CreateObject<OpenGymBoxSpace> (shape);
    //     Ptr<OpenGymBoxSpace> space = CreateObject<OpenGymBoxSpace >(low, high, shape, dtype);

    //     // space->AddValue(averageFlowThroughput);
    //     // space->AddValue(averageFlowDelay);
        
    //     NS_LOG_UNCOND ("MyGetObservationSpace: " << space);
    //     return space;
    // }

    // /*
    // Define action space
    // */
    // Ptr<OpenGymSpace>
    // MyGym::GetActionSpace(void)
    // {
    //     Ptr<OpenGymDiscreteSpace> space = CreateObject<OpenGymDiscreteSpace> (ueNum);
    //     NS_LOG_UNCOND ("MyGetActionSpace: " << space);
    //     return space;
    // }

    // /*
    // Define game over condition
    // */
    // bool
    // MyGym::GetGameOver(void)
    // {

    //     bool isGameOver = false;
    //     bool test = false;
    //     static float stepCounter = 0.0;
    //     stepCounter += 1;
    //     if (stepCounter == 10 && test) {
    //         isGameOver = true;
    //     }
    //     NS_LOG_UNCOND ("MyGetGameOver: " << isGameOver);
    //     return isGameOver;
    // }

    // /*
    // Collect observations
    // */
    // Ptr<OpenGymDataContainer>
    // MyGym::GetObservation(void)
    // {
    //     // uint32_t nodeNum = 4;
    //     // uint32_t low = 0.0;
    //     // uint32_t high = 10.0;
    //     // Ptr<UniformRandomVariable> rngInt = CreateObject<UniformRandomVariable> ();

    //     // std::vector<uint32_t> shape = {2,};
    //     // Ptr<OpenGymBoxContainer<uint32_t> > box = CreateObject<OpenGymBoxContainer<uint32_t> >(shape);

    //     std::vector<uint32_t> shape = {2,};
    //     Ptr<OpenGymBoxContainer<double> > box = CreateObject<OpenGymBoxContainer<double> >(shape);

    //     // generate random data
    //     /*for (uint32_t i = 0; i<nodeNum; i++){
    //         uint32_t value = rngInt->GetInteger(low, high);
    //         box->AddValue(value);
    //     }*/
    //     box->AddValue(averageFlowThroughput);
    //     box->AddValue(averageFlowDelay);

    //     NS_LOG_UNCOND ("MyGetObservation: " << box);
    //     return box;
    // }

    // /*
    // Define reward function
    // */
    // float
    // MyGym::GetReward(void)
    // {
    //     static float reward = 0.0;
    //     reward += 1;
    //     return reward;
    // }

    // /*
    // Define extra info. Optional
    // */
    // std::string
    // MyGym::GetExtraInfo(void)
    // {
    //     std::string myInfo = "testInfo";
    //     myInfo += "|123";
    //     NS_LOG_UNCOND("MyGetExtraInfo: " << myInfo);
    //     return myInfo;
    // }


    // /*
    // Execute received actions
    // */
    // bool
    // MyGym::ExecuteActions(Ptr<OpenGymDataContainer> action)
    // {
    //     Ptr<OpenGymDiscreteContainer> discrete = DynamicCast<OpenGymDiscreteContainer>(action);
    //     NS_LOG_UNCOND ("MyExecuteActions: " << action);
    //     return true;
    // }

    // void 
    // MyGym::ScheduleNextStateRead()
    // {
    //     flow_monitor();
    //     Simulator::Schedule (MilliSeconds(envStepTime), &MyGym::ScheduleNextStateRead,this);
    //     //openGym->NotifyCurrentState();
    //     Notify();
    // }

    NS_LOG_COMPONENT_DEFINE("CustomScheduler");
    NS_OBJECT_ENSURE_REGISTERED(CustomScheduler);

    TypeId
    CustomScheduler::GetTypeId()
    {
        //static TypeId tid = TypeId("ns3::CustomScheduler")
          //                  .SetParent<NrMacSchedulerNs3>()
            //                .AddConstructor<CustomScheduler>();
	static TypeId tid = TypeId("ns3::CustomScheduler")
                            .SetParent<NrMacSchedulerTdma>();

        return tid;
    }

    CustomScheduler::CustomScheduler(): NrMacSchedulerNs3()
    {
	    NS_LOG_FUNCTION(this);
    }

    uint8_t
    CustomScheduler::GetTpc() const
    {
        NS_LOG_FUNCTION(this);
        return 1; // 1 is mapped to 0 for Accumulated mode, and to -1 in Absolute mode TS38.213 Table
                // Table 7.1.1-1
    }

    CustomScheduler::BeamSymbolMap
    CustomScheduler::AssignDLRBG(uint32_t symAvail, const ActiveUeMap& activeDl) const
    {
        NS_LOG_FUNCTION(this);

        // BeforeSchedFn beforeSched = std::bind(&NrMacSchedulerTdma::BeforeDlSched,
        //                                     this,
        //                                     std::placeholders::_1,
        //                                     std::placeholders::_2);
        // AfterSuccessfullAssignmentFn SuccFn = std::bind(&NrMacSchedulerTdma::AssignedDlResources,
        //                                                 this,
        //                                                 std::placeholders::_1,
        //                                                 std::placeholders::_2,
        //                                                 std::placeholders::_3);
        // AfterUnsucessfullAssignmentFn UnSuccFn = std::bind(&NrMacSchedulerTdma::NotAssignedDlResources,
        //                                                 this,
        //                                                 std::placeholders::_1,
        //                                                 std::placeholders::_2,
        //                                                 std::placeholders::_3);
        // GetCompareUeFn compareFn = std::bind(&NrMacSchedulerTdma::GetUeCompareDlFn, this);

        // GetTBSFn GetTbs = &NrMacSchedulerUeInfo::GetDlTBS;
        // GetRBGFn GetRBG = &NrMacSchedulerUeInfo::GetDlRBG;
        // GetSymFn GetSym = &NrMacSchedulerUeInfo::GetDlSym;

        // return AssignRBGTDMA(symAvail,
        //                     activeDl,
        //                     "DL",
        //                     beforeSched,
        //                     compareFn,
        //                     GetTbs,
        //                     GetRBG,
        //                     GetSym,
        //                     SuccFn,
        //                     UnSuccFn);
        
        CustomScheduler::BeamSymbolMap ret;
        return ret;
    }

    CustomScheduler::BeamSymbolMap
    CustomScheduler::AssignULRBG(uint32_t symAvail, const ActiveUeMap& activeUl) const
    {
        NS_LOG_FUNCTION(this);
        // BeforeSchedFn beforeSched = std::bind(&NrMacSchedulerTdma::BeforeUlSched,
        //                                     this,
        //                                     std::placeholders::_1,
        //                                     std::placeholders::_2);
        // AfterSuccessfullAssignmentFn SuccFn = std::bind(&NrMacSchedulerTdma::AssignedUlResources,
        //                                                 this,
        //                                                 std::placeholders::_1,
        //                                                 std::placeholders::_2,
        //                                                 std::placeholders::_3);
        // GetCompareUeFn compareFn = std::bind(&NrMacSchedulerTdma::GetUeCompareUlFn, this);
        // AfterUnsucessfullAssignmentFn UnSuccFn = std::bind(&NrMacSchedulerTdma::NotAssignedUlResources,
        //                                                 this,
        //                                                 std::placeholders::_1,
        //                                                 std::placeholders::_2,
        //                                                 std::placeholders::_3);
        // GetTBSFn GetTbs = &NrMacSchedulerUeInfo::GetUlTBS;
        // GetRBGFn GetRBG = &NrMacSchedulerUeInfo::GetUlRBG;
        // GetSymFn GetSym = &NrMacSchedulerUeInfo::GetUlSym;

        // return AssignRBGTDMA(symAvail,
        //                     activeUl,
        //                     "UL",
        //                     beforeSched,
        //                     compareFn,
        //                     GetTbs,
        //                     GetRBG,
        //                     GetSym,
        //                     SuccFn,
        //                     UnSuccFn);
        
        CustomScheduler::BeamSymbolMap ret;
        return ret;
    }

    std::shared_ptr<DciInfoElementTdma>
    CustomScheduler::CreateDlDci(PointInFTPlane* spoint,
                                    const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
                                    [[maybe_unused]] uint32_t maxSym) const
    {
        NS_LOG_FUNCTION(this);
        // uint16_t countLessThanMinBytes = 0;

        // // we do not need to recalculate the TB size here because we already
        // // computed it in side the method AssignDLRBG called before this method.
        // // otherwise, we need to repeat the logic of NrMacSchedulerUeInfo::UpdateDlMetric
        // // here to cover MIMO

        // // Due to MIMO implementation MCS, TB size, ndi, rv, are vectors
        // std::vector<uint8_t> ndi;
        // ndi.resize(ueInfo->m_dlTbSize.size());
        // std::vector<uint8_t> rv;
        // rv.resize(ueInfo->m_dlTbSize.size());
        // uint32_t tbs = 0;
        // for (uint32_t numTb = 0; numTb < ueInfo->m_dlTbSize.size(); numTb++)
        // {
        //     tbs = ueInfo->m_dlTbSize.at(numTb);
        //     if (tbs < 10)
        //     {
        //         countLessThanMinBytes++;
        //         NS_LOG_DEBUG("While creating DL DCI for UE "
        //                     << ueInfo->m_rnti << " stream " << numTb << " assigned " << ueInfo->m_dlRBG
        //                     << " DL RBG, but TBS < 10, reseting its size to zero in UE info");
        //         ueInfo->m_dlTbSize.at(numTb) = 0;
        //         ndi.at(numTb) = 0;
        //         rv.at(numTb) = 0;
        //         continue;
        //     }
        //     ndi.at(numTb) = 1;
        //     rv.at(numTb) = 0;
        // }

        // // If the size of all the TBs is less than 7 bytes (3 mac header, 2 rlc header, 2 data),
        // // then we can't transmit any new data, so don't create dci.
        // if (countLessThanMinBytes == ueInfo->m_dlTbSize.size())
        // {
        //     NS_LOG_DEBUG("While creating DL DCI for UE " << ueInfo->m_rnti << " assigned "
        //                                                 << ueInfo->m_dlRBG << " DL RBG, but TBS < 10");
        //     return nullptr;
        // }

        // const std::vector<uint8_t> notchedRBGsMask = GetDlNotchedRbgMask();
        // int zeroes = std::count(notchedRBGsMask.begin(), notchedRBGsMask.end(), 0);
        // uint32_t numOfAssignableRbgs = GetBandwidthInRbg() - zeroes;

        // uint8_t numSym = static_cast<uint8_t>(ueInfo->m_dlRBG / numOfAssignableRbgs);

        // auto dci = CreateDci(spoint,
        //                     ueInfo,
        //                     ueInfo->m_dlTbSize,
        //                     DciInfoElementTdma::DL,
        //                     ueInfo->m_dlMcs,
        //                     ndi,
        //                     rv,
        //                     std::max(numSym, static_cast<uint8_t>(1)));

        // // The starting point must advance.
        // spoint->m_rbg = 0;
        // spoint->m_sym += numSym;

        // return dci;

        return nullptr;
    }

    std::shared_ptr<DciInfoElementTdma>
    CustomScheduler::CreateUlDci(NrMacSchedulerNs3::PointInFTPlane* spoint,
                                    const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
                                    uint32_t maxSym) const
    {
         NS_LOG_FUNCTION(this);
        // uint32_t tbs = m_ulAmc->CalculateTbSize(ueInfo->m_ulMcs, ueInfo->m_ulRBG * GetNumRbPerRbg());

        // // If is less than 12, 7 (3 mac header, 2 rlc header, 2 data) + SHORT_BSR (5),
        // // then we can't transmit any new data, so don't create dci.
        // if (tbs < 12)
        // {
        //     NS_LOG_DEBUG("While creating UL DCI for UE " << ueInfo->m_rnti << " assigned "
        //                                                 << ueInfo->m_ulRBG << " UL RBG, but TBS "
        //                                                 << tbs << " < 12");
        //     return nullptr;
        // }

        // const std::vector<uint8_t> notchedRBGsMask = GetUlNotchedRbgMask();
        // int zeroes = std::count(notchedRBGsMask.begin(), notchedRBGsMask.end(), 0);
        // uint32_t numOfAssignableRbgs = GetBandwidthInRbg() - zeroes;

        // uint8_t numSym = static_cast<uint8_t>(std::max(ueInfo->m_ulRBG / numOfAssignableRbgs, 1U));
        // numSym = std::min(numSym, static_cast<uint8_t>(maxSym));

        // NS_ASSERT(spoint->m_sym >= numSym);

        // // The starting point must go backward to accomodate the needed sym
        // spoint->m_sym -= numSym;

        // // Due to MIMO implementation MCS and TB size are vectors
        // std::vector<uint8_t> ulMcs = {ueInfo->m_ulMcs};
        // std::vector<uint32_t> ulTbs = {tbs};
        // std::vector<uint8_t> ndi = {1};
        // std::vector<uint8_t> rv = {0};

        // auto dci = CreateDci(spoint, ueInfo, ulTbs, DciInfoElementTdma::UL, ulMcs, ndi, rv, numSym);

        // // Reset the RBG (we are TDMA)
        // spoint->m_rbg = 0;

        // return dci;

        return nullptr;
    }


    
    void flow_monitor(){
        // Print per-flow statistics
        averageFlowThroughput = 0.0;
        averageFlowDelay = 0.0;

        monitor->CheckForLostPackets ();
        Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
        FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
        std::ofstream outFile;
        std::string filename = outputDir + "/" + simTag;
        outFile.open (filename.c_str (), std::ofstream::out | std::ofstream::trunc);

        if (!outFile.is_open ())
            {
            std::cerr << "Can't open file " << filename << std::endl;
            //return 1;
            }

        outFile.setf (std::ios_base::fixed);

        for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
            {
            Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
            std::stringstream protoStream;
            protoStream << (uint16_t) t.protocol;
            if (t.protocol == 6)
                {
                protoStream.str ("TCP");
                }
            if (t.protocol == 17)
                {
                protoStream.str ("UDP");
                }

            outFile << "Flow " << i->first << " (" << t.sourceAddress << ":" << t.sourcePort << " -> " << t.destinationAddress << ":" << t.destinationPort << ") proto " << protoStream.str () << "\n";
            outFile << "  Tx Packets: " << i->second.txPackets << "\n";
            outFile << "  Tx Bytes:   " << i->second.txBytes << "\n";
            outFile << "  TxOffered:  " << i->second.txBytes * 8.0 / ((simTimeMs - eMBBStartTimeMs) / 1000.0) / 1000.0 / 1000.0  << " Mbps\n";
            outFile << "  Rx Bytes:   " << i->second.rxBytes << "\n";


            if (i->second.rxPackets > 0)
                {
                // Measure the duration of the flow from receiver's perspective
                //double rxDuration = i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ();
                double rxDuration = (simTimeMs - eMBBStartTimeMs) / 1000.0;

                averageFlowThroughput += i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
                averageFlowDelay += 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets;

                outFile << "  Throughput: " << i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000  << " Mbps\n";
                outFile << "  Mean delay:  " << 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets << " ms\n";
                //outFile << "  Mean upt:  " << i->second.uptSum / i->second.rxPackets / 1000/1000 << " Mbps \n";
                outFile << "  Mean jitter:  " << 1000 * i->second.jitterSum.GetSeconds () / i->second.rxPackets  << " ms\n";


                }
            else
                {
                outFile << "  Throughput:  0 Mbps\n";
                outFile << "  Mean delay:  0 ms\n";
                outFile << "  Mean jitter: 0 ms\n";
                }
            outFile << "  Rx Packets: " << i->second.rxPackets << "\n";

            } 
            outFile << "\n\n  Mean flow throughput: " << averageFlowThroughput / stats.size () << "\n";
            outFile << "  Mean flow delay: " << averageFlowDelay / stats.size () << "\n";

            outFile.close ();

            std::ifstream f (filename.c_str ());

            if (f.is_open ())
            {
                std::cout << f.rdbuf ();
            }
    }
    
}
