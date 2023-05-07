#include "mygym.h"
#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/node-list.h"
#include "ns3/log.h"
#include "ns3/nr-module.h"
#include "ns3/flow-monitor-module.h"

#include <iostream>

namespace ns3{

    NetDeviceContainer enbNetDev;
    NetDeviceContainer ueNetDev;

    uint32_t gNbNum = 4;
    uint32_t ueNum = 8;

    //std::unordered_map<

    uint32_t embbPacketSize = 1000;
    uint32_t simTimeMs = 1800;
    uint32_t envStepTime = 200;
    uint32_t eMBBStartTimeMs = 400;
    double centralFrequencyBand = 28e9;
    double bandwidthBand1 = 100e6;
    //double totalTxPower = 4;
    uint16_t PorteMBB = 1234;
    uint32_t simSeed = 1;
    uint32_t testArg = 0;

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;

    FlowMonitorHelper flowmonHelper;
    Ptr<ns3::FlowMonitor> monitor;
    NodeContainer endpointNodes;

    std::string simTag = "default";
    std::string outputDir = "./";

    NS_LOG_COMPONENT_DEFINE ("MyGym");

    NS_OBJECT_ENSURE_REGISTERED (MyGym);

    MyGym::MyGym (/* args */){
        NS_LOG_FUNCTION (this);
        Simulator::Schedule (MilliSeconds(envStepTime), &MyGym::ScheduleNextStateRead,this);
    }
    MyGym::~MyGym (){
        NS_LOG_FUNCTION (this);
    }

    TypeId
    MyGym::GetTypeId (void){
        static TypeId tid = TypeId ("MyGym")
            .SetParent<OpenGymEnv> ()
            .SetGroupName ("OpenGym")
            .AddConstructor<MyGym> ()
        ;
        return tid;
    }
    /*
    Define observation space
    */
    Ptr<OpenGymSpace>
    MyGym::GetObservationSpace()
    {
        //uint32_t nodeNum = 4;
        float low = 0.0;
        float high = 1000.0;
        // std::vector<uint32_t> shape = {2,};
        // std::string dtype = TypeNameGet<uint32_t> ();
        std::vector<uint32_t> shape = {2,};
        std::string dtype = TypeNameGet<double> ();
        //Ptr<OpenGymBoxSpace> space = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);
        //Ptr<OpenGymBoxSpace> space = CreateObject<OpenGymBoxSpace> (shape);
        Ptr<OpenGymBoxSpace> space = CreateObject<OpenGymBoxSpace >(low, high, shape, dtype);

        // space->AddValue(averageFlowThroughput);
        // space->AddValue(averageFlowDelay);
        
        NS_LOG_UNCOND ("MyGetObservationSpace: " << space);
        return space;
    }

    /*
    Define action space
    */
    Ptr<OpenGymSpace>
    MyGym::GetActionSpace(void)
    {
        Ptr<OpenGymDiscreteSpace> space = CreateObject<OpenGymDiscreteSpace> (ueNum);
        NS_LOG_UNCOND ("MyGetActionSpace: " << space);
        return space;
    }

    /*
    Define game over condition
    */
    bool
    MyGym::GetGameOver(void)
    {

        bool isGameOver = false;
        bool test = false;
        static float stepCounter = 0.0;
        stepCounter += 1;
        if (stepCounter == 10 && test) {
            isGameOver = true;
        }
        NS_LOG_UNCOND ("MyGetGameOver: " << isGameOver);
        return isGameOver;
    }

    /*
    Collect observations
    */
    Ptr<OpenGymDataContainer>
    MyGym::GetObservation(void)
    {
        // uint32_t nodeNum = 4;
        // uint32_t low = 0.0;
        // uint32_t high = 10.0;
        // Ptr<UniformRandomVariable> rngInt = CreateObject<UniformRandomVariable> ();

        // std::vector<uint32_t> shape = {2,};
        // Ptr<OpenGymBoxContainer<uint32_t> > box = CreateObject<OpenGymBoxContainer<uint32_t> >(shape);

        std::vector<double> shape = {2,};
        Ptr<OpenGymBoxContainer<double> > box = CreateObject<OpenGymBoxContainer<double> >(shape);

        // generate random data
        /*for (uint32_t i = 0; i<nodeNum; i++){
            uint32_t value = rngInt->GetInteger(low, high);
            box->AddValue(value);
        }*/
        box->AddValue(averageFlowThroughput);
        box->AddValue(averageFlowDelay);

        NS_LOG_UNCOND ("MyGetObservation: " << box);
        return box;
    }

    /*
    Define reward function
    */
    float
    MyGym::GetReward(void)
    {
        static float reward = 0.0;
        reward += 1;
        return reward;
    }

    /*
    Define extra info. Optional
    */
    std::string
    MyGym::GetExtraInfo(void)
    {
        std::string myInfo = "testInfo";
        myInfo += "|123";
        NS_LOG_UNCOND("MyGetExtraInfo: " << myInfo);
        return myInfo;
    }


    /*
    Execute received actions
    */
    bool
    MyGym::ExecuteActions(Ptr<OpenGymDataContainer> action)
    {
        Ptr<OpenGymDiscreteContainer> discrete = DynamicCast<OpenGymDiscreteContainer>(action);
        NS_LOG_UNCOND ("MyExecuteActions: " << action);
        return true;
    }

    void 
    MyGym::ScheduleNextStateRead()
    {
        flow_monitor();
        Simulator::Schedule (MilliSeconds(envStepTime), &MyGym::ScheduleNextStateRead,this);
        //openGym->NotifyCurrentState();
        Notify();
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
