#ifndef MY_GYM_ENTITY_H
#define MY_GYM_ENTITY_H

#include "ns3/opengym-module.h"
#include "ns3/net-device-container.h"
#include "ns3/flow-monitor-module.h"

namespace ns3{
    extern NetDeviceContainer enbNetDev;
    extern NetDeviceContainer ueNetDev;
    extern NodeContainer gNbNodes;
    extern NodeContainer ueNodes;

    extern int64_t randomStream;
    
    extern uint32_t gNbNum;
    extern uint32_t ueNum;

    extern uint32_t embbPacketSize;
    extern uint32_t simTimeMs;
    extern uint32_t envStepTime;
    extern uint32_t eMBBStartTimeMs;
    extern double centralFrequencyBand;
    extern double bandwidthBand1 ;
    //double totalTxPower = 4;
    extern uint16_t PorteMBB;
    extern uint32_t simSeed;
    extern uint32_t testArg;

    extern double averageFlowThroughput;
    extern double averageFlowDelay;

    extern FlowMonitorHelper flowmonHelper;
    extern Ptr<ns3::FlowMonitor> monitor;
    extern NodeContainer endpointNodes;

    void make_grid();
    void flow_monitor();

    class MyGym : public OpenGymEnv
    {
    private:
        void ScheduleNextStateRead();
    public:
        MyGym(/* args */);
        ~MyGym();
        static TypeId GetTypeId (void);
        Ptr<OpenGymSpace> GetActionSpace();
        Ptr<OpenGymSpace> GetObservationSpace();
        Ptr<OpenGymDataContainer> GetObservation();
        float GetReward(void);
        bool GetGameOver(void);
        bool ExecuteActions(Ptr<OpenGymDataContainer> action);
        std::string GetExtraInfo(void);
    };
    
}

#endif
