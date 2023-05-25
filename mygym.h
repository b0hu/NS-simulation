#ifndef MY_GYM_ENTITY_H
#define MY_GYM_ENTITY_H

//#include "ns3/opengym-module.h"
#include "ns3/net-device-container.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/nr-mac-scheduler-tdma.h"
#include "ns3/nr-mac-scheduler-ns3.h"

namespace ns3{
    extern NetDeviceContainer enbNetDev;
    // extern NetDeviceContainer ueNetDev;
    extern NetDeviceContainer eMBBNetDev;
    extern NetDeviceContainer mMTCNetDev;
    extern NetDeviceContainer URLLCNetDev;

    extern NodeContainer gNbNodes;
    // extern NodeContainer ueNodes;
    extern NodeContainer eMBBueNodes;
    extern NodeContainer mMTCueNodes;
    extern NodeContainer URLLCueNodes;
    // extern GridScenarioHelper gridScenario;

    extern uint64_t randomStream;
    
    extern uint32_t gNbNum;
    extern uint32_t ueNum;
    extern double gNbHeight;
    extern double ueHeight;

    extern uint32_t embbPacketSize;
    extern uint32_t mmtcPacketSize;
    extern uint32_t urllcPacketSize;

    extern uint32_t simTimeMs;
    extern uint32_t envStepTime;
    extern uint32_t eMBBStartTimeMs;
    extern double centralFrequencyBand;
    extern double bandwidthBand1 ;
    //double totalTxPower = 4;
    extern uint16_t PorteMBB;
    extern uint16_t PortmMTC;
    extern uint16_t PortURLLC;

    extern uint32_t simSeed;
    extern uint32_t testArg;

    extern double averageFlowThroughput;
    extern double averageFlowDelay;

    extern FlowMonitorHelper flowmonHelper;
    extern Ptr<ns3::FlowMonitor> monitor;
    extern NodeContainer endpointNodes;

    void flow_monitor();

    // class MyGym : public OpenGymEnv
    // {
    // private:
    //     void ScheduleNextStateRead();
    // public:
    //     MyGym(/* args */);
    //     ~MyGym();
    //     static TypeId GetTypeId (void);
    //     Ptr<OpenGymSpace> GetActionSpace();
    //     Ptr<OpenGymSpace> GetObservationSpace();
    //     Ptr<OpenGymDataContainer> GetObservation();
    //     float GetReward(void);
    //     bool GetGameOver(void);
    //     bool ExecuteActions(Ptr<OpenGymDataContainer> action);
    //     std::string GetExtraInfo(void);
    // };

    class CustomScheduler :  NrMacSchedulerNs3
    {
        public:
            CustomScheduler();
            ~CustomScheduler() override;
            static TypeId GetTypeId();
            //struct PoInFTPlane ft = new PointInFTPlane(rbg,sym);

        private:
        std::shared_ptr<NrMacSchedulerUeInfo> CreateUeRepresentation(
        const NrMacCschedSapProvider::CschedUeConfigReqParameters& params);

        uint8_t GetTpc();
        
        BeamSymbolMap AssignDLRBG(uint32_t symAvail, const ActiveUeMap& activeDl);
        
        BeamSymbolMap AssignULRBG(uint32_t symAvail, const ActiveUeMap& activeUl);

        std::shared_ptr<DciInfoElementTdma> CreateDlDci(
        PointInFTPlane* spoint,
        const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
        uint32_t maxSym);

        std::shared_ptr<DciInfoElementTdma> CreateUlDci(
        PointInFTPlane* spoint,
        const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
        uint32_t maxSym);

        void ChangeDlBeam(PointInFTPlane* spoint, uint32_t symOfBeam)
        {}

        void ChangeUlBeam(PointInFTPlane* spoint, uint32_t symOfBeam)
        {}

    };

    /*class Test : Object{
        public:
            Test();
    };*/
}


#endif
