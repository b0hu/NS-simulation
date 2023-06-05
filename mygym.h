#ifndef MY_GYM_ENTITY_H
#define MY_GYM_ENTITY_H

//#include "ns3/opengym-module.h"
#include "ns3/net-device-container.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/nr-mac-scheduler-tdma.h"
#include "ns3/nr-mac-scheduler-ns3.h"
#include "ns3/nr-mac-scheduler-ofdma.h"

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

    
    /*class CustomScheduler :  public NrMacSchedulerNs3
    {
        public:
            CustomScheduler();
            ~CustomScheduler() override
            {}
            static TypeId GetTypeId();
            //struct PoInFTPlane ft = new PointInFTPlane(rbg,sym);

        private:
        std::shared_ptr<NrMacSchedulerUeInfo> CreateUeRepresentation(
        const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const override;

        uint8_t GetTpc() const override;
        
        BeamSymbolMap AssignDLRBG(uint32_t symAvail, const ActiveUeMap& activeDl) const override;
        
        BeamSymbolMap AssignULRBG(uint32_t symAvail, const ActiveUeMap& activeUl) const override;

        std::shared_ptr<DciInfoElementTdma> CreateDlDci(
        PointInFTPlane* spoint,
        const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
        uint32_t maxSym) const override;

        std::shared_ptr<DciInfoElementTdma> CreateUlDci(
        PointInFTPlane* spoint,
        const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
        uint32_t maxSym) const override;

        void ChangeDlBeam(PointInFTPlane* spoint, uint32_t symOfBeam) const override
        {}

        void ChangeUlBeam(PointInFTPlane* spoint, uint32_t symOfBeam) const override
        {}

    };*/

    class CustomScheduler : public NrMacSchedulerOfdma
    {
    public:
        /**
         * \brief GetTypeId
         * \return The TypeId of the class
         */
        static TypeId GetTypeId();

        /**
         * \brief NrMacSchedulerTdmaRR constructor
         */
        CustomScheduler();

        /**
         * \brief ~NrMacSchedulerTdmaRR deconstructor
         */
        ~CustomScheduler() override
        {
        }
	/*class Test :
	    {	
    		public:
		Test(){}
		~Test(){}
		void hello(){
			cout << "hello" << endl;
		}
		};*/

    protected:
	        /**
         * \brief Create an UE representation of the type NrMacSchedulerUeInfoRR
         * \param params parameters
         * \return NrMacSchedulerUeInfoRR instance
         */
        std::shared_ptr<NrMacSchedulerUeInfo> CreateUeRepresentation(
            const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const override;

        /**
         * \brief Return the comparison function to sort DL UE according to the scheduler policy
         * \return a pointer to NrMacSchedulerUeInfoRR::CompareUeWeightsDl
         */
        std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                        const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
        GetUeCompareDlFn() const override;

        /**
         * \brief Return the comparison function to sort UL UE according to the scheduler policy
         * \return a pointer to NrMacSchedulerUeInfoRR::CompareUeWeightsUl
         */
        std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                        const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
        GetUeCompareUlFn() const override;

        /**
         * \brief Update the UE representation after a symbol (DL) has been assigned to it
         * \param ue UE to which a symbol has been assigned
         * \param assigned the amount of resources assigned
         * \param totAssigned the total amount of resources assigned in the slot
         *
         * Update DL metrics by calling NrMacSchedulerUeInfoRR::UpdateDlMetric
         */
        void AssignedDlResources(const UePtrAndBufferReq& ue,
                                const FTResources& assigned,
                                const FTResources& totAssigned) const override;

        /**
         * \brief Update the UE representation after a symbol (DL) has been assigned to it
         * \param ue UE to which a symbol has been assigned
         * \param assigned the amount of resources assigned
         * \param totAssigned the total amount of resources assigned in the slot
         *
         * Update DL metrics by calling NrMacSchedulerUeInfoRR::UpdateUlMetric
         */
        void AssignedUlResources(const UePtrAndBufferReq& ue,
                                const FTResources& assigned,
                                const FTResources& totAssigned) const override;

        // RR is a simple scheduler: it doesn't do anything in the next
        // inherited calls.
        void NotAssignedDlResources(const UePtrAndBufferReq& ue,
                                    const FTResources& notAssigned,
                                    const FTResources& totalAssigned) const override
        {
        }

        void NotAssignedUlResources(const UePtrAndBufferReq& ue,
                                    const FTResources& notAssigned,
                                    const FTResources& totalAssigned) const override
        {
        }

        void BeforeDlSched(const UePtrAndBufferReq& ue,
                        const FTResources& assignableInIteration) const override
        {
        }

        void BeforeUlSched(const UePtrAndBufferReq& ue,
                        const FTResources& assignableInIteration) const override
        {
        }
    };

    class CustomSchedulerUeInfo : public NrMacSchedulerUeInfo
    {
    public:
        /**
         * \brief NrMacSchedulerUeInfoRR constructor
         * \param rnti RNTI of the UE
         * \param beamConfId BeamConfId of the UE
         * \param fn A function that tells how many RB per RBG
         */
        CustomSchedulerUeInfo(uint16_t rnti, BeamConfId beamConfId, const GetRbPerRbgFn& fn)
            : NrMacSchedulerUeInfo(rnti, beamConfId, fn)
        {
        }

        /**
         * \brief comparison function object (i.e. an object that satisfies the
         * requirements of Compare) which returns ​true if the first argument is less
         * than (i.e. is ordered before) the second.
         * \param lue Left UE
         * \param rue Right UE
         * \return true if the assigned RBG of lue is less than the assigned RBG of rue
         *
         * The ordering is made by considering the RBG. An UE with 0 RBG will always
         * be the first (i.e., has an higher priority) in a RR scheduler. The objective
         * is to distribute evenly all the resources, in order to have the same RBG
         * number for all the UEs.
         */
        static bool CompareUeWeightsDl(const NrMacSchedulerNs3::UePtrAndBufferReq& lue,
                                    const NrMacSchedulerNs3::UePtrAndBufferReq& rue)
        {
            return (lue.first->m_dlRBG < rue.first->m_dlRBG);
        }

        /**
         * \brief comparison function object (i.e. an object that satisfies the
         * requirements of Compare) which returns ​true if the first argument is less
         * than (i.e. is ordered before) the second.
         * \param lue Left UE
         * \param rue Right UE
         * \return true if the assigned RBG of lue is less than the assigned RBG of rue
         *
         * The ordering is made by considering the RBG. An UE with 0 RBG will always
         * be the first (i.e., has an higher priority) in a RR scheduler. The objective
         * is to distribute evenly all the resources, in order to have the same RBG
         * number for all the UEs.
         */
        static bool CompareUeWeightsUl(const NrMacSchedulerNs3::UePtrAndBufferReq& lue,
                                    const NrMacSchedulerNs3::UePtrAndBufferReq& rue)
        {
            return (lue.first->m_ulRBG < rue.first->m_ulRBG);
        }
    };

}


#endif
