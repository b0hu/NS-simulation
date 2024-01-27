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
#include <fstream>
#include <cmath>

namespace ns3{

    uint32_t gNbNum = 9;
    uint32_t ueNum = 8;
    double gNbHeight = 10.0;
    double ueHeight = 1.5;

    uint32_t embbPacketSize1 = 1500;
    uint32_t embbPacketSize2 = 1600;
    uint32_t mmtcPacketSize1 = 2000;
    uint32_t mmtcPacketSize2 = 2048;
    uint32_t urllcPacketSize1 = 32;
    uint32_t urllcPacketSize2 = 64;

    uint32_t simTimeMs = 200000;
    uint32_t envStepTime = 1000;
    uint32_t StartTimeMs = 200;
    double centralFrequencyBand = 28e9;
    double bandwidthBand1 = 100e6;
    //double totalTxPower = 4;
    uint16_t PorteMBB1 = 1234;
    uint16_t PortmMTC1 = 1235;
    uint16_t PortURLLC1 = 1236;

    uint16_t PorteMBB2 = 1237;
    uint16_t PortmMTC2 = 1238;
    uint16_t PortURLLC2 = 1239;

    std::vector<NrMacSchedulerNs3::UePtrAndBufferReq> tempvec;
    std::unordered_map<int, float> ue_metric;
    // std::unordered_set<int> rnti_set;
    int ue_num = 0;

    uint32_t simSeed = 1;
    uint32_t testArg = 0;

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;

    double embbthruput = 0.0;
    double embbdelay = 0.0;

    double urllcthruput = 0.0;
    double urllcdelay = 0.0;
    double urllcreliability = 0.0;

    double mmtcthruput = 0.0;
    double mmtcdelay = 0.0;

    // double duration = 0.0;

    uint64_t randomStream = 1;

    NetDeviceContainer enbNetDev;
    // NetDeviceContainer ueNetDev;
    NetDeviceContainer eMBBNetDev1;
    NetDeviceContainer eMBBNetDev2;
    NetDeviceContainer mMTCNetDev1;
    NetDeviceContainer mMTCNetDev2;
    NetDeviceContainer URLLCNetDev1;
    NetDeviceContainer URLLCNetDev2;


    NodeContainer gNbNodes;
    // NodeContainer ueNodes;
    NodeContainer eMBBueNodes1;
    NodeContainer eMBBueNodes2;
    NodeContainer mMTCueNodes1;
    NodeContainer mMTCueNodes2;
    NodeContainer URLLCueNodes1;
    NodeContainer URLLCueNodes2;

    FlowMonitorHelper flowmonHelper;
    Ptr<ns3::FlowMonitor> monitor;
    NodeContainer endpointNodes;

    std::string simTag = "default";
    std::string outputDir = "./";

    std::ofstream myfile;
    std::ofstream embbfile;
    std::ofstream urllcfile;
    std::ofstream mmtcfile;
    std::ofstream rewardfile;
    
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
    //     //Simulator::Schedule (MilliSeconds(envStepTime), &MyGym::ScheduleNextStateRead,this);
    //     //openGym->NotifyCurrentState();
    //     Notify();
    // }

    // MyGym::MyGym (){
    //     NS_LOG_FUNCTION (this);
    //     Simulator::Schedule (MilliSeconds(envStepTime), &MyGym::ScheduleNextStateRead,this);
    // }
    // MyGym::~MyGym (){
    //     NS_LOG_FUNCTION (this);
    // }

    // TypeId
    // GetTypeId (void){
    //     static TypeId tid = TypeId ("MyGym")
    //         .SetParent<OpenGymEnv> ()
    //         .SetGroupName ("OpenGym")
    //         .AddConstructor<MyGym> ()
    //     ;
    //     return tid;
    // }

    Ptr<OpenGymInterface> openGym = CreateObject<OpenGymInterface> (1111);
    
    /*
    Define observation space
    */
    Ptr<OpenGymSpace>
    GetObservationSpace()
    {
        float low = 0.0;
        float high = 10000.0;

        std::vector<uint32_t> shape = {2,};
        std::string dtype = TypeNameGet<double> ();

        Ptr<OpenGymBoxSpace> space = CreateObject<OpenGymBoxSpace >(low, high, shape, dtype);
        // Ptr<OpenGymDictSpace> dict_space = CreateObject<OpenGymDictSpace >();
        
        NS_LOG_UNCOND ("MyGetObservationSpace: " << space);
        return space;
    }

    /*
    Define action space
    */
    Ptr<OpenGymSpace>
    GetActionSpace(void)
    {
        float low = 0.0;
        float high = 100.0;
        std::vector<uint32_t> shape = {27,};
        std::string dtype = TypeNameGet<float> ();
        Ptr<OpenGymBoxSpace> space = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);
        NS_LOG_UNCOND ("MyGetActionSpace: " << space);
        return space;
    }

    /*
    Define game over condition
    */
    bool
    GetGameOver(void)
    {

        bool isGameOver = false;
        // bool test = false;
        // static float stepCounter = 0.0;
        // stepCounter += 1;
        // if (stepCounter == 10 && test) {
        //     isGameOver = true;
        // }
        NS_LOG_UNCOND ("MyGetGameOver: " << isGameOver);
        return isGameOver;
    }

    /*
    Collect observations
    */
    Ptr<OpenGymDataContainer>
    GetObservation(void)
    {
        // uint32_t nodeNum = 4;
        // uint32_t low = 0.0;
        // uint32_t high = 10.0;
        // Ptr<UniformRandomVariable> rngInt = CreateObject<UniformRandomVariable> ();

        // std::vector<uint32_t> shape = {2,};
        // Ptr<OpenGymBoxContainer<uint32_t> > box = CreateObject<OpenGymBoxContainer<uint32_t> >(shape);

        std::vector<uint32_t> shape = {27,};
        Ptr<OpenGymBoxContainer<double> > box = CreateObject<OpenGymBoxContainer<double> >(shape);
        Ptr<OpenGymDictContainer> dict = CreateObject<OpenGymDictContainer>();

        reset_flow();
        // box->AddValue(averageFlowThroughput);
        // box->AddValue(averageFlowDelay);
        // box->AddValue(ue_num);

        for (auto& ue:tempvec)
        {
            box->AddValue(ue.second);
        }

        //add tempvec info

        NS_LOG_UNCOND ("MyGetObservation: " << box);
        return box;
    }

    /*
    Define reward function
    */
    float
    GetReward(void)
    {
        float reward = 100 * (pow((embbthruput * 30), 2) - pow((urllcdelay * 5), 2) + pow((mmtcthruput * 100), 2) - pow((mmtcdelay * 50), 2));
        rewardfile << reward << std::endl;
        return reward;
    }

    /*
    Define extra info. Optional
    */
    std::string
    GetExtraInfo(void)
    {
        std::string myInfo = "testInfo";
        myInfo += "|123";
        NS_LOG_UNCOND("MyGetExtraInfo: " << myInfo);
        return myInfo;
    }

    bool
    ExecuteActions(Ptr<OpenGymDataContainer> action)
    {
        Ptr<OpenGymBoxContainer<float>> box = DynamicCast<OpenGymBoxContainer<float>>(action);
        std::vector<float> actionVector = box->GetData();
        std::vector<float>::iterator i = actionVector.begin();
        
        for(auto& it:tempvec){
            // update value of ue_metric
            ue_metric[it.first->m_rnti] = *i;
            i++;
        }

        NS_LOG_UNCOND ("MyExecuteActions: " << action);
        return true;
    }

    void 
    ScheduleNextStateRead()
    {
        // flow_monitor();
        // Simulator::Schedule (MilliSeconds(envStepTime), &ScheduleNextStateRead);
        // Simulator::ScheduleNow (&ScheduleNextStateRead);
        openGym->NotifyCurrentState();
        // Notify();
    }

    NS_LOG_COMPONENT_DEFINE("CustomScheduler");
    NS_OBJECT_ENSURE_REGISTERED(CustomScheduler);

    void reset_flow(){
        averageFlowThroughput = 0.0;
        averageFlowDelay = 0.0;

        embbthruput = 0.0;
        embbdelay = 0.0;
        urllcthruput = 0.0;
        urllcdelay = 0.0;
        urllcreliability = 0.0;
        mmtcthruput = 0.0;
        mmtcdelay = 0.0;

        double duration = Simulator::Now().GetSeconds () - (StartTimeMs/1000);

        monitor->CheckForLostPackets ();
        Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
        FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

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


            if (i->second.rxPackets > 0)
                {
                // Measure the duration of the flow from receiver's perspective
                //double rxDuration = i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ();
                // double rxDuration = (simTimeMs - StartTimeMs) / 1000.0;

                averageFlowThroughput += i->second.rxBytes * 8.0 / 1000 / 1000 / duration;
                averageFlowDelay += 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets/ duration;

                if(t.destinationPort == 1234 || t.destinationPort == 1237){
                    embbthruput += i->second.rxBytes * 8.0 / 1000 / 1000 / duration;
                    embbdelay += 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets / duration;
                }
                else if(t.destinationPort == 1235 || t.destinationPort == 1238){
                    urllcthruput += i->second.rxBytes * 8.0 / 1000 / 1000 / duration;
                    urllcdelay += 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets / duration;
                    urllcreliability += i->second.rxBytes / i->second.txBytes;
                }
                else if(t.destinationPort == 1236 || t.destinationPort == 1239){
                    mmtcthruput += i->second.rxBytes * 8.0 / 1000 / 1000 / duration;
                    mmtcdelay += 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets / duration;
                }

                }

                // std::cout << "timeFirstTxPacket: "<< i->second.timeFirstTxPacket << std::endl;
       
            }
            // std::cout << "flowthroughput: " << averageFlowThroughput << std::endl;
            averageFlowThroughput = averageFlowThroughput/ stats.size ();
            averageFlowDelay = averageFlowDelay/ stats.size ();
            embbthruput = embbthruput/ stats.size ();
            embbdelay = embbdelay/ stats.size ();
            urllcthruput = urllcthruput/ stats.size ();
            urllcdelay = urllcdelay/ stats.size ();
            urllcreliability = urllcreliability/ stats.size ();
            mmtcthruput = mmtcthruput/ stats.size ();
            mmtcdelay = mmtcdelay/ stats.size ();

            // myfile << averageFlowThroughput << "," << averageFlowDelay << std::endl;
            // embbfile << embbthruput << "," << embbdelay << std::endl;
            // urllcfile << urllcthruput << "," << urllcdelay << std::endl;
            // mmtcfile << mmtcthruput << "," << mmtcdelay << std::endl;
    }

    void save_data(){
        myfile << averageFlowThroughput << "," << averageFlowDelay << std::endl;
        embbfile << embbthruput << "," << embbdelay << std::endl;
        urllcfile << urllcthruput << "," << urllcdelay << "," << urllcreliability << std::endl;
        mmtcfile << mmtcthruput << "," << mmtcdelay << std::endl;
        Simulator::Schedule (MilliSeconds(envStepTime), &save_data);
    }
    
    void flow_monitor(){
        // Print per-flow statistics
        averageFlowThroughput = 0.0;
        averageFlowDelay = 0.0;

        embbthruput = 0.0;
        embbdelay = 0.0;
        urllcthruput = 0.0;
        urllcdelay = 0.0;
        mmtcthruput = 0.0;
        mmtcdelay = 0.0;

        monitor->CheckForLostPackets ();
        Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
        FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
        std::ofstream outFile;
        std::string filename = outputDir + "/" + simTag;
        outFile.open (filename.c_str (), std::ofstream::out | std::ofstream::trunc);

        // std::cout << "rnti set: " << std::endl;
        // for(std::unordered_set<int>::iterator i = rnti_set.begin(); i != rnti_set.end(); ++i) {
        //     std::cout << (*i) << std::endl;
        // }

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
            outFile << "  TxOffered:  " << i->second.txBytes * 8.0 / ((simTimeMs - StartTimeMs) / 1000.0) / 1000.0 / 1000.0  << " Mbps\n";
            outFile << "  Rx Bytes:   " << i->second.rxBytes << "\n";


            if (i->second.rxPackets > 0)
                {
                // Measure the duration of the flow from receiver's perspective
                //double rxDuration = i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ();
                double rxDuration = (simTimeMs - StartTimeMs) / 1000.0;

                averageFlowThroughput += i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
                averageFlowDelay += 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets;

                if(t.destinationPort == 1234 || t.destinationPort == 1237){
                    embbthruput += i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
                    embbdelay += 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets;
                }
                else if(t.destinationPort == 1235 || t.destinationPort == 1238){
                    urllcthruput += i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
                    urllcdelay += 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets;
                }
                else if(t.destinationPort == 1236 || t.destinationPort == 1239){
                    mmtcthruput += i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
                    mmtcdelay += 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets;
                }

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

            outFile << "\n\n  Mean flow throughput of eMBB: " << embbthruput / stats.size () << "\n";
            outFile << "  Mean flow delay of eMBB: " << embbdelay / stats.size () << "\n";

            outFile << "\n\n  Mean flow throughput of URLLC: " << urllcthruput / stats.size () << "\n";
            outFile << "  Mean flow delay of URLLC: " << urllcdelay / stats.size () << "\n";

            outFile << "\n\n  Mean flow throughput of mmTC: " << mmtcthruput / stats.size () << "\n";
            outFile << "  Mean flow delay of mmTC: " << mmtcdelay / stats.size () << "\n";

            outFile.close ();

            std::ifstream f (filename.c_str ());

            if (f.is_open ())
            {
                std::cout << f.rdbuf ();
            }
    }

    TypeId
    CustomScheduler::GetTypeId()
    {
        static TypeId tid = TypeId("ns3::CustomScheduler")
                                .SetParent<NrMacSchedulerNs3>()
                                .AddConstructor<CustomScheduler>();
        return tid;
    }

    CustomScheduler::CustomScheduler()
        : NrMacSchedulerNs3()
    {
        openGym->SetGetActionSpaceCb( MakeCallback (&GetActionSpace) );
        openGym->SetGetObservationSpaceCb( MakeCallback (&GetObservationSpace) );
        openGym->SetGetGameOverCb( MakeCallback (&GetGameOver) );
        openGym->SetGetObservationCb( MakeCallback (&GetObservation) );
        openGym->SetGetRewardCb( MakeCallback (&GetReward) );
        openGym->SetGetExtraInfoCb( MakeCallback (&GetExtraInfo) );
        openGym->SetExecuteActionsCb( MakeCallback (&ExecuteActions) );
        // Simulator::Schedule (MilliSeconds(envStepTime), &ScheduleNextStateRead);
    }

    std::shared_ptr<NrMacSchedulerUeInfo>
    CustomScheduler::CreateUeRepresentation(
        const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const
    {
        NS_LOG_FUNCTION(this);
        return std::make_shared<CustomSchedulerUeInfo>(
            params.m_rnti,
            params.m_beamConfId,
            std::bind(&CustomScheduler::GetNumRbPerRbg, this));
    }

    void
    CustomScheduler::AssignedDlResources(const UePtrAndBufferReq& ue,
                                            [[maybe_unused]] const FTResources& assigned,
                                            [[maybe_unused]] const FTResources& totAssigned) const
    {
        NS_LOG_FUNCTION(this);
        // std::cout << "assigned in AssignedDlResources: " << assigned.m_rbg << " " << assigned.m_sym << std::endl;
        // std::cout << "totassigned in AssignedDlResources: " << totAssigned.m_rbg << " " << totAssigned.m_sym << std::endl;
        GetFirst GetUe;
        GetUe(ue)->UpdateDlMetric(m_dlAmc);
    }

    void
    CustomScheduler::AssignedUlResources(const UePtrAndBufferReq& ue,
                                            [[maybe_unused]] const FTResources& assigned,
                                            [[maybe_unused]] const FTResources& totAssigned) const
    {
        NS_LOG_FUNCTION(this);
        GetFirst GetUe;
        GetUe(ue)->UpdateUlMetric(m_ulAmc);
    }

    std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                    const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
    CustomScheduler::GetUeCompareDlFn() const
    {
        return CustomSchedulerUeInfo::CompareUeWeightsDl;
    }

    std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                    const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
    CustomScheduler::GetUeCompareUlFn() const
    {
        return CustomSchedulerUeInfo::CompareUeWeightsUl;
    }


    CustomScheduler::BeamSymbolMap
    CustomScheduler::GetSymPerBeam(uint32_t symAvail,
                                    const NrMacSchedulerNs3::ActiveUeMap& activeDl) const
    {
        NS_LOG_FUNCTION(this);

        GetSecond GetUeVector;
        GetSecond GetUeBufSize;
        GetFirst GetBeamId;
        double bufTotal = 0.0;
        uint8_t symUsed = 0;
        BeamSymbolMap ret;

        // Compute buf total
        for (const auto& el : activeDl)
        {
            for (const auto& ue : GetUeVector(el))
            {
                bufTotal += GetUeBufSize(ue);
            }
        }

        for (const auto& el : activeDl)
        {
            uint32_t bufSizeBeam = 0;
            for (const auto& ue : GetUeVector(el))
            {
                bufSizeBeam += GetUeBufSize(ue);
            }

            double tmp = symAvail / bufTotal;
            uint32_t symForBeam = static_cast<uint32_t>(bufSizeBeam * tmp);
            symUsed += symForBeam;
            ret.emplace(std::make_pair(GetBeamId(el), symForBeam));
            NS_LOG_DEBUG("Assigned to beam " << GetBeamId(el) << " symbols " << symForBeam);
        }

        NS_ASSERT(symAvail >= symUsed);
        if (symAvail - symUsed > 0)
        {
            uint8_t symToRedistribute = symAvail - symUsed;
            while (symToRedistribute > 0)
            {
                BeamSymbolMap::iterator min = ret.end();
                for (auto it = ret.begin(); it != ret.end(); ++it)
                {
                    if (min == ret.end() || it->second < min->second)
                    {
                        min = it;
                    }
                }
                min->second += 1;
                symToRedistribute--;
                NS_LOG_DEBUG("Assigned to beam "
                            << min->first << " an additional symbol, for a total of " << min->second);
            }
        }

        // Trigger the trace source firing, using const_cast as we don't change
        // the internal state of the class
        for (const auto& v : ret)
        {
            const_cast<CustomScheduler*>(this)->m_tracedValueSymPerBeam = v.second;
        }

        return ret;
    }

    NrMacSchedulerNs3::BeamSymbolMap
    CustomScheduler::AssignDLRBG(uint32_t symAvail, const ActiveUeMap& activeDl) const
    {
        NS_LOG_FUNCTION(this);

        NS_LOG_DEBUG("# beams active flows: " << activeDl.size() << ", # sym: " << symAvail);
        // std::cout << "# beams active flows: " << activeDl.size() << ", # sym: " << symAvail << std::endl;

        GetFirst GetBeamId;
        GetSecond GetUeVector;
        BeamSymbolMap symPerBeam = GetSymPerBeam(symAvail, activeDl);

        // Iterate through the different beams
        for (const auto& el : activeDl)
        {
            // Distribute the RBG evenly among UEs of the same beam
            uint32_t beamSym = symPerBeam.at(GetBeamId(el));
            uint32_t rbgAssignable = 1 * beamSym;
            std::vector<UePtrAndBufferReq> ueVector;
            FTResources assigned(0, 0);
            const std::vector<uint8_t> dlNotchedRBGsMask = GetDlNotchedRbgMask();
            uint32_t resources = dlNotchedRBGsMask.size() > 0
                                    ? std::count(dlNotchedRBGsMask.begin(), dlNotchedRBGsMask.end(), 1)
                                    : GetBandwidthInRbg();
            NS_ASSERT(resources > 0);

            tempvec.clear();

            for (const auto& ue : GetUeVector(el))
            {
                ueVector.emplace_back(ue);
                tempvec.emplace_back(ue);
            }

            for (auto& ue : ueVector)
            {
                BeforeDlSched(ue, FTResources(rbgAssignable * beamSym, beamSym));
            }

            ue_metric.clear();
            
            for (auto& ue : ueVector)
            {
                std::cout << "ue rnti:"<< ue.first->m_rnti << std::endl;
                std::cout << "ue buffer:"<< ue.second << std::endl;

                // set the metric to default value 0
                // ue_metric[ue.first->m_rnti] = 0;
                ue_metric.insert(std::make_pair(ue.first->m_rnti, 0));
                // rnti_set.insert(ue.first->m_rnti);
            }

            ue_num = tempvec.size();
            std::cout << ue_num << std::endl;

            //determine the metric for every ue
            if(ue_num > 1){
                openGym->NotifyCurrentState();
            } 

            for (std::unordered_map<int, float>::iterator it = ue_metric.begin(); it != ue_metric.end(); it++) {
                std::cout << "rnti: " << (*it).first << ", metric: " << (*it).second << "\n";
            }

            // for (auto iter = ueVector.begin(); iter != ueVector.end(); iter++) {
            //     std::cout << "ue rnti:"<< iter->first->m_rnti << std::endl;
            //     std::cout << "ue buffer:"<< iter->second << std::endl;
            // }


            while (resources > 0)
            {
                GetFirst GetUe;
                std::sort(ueVector.begin(), ueVector.end(), GetUeCompareDlFn());
                auto schedInfoIt = ueVector.begin();

                // Ensure fairness: pass over UEs which already has enough resources to transmit
                while (schedInfoIt != ueVector.end())
                {
                    uint32_t bufQueueSize = schedInfoIt->second;

                    // if there are two streams we add the TbSizes of the two
                    // streams to satisfy the bufQueueSize
                    uint32_t tbSize = 0;
                    for (const auto& it : GetUe(*schedInfoIt)->m_dlTbSize)
                    {
                        tbSize += it;
                    }

                    if (tbSize >= std::max(bufQueueSize, 10U))
                    {
                        if (GetUe(*schedInfoIt)->m_dlTbSize.size() > 1)
                        {
                            // This "if" is purely for MIMO. In MIMO, for example, if the
                            // first TB size is big enough to empty the buffer then we
                            // should not allocate anything to the second stream. In this
                            // case, if we allocate bytes to the second stream, the UE
                            // would expect the TB but the gNB would not be able to transmit
                            // it. This would break HARQ TX state machine at UE PHY.

                            uint8_t streamCounter = 0;
                            uint32_t copyBufQueueSize = bufQueueSize;
                            auto dlTbSizeIt = GetUe(*schedInfoIt)->m_dlTbSize.begin();
                            while (dlTbSizeIt != GetUe(*schedInfoIt)->m_dlTbSize.end())
                            {
                                if (copyBufQueueSize != 0)
                                {
                                    NS_LOG_DEBUG("Stream " << +streamCounter << " with TB size "
                                                        << *dlTbSizeIt << " needed to TX MIMO TB");
                                    if (*dlTbSizeIt >= copyBufQueueSize)
                                    {
                                        copyBufQueueSize = 0;
                                    }
                                    else
                                    {
                                        copyBufQueueSize = copyBufQueueSize - *dlTbSizeIt;
                                    }
                                    streamCounter++;
                                    dlTbSizeIt++;
                                }
                                else
                                {
                                    // if we are here, that means previously iterated
                                    // streams were enough to empty the buffer. We do
                                    // not need this stream. Make its TB size zero.
                                    NS_LOG_DEBUG("Stream " << +streamCounter << " with TB size "
                                                        << *dlTbSizeIt
                                                        << " not needed to TX MIMO TB");
                                    *dlTbSizeIt = 0;
                                    streamCounter++;
                                    dlTbSizeIt++;
                                }
                            }
                        }
                        schedInfoIt++;
                    }
                    else
                    {
                        break;
                    }
                }

                // In the case that all the UE already have their requirements fullfilled,
                // then stop the beam processing and pass to the next
                if (schedInfoIt == ueVector.end())
                {
                    break;
                }

                // Assign 1 RBG for each available symbols for the beam,
                // and then update the count of available resources
                GetUe(*schedInfoIt)->m_dlRBG += rbgAssignable;
                assigned.m_rbg += rbgAssignable;

                GetUe(*schedInfoIt)->m_dlSym = beamSym;
                assigned.m_sym = beamSym;

                resources -= 1; // Resources are RBG, so they do not consider the beamSym

                // Update metrics
                NS_LOG_DEBUG("Assigned " << rbgAssignable << " DL RBG, spanned over " << beamSym
                                        << " SYM, to UE " << GetUe(*schedInfoIt)->m_rnti);
                // Following call to AssignedDlResources would update the
                // TB size in the NrMacSchedulerUeInfo of this particular UE
                // according the Rank Indicator reported by it. Only one call
                // to this method is enough even if the UE reported rank indicator 2,
                // since the number of RBG assigned to both the streams are the same.
                AssignedDlResources(*schedInfoIt, FTResources(rbgAssignable, beamSym), assigned);

                // Update metrics for the unsuccessfull UEs (who did not get any resource in this
                // iteration)
                for (auto& ue : ueVector)
                {
                    if (GetUe(ue)->m_rnti != GetUe(*schedInfoIt)->m_rnti)
                    {
                        NotAssignedDlResources(ue, FTResources(rbgAssignable, beamSym), assigned);
                    }
                }
            }
        }

        return symPerBeam;
    }

    NrMacSchedulerNs3::BeamSymbolMap
    CustomScheduler::AssignULRBG(uint32_t symAvail, const ActiveUeMap& activeUl) const
    {
        NS_LOG_FUNCTION(this);

        NS_LOG_DEBUG("# beams active flows: " << activeUl.size() << ", # sym: " << symAvail);

        GetFirst GetBeamId;
        GetSecond GetUeVector;
        BeamSymbolMap symPerBeam = GetSymPerBeam(symAvail, activeUl);

        // Iterate through the different beams
        for (const auto& el : activeUl)
        {
            // Distribute the RBG evenly among UEs of the same beam
            uint32_t beamSym = symPerBeam.at(GetBeamId(el));
            uint32_t rbgAssignable = 1 * beamSym;
            std::vector<UePtrAndBufferReq> ueVector;
            FTResources assigned(0, 0);
            const std::vector<uint8_t> ulNotchedRBGsMask = GetUlNotchedRbgMask();
            uint32_t resources = ulNotchedRBGsMask.size() > 0
                                    ? std::count(ulNotchedRBGsMask.begin(), ulNotchedRBGsMask.end(), 1)
                                    : GetBandwidthInRbg();
            NS_ASSERT(resources > 0);

            for (const auto& ue : GetUeVector(el))
            {
                ueVector.emplace_back(ue);
            }

            for (auto& ue : ueVector)
            {
                BeforeUlSched(ue, FTResources(rbgAssignable * beamSym, beamSym));
            }

            while (resources > 0)
            {
                GetFirst GetUe;
                std::sort(ueVector.begin(), ueVector.end(), GetUeCompareUlFn());
                auto schedInfoIt = ueVector.begin();

                // Ensure fairness: pass over UEs which already has enough resources to transmit
                while (schedInfoIt != ueVector.end())
                {
                    uint32_t bufQueueSize = schedInfoIt->second;
                    if (GetUe(*schedInfoIt)->m_ulTbSize >= std::max(bufQueueSize, 12U))
                    {
                        schedInfoIt++;
                    }
                    else
                    {
                        break;
                    }
                }

                // In the case that all the UE already have their requirements fullfilled,
                // then stop the beam processing and pass to the next
                if (schedInfoIt == ueVector.end())
                {
                    break;
                }

                // Assign 1 RBG for each available symbols for the beam,
                // and then update the count of available resources
                GetUe(*schedInfoIt)->m_ulRBG += rbgAssignable;
                assigned.m_rbg += rbgAssignable;

                GetUe(*schedInfoIt)->m_ulSym = beamSym;
                assigned.m_sym = beamSym;

                resources -= 1; // Resources are RBG, so they do not consider the beamSym

                // Update metrics
                NS_LOG_DEBUG("Assigned " << rbgAssignable << " UL RBG, spanned over " << beamSym
                                        << " SYM, to UE " << GetUe(*schedInfoIt)->m_rnti);
                AssignedUlResources(*schedInfoIt, FTResources(rbgAssignable, beamSym), assigned);

                // Update metrics for the unsuccessfull UEs (who did not get any resource in this
                // iteration)
                for (auto& ue : ueVector)
                {
                    if (GetUe(ue)->m_rnti != GetUe(*schedInfoIt)->m_rnti)
                    {
                        NotAssignedUlResources(ue, FTResources(rbgAssignable, beamSym), assigned);
                    }
                }
            }
        }

        return symPerBeam;
    }

    /**
     * \brief Create the DL DCI in OFDMA mode
     * \param spoint Starting point
     * \param ueInfo UE representation
     * \param maxSym Maximum symbols to use
     * \return a pointer to the newly created instance
     *
     * The function calculates the TBS and then call CreateDci().
     */
    std::shared_ptr<DciInfoElementTdma>
    CustomScheduler::CreateDlDci(NrMacSchedulerNs3::PointInFTPlane* spoint,
                                    const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
                                    uint32_t maxSym) const
    {
        NS_LOG_FUNCTION(this);

        uint16_t countLessThanMinBytes = 0;

        // we do not need to recalculate the TB size here because we already
        // computed it in side the method AssignDLRBG called before this method.
        // otherwise, we need to repeat the logic of NrMacSchedulerUeInfo::UpdateDlMetric
        // here to cover MIMO

        // Due to MIMO implementation MCS, TB size, ndi, rv, are vectors
        std::vector<uint8_t> ndi;
        ndi.resize(ueInfo->m_dlTbSize.size());
        std::vector<uint8_t> rv;
        rv.resize(ueInfo->m_dlTbSize.size());

        for (uint32_t numTb = 0; numTb < ueInfo->m_dlTbSize.size(); numTb++)
        {
            uint32_t tbs = ueInfo->m_dlTbSize.at(numTb);
            if (tbs < 10)
            {
                countLessThanMinBytes++;
                NS_LOG_DEBUG("While creating DCI for UE "
                            << ueInfo->m_rnti << " stream " << numTb << " assigned " << ueInfo->m_dlRBG
                            << " DL RBG, but TBS < 10, reseting its size to zero in UE info");
                ueInfo->m_dlTbSize.at(numTb) = 0;
                ndi.at(numTb) = 0;
                rv.at(numTb) = 0;
                continue;
            }
            ndi.at(numTb) = 1;
            rv.at(numTb) = 0;
        }

        NS_ASSERT_MSG(ueInfo->m_dlRBG % maxSym == 0,
                    " MaxSym " << maxSym << " RBG: " << ueInfo->m_dlRBG);
        NS_ASSERT(ueInfo->m_dlRBG <= maxSym * GetBandwidthInRbg());
        NS_ASSERT(spoint->m_rbg < GetBandwidthInRbg());
        NS_ASSERT(maxSym <= UINT8_MAX);

        // If the size of all the TBs is less than 10 bytes,
        // i.e. 7 bytes (the minimum TX opportunity by RLC AM) + 3 bytes of the MAC header
        // then we can't transmit any new data, so don't create dci.
        if (countLessThanMinBytes == ueInfo->m_dlTbSize.size())
        {
            NS_LOG_DEBUG("While creating DCI for UE " << ueInfo->m_rnti << " assigned "
                                                    << ueInfo->m_dlRBG << " DL RBG, but TBS < 10");
            return nullptr;
        }

        uint32_t RBGNum = ueInfo->m_dlRBG / maxSym;
        std::vector<uint8_t> rbgBitmask = GetDlNotchedRbgMask();

        if (rbgBitmask.size() == 0)
        {
            rbgBitmask = std::vector<uint8_t>(GetBandwidthInRbg(), 1);
        }

        // rbgBitmask is all 1s or have 1s in the place we are allowed to transmit.

        NS_ASSERT(rbgBitmask.size() == GetBandwidthInRbg());

        uint32_t lastRbg = spoint->m_rbg;

        // Limit the places in which we can transmit following the starting point
        // and the number of RBG assigned to the UE
        for (uint32_t i = 0; i < GetBandwidthInRbg(); ++i)
        {
            if (i >= spoint->m_rbg && RBGNum > 0 && rbgBitmask[i] == 1)
            {
                // assigned! Decrement RBGNum and continue the for
                RBGNum--;
                lastRbg = i;
            }
            else
            {
                // Set to 0 the position < spoint->m_rbg OR the remaining RBG when
                // we already assigned the number of requested RBG
                rbgBitmask[i] = 0;
            }
        }

        NS_ASSERT_MSG(
            RBGNum == 0,
            "If you see this message, it means that the AssignRBG and CreateDci method are unaligned");

        std::ostringstream oss;
        for (const auto& x : rbgBitmask)
        {
            oss << std::to_string(x) << " ";
        }

        NS_LOG_INFO("UE " << ueInfo->m_rnti << " assigned RBG from " << spoint->m_rbg << " with mask "
                        << oss.str() << " for " << static_cast<uint32_t>(maxSym) << " SYM.");

        std::shared_ptr<DciInfoElementTdma> dci =
            std::make_shared<DciInfoElementTdma>(ueInfo->m_rnti,
                                                DciInfoElementTdma::DL,
                                                spoint->m_sym,
                                                maxSym,
                                                ueInfo->m_dlMcs,
                                                ueInfo->m_dlTbSize,
                                                ndi,
                                                rv,
                                                DciInfoElementTdma::DATA,
                                                GetBwpId(),
                                                GetTpc());

        dci->m_rbgBitmask = std::move(rbgBitmask);

        NS_ASSERT(std::count(dci->m_rbgBitmask.begin(), dci->m_rbgBitmask.end(), 0) !=
                GetBandwidthInRbg());

        spoint->m_rbg = lastRbg + 1;

        return dci;
    }

    // std::shared_ptr<DciInfoElementTdma>
    // CustomScheduler::CreateUlDci(PointInFTPlane* spoint,
    //                                 const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
    //                                 uint32_t maxSym) const
    // {
    //     NS_LOG_FUNCTION(this);

    //     uint32_t tbs = m_ulAmc->CalculateTbSize(ueInfo->m_ulMcs, ueInfo->m_ulRBG * GetNumRbPerRbg());

    //     // If is less than 12, i.e., 7 (3 mac header, 2 rlc header, 2 data) + 5 bytes for
    //     // the SHORT_BSR. then we can't transmit any new data, so don't create dci.
    //     if (tbs < 12)
    //     {
    //         NS_LOG_DEBUG("While creating UL DCI for UE " << ueInfo->m_rnti << " assigned "
    //                                                     << ueInfo->m_ulRBG << " UL RBG, but TBS < 12");
    //         return nullptr;
    //     }

    //     uint32_t RBGNum = ueInfo->m_ulRBG / maxSym;
    //     std::vector<uint8_t> rbgBitmask = GetUlNotchedRbgMask();

    //     if (rbgBitmask.size() == 0)
    //     {
    //         rbgBitmask = std::vector<uint8_t>(GetBandwidthInRbg(), 1);
    //     }

    //     // rbgBitmask is all 1s or have 1s in the place we are allowed to transmit.

    //     NS_ASSERT(rbgBitmask.size() == GetBandwidthInRbg());

    //     uint32_t lastRbg = spoint->m_rbg;
    //     uint32_t assigned = RBGNum;

    //     // Limit the places in which we can transmit following the starting point
    //     // and the number of RBG assigned to the UE
    //     for (uint32_t i = 0; i < GetBandwidthInRbg(); ++i)
    //     {
    //         if (i >= spoint->m_rbg && RBGNum > 0 && rbgBitmask[i] == 1)
    //         {
    //             // assigned! Decrement RBGNum and continue the for
    //             RBGNum--;
    //             lastRbg = i;
    //         }
    //         else
    //         {
    //             // Set to 0 the position < spoint->m_rbg OR the remaining RBG when
    //             // we already assigned the number of requested RBG
    //             rbgBitmask[i] = 0;
    //         }
    //     }

    //     NS_ASSERT_MSG(
    //         RBGNum == 0,
    //         "If you see this message, it means that the AssignRBG and CreateDci method are unaligned");

    //     NS_LOG_INFO("UE " << ueInfo->m_rnti << " assigned RBG from " << spoint->m_rbg << " to "
    //                     << spoint->m_rbg + assigned << " for " << static_cast<uint32_t>(maxSym)
    //                     << " SYM.");

    //     // Due to MIMO implementation MCS, TB size, ndi, rv, are vectors
    //     std::vector<uint8_t> ulMcs = {ueInfo->m_ulMcs};
    //     std::vector<uint32_t> ulTbs = {tbs};
    //     std::vector<uint8_t> ndi = {1};
    //     std::vector<uint8_t> rv = {0};

    //     NS_ASSERT(spoint->m_sym >= maxSym);
    //     std::shared_ptr<DciInfoElementTdma> dci =
    //         std::make_shared<DciInfoElementTdma>(ueInfo->m_rnti,
    //                                             DciInfoElementTdma::UL,
    //                                             spoint->m_sym - maxSym,
    //                                             maxSym,
    //                                             ulMcs,
    //                                             ulTbs,
    //                                             ndi,
    //                                             rv,
    //                                             DciInfoElementTdma::DATA,
    //                                             GetBwpId(),
    //                                             GetTpc());

    //     dci->m_rbgBitmask = std::move(rbgBitmask);

    //     std::ostringstream oss;
    //     for (auto x : dci->m_rbgBitmask)
    //     {
    //         oss << std::to_string(x) << " ";
    //     }
    //     NS_LOG_INFO("UE " << ueInfo->m_rnti << " DCI RBG mask: " << oss.str());

    //     NS_ASSERT(std::count(dci->m_rbgBitmask.begin(), dci->m_rbgBitmask.end(), 0) !=
    //             GetBandwidthInRbg());

    //     spoint->m_rbg = lastRbg + 1;

    //     return dci;
    // }

    void
    CustomScheduler::ChangeDlBeam(PointInFTPlane* spoint, uint32_t symOfBeam) const
    {
        spoint->m_rbg = 0;
        spoint->m_sym += symOfBeam;
    }

    void
    CustomScheduler::ChangeUlBeam(PointInFTPlane* spoint, uint32_t symOfBeam) const
    {
        spoint->m_rbg = 0;
        spoint->m_sym -= symOfBeam;
    }

    std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>
    CustomScheduler::GetUeVectorFromActiveUeMap(const NrMacSchedulerNs3::ActiveUeMap& activeUes)
    {
        std::vector<UePtrAndBufferReq> ueVector;
        for (const auto& el : activeUes)
        {
            uint64_t size = ueVector.size();
            GetSecond GetUeVector;
            for (const auto& ue : GetUeVector(el))
            {
                ueVector.emplace_back(ue);
            }
            NS_ASSERT(size + GetUeVector(el).size() == ueVector.size());
        }
        return ueVector;
    }

    // CustomScheduler::BeamSymbolMap
    // CustomScheduler::AssignRBGTDMA(
    //     uint32_t symAvail,
    //     const ActiveUeMap& activeUe,
    //     const std::string& type,
    //     const BeforeSchedFn& BeforeSchedFn,
    //     const GetCompareUeFn& GetCompareFn,
    //     const GetTBSFn& GetTBSFn,
    //     const GetRBGFn& GetRBGFn,
    //     const GetSymFn& GetSymFn,
    //     const AfterSuccessfullAssignmentFn& SuccessfullAssignmentFn,
    //     const AfterUnsucessfullAssignmentFn& UnSuccessfullAssignmentFn) const
    // {
    //     NS_LOG_FUNCTION(this);
    //     NS_LOG_DEBUG("Assigning RBG in " << type << ", # beams active flows: " << activeUe.size()
    //                                     << ", # sym: " << symAvail);

    //     // Create vector of UE (without considering the beam)
    //     std::vector<UePtrAndBufferReq> ueVector = GetUeVectorFromActiveUeMap(activeUe);

    //     // Distribute the symbols following the selected behaviour among UEs
    //     uint32_t resources = symAvail;
    //     FTResources assigned(0, 0);

    //     const std::vector<uint8_t> notchedRBGsMask =
    //         type == "DL" ? GetDlNotchedRbgMask() : GetUlNotchedRbgMask();
    //     int zeroes = std::count(notchedRBGsMask.begin(), notchedRBGsMask.end(), 0);
    //     uint32_t numOfAssignableRbgs = GetBandwidthInRbg() - zeroes;
    //     NS_ASSERT(numOfAssignableRbgs > 0);

    //     for (auto& ue : ueVector)
    //     {
    //         BeforeSchedFn(ue, FTResources(numOfAssignableRbgs, 1));
    //     }

    //     while (resources > 0)
    //     {
    //         GetFirst GetUe;

    //         auto schedInfoIt = ueVector.begin();

    //         std::sort(ueVector.begin(), ueVector.end(), GetCompareFn());

    //         // Ensure fairness: pass over UEs which already has enough resources to transmit
    //         while (schedInfoIt != ueVector.end())
    //         {
    //             uint32_t bufQueueSize = schedInfoIt->second;

    //             if (GetTBSFn(GetUe(*schedInfoIt)) >= std::max(bufQueueSize, 10U))
    //             {
    //                 if (GetUe(*schedInfoIt)->m_dlTbSize.size() > 1 && type == "DL")
    //                 {

    //                     uint8_t streamCounter = 0;
    //                     uint32_t copyBufQueueSize = bufQueueSize;
    //                     auto dlTbSizeIt = GetUe(*schedInfoIt)->m_dlTbSize.begin();
    //                     while (dlTbSizeIt != GetUe(*schedInfoIt)->m_dlTbSize.end())
    //                     {
    //                         if (copyBufQueueSize != 0)
    //                         {
    //                             NS_LOG_DEBUG("Stream " << +streamCounter << " with TB size "
    //                                                 << *dlTbSizeIt << " needed to TX MIMO TB");
    //                             if (*dlTbSizeIt >= copyBufQueueSize)
    //                             {
    //                                 copyBufQueueSize = 0;
    //                             }
    //                             else
    //                             {
    //                                 copyBufQueueSize = copyBufQueueSize - *dlTbSizeIt;
    //                             }
    //                             streamCounter++;
    //                             dlTbSizeIt++;
    //                         }
    //                         else
    //                         {
    //                             // if we are here, that means previously iterated
    //                             // streams were enough to empty the buffer. We do
    //                             // not need this stream. Make its TB size zero.
    //                             NS_LOG_DEBUG("Stream " << +streamCounter << " with TB size "
    //                                                 << *dlTbSizeIt << " not needed to TX MIMO TB");
    //                             *dlTbSizeIt = 0;
    //                             streamCounter++;
    //                             dlTbSizeIt++;
    //                         }
    //                     }
    //                 }
    //                 NS_LOG_INFO("UE " << GetUe(*schedInfoIt)->m_rnti << " TBS "
    //                                 << GetTBSFn(GetUe(*schedInfoIt)) << " queue " << bufQueueSize
    //                                 << ", passing");
    //                 schedInfoIt++;
    //             }
    //             else
    //             {
    //                 break;
    //             }
    //         }

    //         // In the case that all the UE already have their requirements fullfilled,
    //         // then stop the assignment
    //         if (schedInfoIt == ueVector.end())
    //         {
    //             NS_LOG_INFO("All the UE already have their resources allocated. Skipping the beam");
    //             break;
    //         }

    //         // Assign 1 entire symbol (full RBG) to the selected UE and to the total
    //         // resources assigned count
    //         GetRBGFn(GetUe(*schedInfoIt)) += numOfAssignableRbgs;
    //         assigned.m_rbg += numOfAssignableRbgs;

    //         GetSymFn(GetUe(*schedInfoIt)) += 1;
    //         assigned.m_sym += 1;

    //         // substract 1 SYM from the number of sym available for the while loop
    //         resources -= 1;

    //         // Update metrics for the successfull UE
    //         NS_LOG_DEBUG("Assigned " << numOfAssignableRbgs << " " << type << " RBG (= 1 SYM) to UE "
    //                                 << GetUe(*schedInfoIt)->m_rnti
    //                                 << " total assigned up to now: " << GetRBGFn(GetUe(*schedInfoIt))
    //                                 << " that corresponds to " << assigned.m_rbg);
    //         SuccessfullAssignmentFn(*schedInfoIt, FTResources(numOfAssignableRbgs, 1), assigned);

    //         // Update metrics for the unsuccessfull UEs (who did not get any resource in this iteration)
    //         for (auto& ue : ueVector)
    //         {
    //             if (GetUe(ue)->m_rnti != GetUe(*schedInfoIt)->m_rnti)
    //             {
    //                 UnSuccessfullAssignmentFn(ue, FTResources(numOfAssignableRbgs, 1), assigned);
    //             }
    //         }
    //     }

    //     // Count the number of assigned symbol of each beam.
    //     CustomScheduler::BeamSymbolMap ret;
    //     for (const auto& el : activeUe)
    //     {
    //         uint32_t symOfBeam = 0;
    //         for (const auto& ue : el.second)
    //         {
    //             symOfBeam += GetRBGFn(ue.first) / numOfAssignableRbgs;
    //         }
    //         ret.insert(std::make_pair(el.first, symOfBeam));
    //     }
    //     return ret;
    // }

    // CustomScheduler::BeamSymbolMap
    // CustomScheduler::AssignDLRBG(uint32_t symAvail, const ActiveUeMap& activeDl) const
    // {
    //     NS_LOG_FUNCTION(this);

    //     BeforeSchedFn beforeSched = std::bind(&NrMacSchedulerTdma::BeforeDlSched,
    //                                         this,
    //                                         std::placeholders::_1,
    //                                         std::placeholders::_2);
    //     AfterSuccessfullAssignmentFn SuccFn = std::bind(&NrMacSchedulerTdma::AssignedDlResources,
    //                                                     this,
    //                                                     std::placeholders::_1,
    //                                                     std::placeholders::_2,
    //                                                     std::placeholders::_3);
    //     AfterUnsucessfullAssignmentFn UnSuccFn = std::bind(&NrMacSchedulerTdma::NotAssignedDlResources,
    //                                                     this,
    //                                                     std::placeholders::_1,
    //                                                     std::placeholders::_2,
    //                                                     std::placeholders::_3);
    //     GetCompareUeFn compareFn = std::bind(&NrMacSchedulerTdma::GetUeCompareDlFn, this);

    //     GetTBSFn GetTbs = &NrMacSchedulerUeInfo::GetDlTBS;
    //     GetRBGFn GetRBG = &NrMacSchedulerUeInfo::GetDlRBG;
    //     GetSymFn GetSym = &NrMacSchedulerUeInfo::GetDlSym;

    //     return AssignRBGTDMA(symAvail,
    //                         activeDl,
    //                         "DL",
    //                         beforeSched,
    //                         compareFn,
    //                         GetTbs,
    //                         GetRBG,
    //                         GetSym,
    //                         SuccFn,
    //                         UnSuccFn);
    // }

    /**
     * \brief Assign the available UL RBG to the UEs
     * \param symAvail Number of available symbols
     * \param activeUl active DL flows and UE
     * \return a map between the beam and the symbols assigned to each one
     *
     * The function will prepare all the needed callbacks to return UE UL parameters
     * (e.g., the UL TBS, the UL RBG) and then will call NrMacSchedulerTdma::AssignRBGTDMA.
     */
    // CustomScheduler::BeamSymbolMap
    // CustomScheduler::AssignULRBG(uint32_t symAvail, const ActiveUeMap& activeUl) const
    // {
    //     NS_LOG_FUNCTION(this);
    //     BeforeSchedFn beforeSched = std::bind(&CustomScheduler::BeforeUlSched,
    //                                         this,
    //                                         std::placeholders::_1,
    //                                         std::placeholders::_2);
    //     AfterSuccessfullAssignmentFn SuccFn = std::bind(&CustomScheduler::AssignedUlResources,
    //                                                     this,
    //                                                     std::placeholders::_1,
    //                                                     std::placeholders::_2,
    //                                                     std::placeholders::_3);
    //     GetCompareUeFn compareFn = std::bind(&CustomScheduler::GetUeCompareUlFn, this);
    //     AfterUnsucessfullAssignmentFn UnSuccFn = std::bind(&CustomScheduler::NotAssignedUlResources,
    //                                                     this,
    //                                                     std::placeholders::_1,
    //                                                     std::placeholders::_2,
    //                                                     std::placeholders::_3);
    //     GetTBSFn GetTbs = &NrMacSchedulerUeInfo::GetUlTBS;
    //     GetRBGFn GetRBG = &NrMacSchedulerUeInfo::GetUlRBG;
    //     GetSymFn GetSym = &NrMacSchedulerUeInfo::GetUlSym;

    //     return AssignRBGTDMA(symAvail,
    //                         activeUl,
    //                         "UL",
    //                         beforeSched,
    //                         compareFn,
    //                         GetTbs,
    //                         GetRBG,
    //                         GetSym,
    //                         SuccFn,
    //                         UnSuccFn);
    // }

    /**
     * \brief Create a DL DCI starting from spoint and spanning maxSym symbols
     * \param spoint Starting point of the DCI
     * \param ueInfo UE representation
     * \param maxSym Maximum number of symbols for the creation of the DCI
     * \return a pointer to the newly created DCI
     *
     * The method calculates the TBS and the real number of symbols needed, and
     * then call CreateDci().
     */
    // std::shared_ptr<DciInfoElementTdma>
    // CustomScheduler::CreateDlDci(PointInFTPlane* spoint,
    //                                 const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
    //                                 [[maybe_unused]] uint32_t maxSym) const
    // {
    //     NS_LOG_FUNCTION(this);
    //     uint16_t countLessThanMinBytes = 0;

    //     // we do not need to recalculate the TB size here because we already
    //     // computed it in side the method AssignDLRBG called before this method.
    //     // otherwise, we need to repeat the logic of NrMacSchedulerUeInfo::UpdateDlMetric
    //     // here to cover MIMO

    //     // Due to MIMO implementation MCS, TB size, ndi, rv, are vectors
    //     std::vector<uint8_t> ndi;
    //     ndi.resize(ueInfo->m_dlTbSize.size());
    //     std::vector<uint8_t> rv;
    //     rv.resize(ueInfo->m_dlTbSize.size());
    //     uint32_t tbs = 0;
    //     for (uint32_t numTb = 0; numTb < ueInfo->m_dlTbSize.size(); numTb++)
    //     {
    //         tbs = ueInfo->m_dlTbSize.at(numTb);
    //         if (tbs < 10)
    //         {
    //             countLessThanMinBytes++;
    //             NS_LOG_DEBUG("While creating DL DCI for UE "
    //                         << ueInfo->m_rnti << " stream " << numTb << " assigned " << ueInfo->m_dlRBG
    //                         << " DL RBG, but TBS < 10, reseting its size to zero in UE info");
    //             ueInfo->m_dlTbSize.at(numTb) = 0;
    //             ndi.at(numTb) = 0;
    //             rv.at(numTb) = 0;
    //             continue;
    //         }
    //         ndi.at(numTb) = 1;
    //         rv.at(numTb) = 0;
    //     }

    //     // If the size of all the TBs is less than 7 bytes (3 mac header, 2 rlc header, 2 data),
    //     // then we can't transmit any new data, so don't create dci.
    //     if (countLessThanMinBytes == ueInfo->m_dlTbSize.size())
    //     {
    //         NS_LOG_DEBUG("While creating DL DCI for UE " << ueInfo->m_rnti << " assigned "
    //                                                     << ueInfo->m_dlRBG << " DL RBG, but TBS < 10");
    //         return nullptr;
    //     }

    //     const std::vector<uint8_t> notchedRBGsMask = GetDlNotchedRbgMask();
    //     int zeroes = std::count(notchedRBGsMask.begin(), notchedRBGsMask.end(), 0);
    //     uint32_t numOfAssignableRbgs = GetBandwidthInRbg() - zeroes;

    //     uint8_t numSym = static_cast<uint8_t>(ueInfo->m_dlRBG / numOfAssignableRbgs);

    //     auto dci = CreateDci(spoint,
    //                         ueInfo,
    //                         ueInfo->m_dlTbSize,
    //                         DciInfoElementTdma::DL,
    //                         ueInfo->m_dlMcs,
    //                         ndi,
    //                         rv,
    //                         std::max(numSym, static_cast<uint8_t>(1)));

    //     // The starting point must advance.
    //     spoint->m_rbg = 0;
    //     spoint->m_sym += numSym;

    //     return dci;
    // }

    /**
     * \brief Create a UL DCI starting from spoint and spanning maxSym symbols
     * \param spoint Starting point of the DCI
     * \param ueInfo UE representation
     * \return a pointer to the newly created DCI
     *
     * The method calculates the TBS and the real number of symbols needed, and
     * then call CreateDci().
     * Allocate the DCI going bacward from the starting point (it should be called
     * ending point maybe).
     */
    std::shared_ptr<DciInfoElementTdma>
    CustomScheduler::CreateUlDci(NrMacSchedulerNs3::PointInFTPlane* spoint,
                                    const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
                                    uint32_t maxSym) const
    {
        NS_LOG_FUNCTION(this);
        uint32_t tbs = m_ulAmc->CalculateTbSize(ueInfo->m_ulMcs, ueInfo->m_ulRBG * GetNumRbPerRbg());

        // If is less than 12, 7 (3 mac header, 2 rlc header, 2 data) + SHORT_BSR (5),
        // then we can't transmit any new data, so don't create dci.
        if (tbs < 12)
        {
            NS_LOG_DEBUG("While creating UL DCI for UE " << ueInfo->m_rnti << " assigned "
                                                        << ueInfo->m_ulRBG << " UL RBG, but TBS "
                                                        << tbs << " < 12");
            return nullptr;
        }

        const std::vector<uint8_t> notchedRBGsMask = GetUlNotchedRbgMask();
        int zeroes = std::count(notchedRBGsMask.begin(), notchedRBGsMask.end(), 0);
        uint32_t numOfAssignableRbgs = GetBandwidthInRbg() - zeroes;

        uint8_t numSym = static_cast<uint8_t>(std::max(ueInfo->m_ulRBG / numOfAssignableRbgs, 1U));
        numSym = std::min(numSym, static_cast<uint8_t>(maxSym));

        NS_ASSERT(spoint->m_sym >= numSym);

        // The starting point must go backward to accomodate the needed sym
        spoint->m_sym -= numSym;

        // Due to MIMO implementation MCS and TB size are vectors
        std::vector<uint8_t> ulMcs = {ueInfo->m_ulMcs};
        std::vector<uint32_t> ulTbs = {tbs};
        std::vector<uint8_t> ndi = {1};
        std::vector<uint8_t> rv = {0};

        auto dci = CreateDci(spoint, ueInfo, ulTbs, DciInfoElementTdma::UL, ulMcs, ndi, rv, numSym);

        // Reset the RBG (we are TDMA)
        spoint->m_rbg = 0;

        return dci;
    }

    uint8_t
    CustomScheduler::GetTpc() const
    {
        NS_LOG_FUNCTION(this);
        return 1; // 1 is mapped to 0 for Accumulated mode, and to -1 in Absolute mode TS38.213 Table
                // Table 7.1.1-1
    }

    /**
     * \brief Create a DCI with the parameters specified as input
     * \param spoint starting point
     * \param ueInfo ue representation
     * \param tbs Transport Block Size
     * \param fmt Format of the DCI (UL or DL)
     * \param mcs MCS
     * \param numSym Number of symbols
     * \return a pointer to the newly created DCI
     *
     * Creates a TDMA DCI (a DCI with all the resource block assigned for the
     * specified number of symbols)
     */
    std::shared_ptr<DciInfoElementTdma>
    CustomScheduler::CreateDci(NrMacSchedulerNs3::PointInFTPlane* spoint,
                                const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
                                std::vector<uint32_t> tbs,
                                DciInfoElementTdma::DciFormat fmt,
                                std::vector<uint8_t> mcs,
                                std::vector<uint8_t> ndi,
                                std::vector<uint8_t> rv,
                                uint8_t numSym) const
    {
        NS_LOG_FUNCTION(this);
        uint32_t sumTbSize = 0;
        for (const auto& it : tbs)
        {
            sumTbSize += it;
        }
        NS_ASSERT(sumTbSize > 0);
        NS_ASSERT(numSym > 0);

        std::shared_ptr<DciInfoElementTdma> dci =
            std::make_shared<DciInfoElementTdma>(ueInfo->m_rnti,
                                                fmt,
                                                spoint->m_sym,
                                                numSym,
                                                mcs,
                                                tbs,
                                                ndi,
                                                rv,
                                                DciInfoElementTdma::DATA,
                                                GetBwpId(),
                                                GetTpc());

        std::vector<uint8_t> rbgAssigned =
            fmt == DciInfoElementTdma::DL ? GetDlNotchedRbgMask() : GetUlNotchedRbgMask();

        if (rbgAssigned.size() == 0)
        {
            rbgAssigned = std::vector<uint8_t>(GetBandwidthInRbg(), 1);
        }

        NS_ASSERT(rbgAssigned.size() == GetBandwidthInRbg());

        dci->m_rbgBitmask = std::move(rbgAssigned);

        std::ostringstream oss;
        for (auto& x : dci->m_rbgBitmask)
        {
            oss << std::to_string(x) << " ";
        }

        NS_LOG_INFO("UE " << ueInfo->m_rnti << " assigned RBG from " << spoint->m_rbg << " with mask "
                        << oss.str() << " for " << static_cast<uint32_t>(numSym) << " SYM ");

        NS_ASSERT(std::count(dci->m_rbgBitmask.begin(), dci->m_rbgBitmask.end(), 0) !=
                GetBandwidthInRbg());

        return dci;
    }


    
}
