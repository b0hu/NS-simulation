#ifndef MY_GYM_ENTITY_H
#define MY_GYM_ENTITY_H

#include "ns3/opengym-module.h"
#include "ns3/net-device-container.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/nr-mac-scheduler-tdma.h"
#include "ns3/nr-mac-scheduler-ns3.h"
#include "ns3/nr-mac-scheduler-ofdma.h"
#include "ns3/log.h"

namespace ns3{
    extern NetDeviceContainer enbNetDev;
    // extern NetDeviceContainer ueNetDev;
    extern NetDeviceContainer eMBBNetDev1;
    extern NetDeviceContainer mMTCNetDev1;
    extern NetDeviceContainer URLLCNetDev1;

    extern NetDeviceContainer eMBBNetDev2;
    extern NetDeviceContainer mMTCNetDev2;
    extern NetDeviceContainer URLLCNetDev2;

    extern NodeContainer gNbNodes;
    // extern NodeContainer ueNodes;
    extern NodeContainer eMBBueNodes1;
    extern NodeContainer mMTCueNodes1;
    extern NodeContainer URLLCueNodes1;

    extern NodeContainer eMBBueNodes2;
    extern NodeContainer mMTCueNodes2;
    extern NodeContainer URLLCueNodes2;
    // extern GridScenarioHelper gridScenario;

    extern std::unordered_map<int, float> ue_metric;

    extern uint64_t randomStream;
    
    extern uint32_t gNbNum;
    extern uint32_t ueNum;
    extern double gNbHeight;
    extern double ueHeight;

    extern uint32_t embbPacketSize1;
    extern uint32_t mmtcPacketSize1;
    extern uint32_t urllcPacketSize1;

    extern uint32_t embbPacketSize2;
    extern uint32_t mmtcPacketSize2;
    extern uint32_t urllcPacketSize2;

    extern uint32_t simTimeMs;
    extern uint32_t envStepTime;
    extern uint32_t StartTimeMs;
    extern double centralFrequencyBand;
    extern double bandwidthBand1 ;
    //double totalTxPower = 4;
    extern uint16_t PorteMBB1;
    extern uint16_t PortmMTC1;
    extern uint16_t PortURLLC1;

    extern uint16_t PorteMBB2;
    extern uint16_t PortmMTC2;
    extern uint16_t PortURLLC2;

    extern uint32_t simSeed;
    extern uint32_t testArg;

    extern double averageFlowThroughput;
    extern double averageFlowDelay;

    extern double embbthruput;
    extern double embbdelay;

    extern double urllcthruput;
    extern double urllcdelay;
    extern double urllcreliability; 

    extern double mmtcthruput;
    extern double mmtcdelay;

    extern FlowMonitorHelper flowmonHelper;
    extern Ptr<ns3::FlowMonitor> monitor;
    extern NodeContainer endpointNodes;

    extern Ptr<OpenGymInterface> openGym;
    extern std::ofstream myfile;
    extern std::ofstream embbfile;
    extern std::ofstream urllcfile;
    extern std::ofstream mmtcfile;
    extern std::ofstream rewardfile;

    void flow_monitor();
    void reset_flow();
    void save_data();
    // static TypeId GetTypeId (void);
    Ptr<OpenGymSpace> GetActionSpace();
    Ptr<OpenGymSpace> GetObservationSpace();
    Ptr<OpenGymDataContainer> GetObservation();
    float GetReward(void);
    bool GetGameOver(void);
    bool ExecuteActions(Ptr<OpenGymDataContainer> action);
    std::string GetExtraInfo(void);
    void ScheduleNextStateRead();

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


    class CustomScheduler : public NrMacSchedulerNs3
    {
    public:
      
        static TypeId GetTypeId();

        CustomScheduler();
        
        ~CustomScheduler() override
        {
        }

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
        GetUeCompareDlFn() const;

        /**
         * \brief Return the comparison function to sort UL UE according to the scheduler policy
         * \return a pointer to NrMacSchedulerUeInfoRR::CompareUeWeightsUl
         */
        std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                        const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
        GetUeCompareUlFn() const;

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
                                const FTResources& totAssigned) const;

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
                                const FTResources& totAssigned) const;

        // RR is a simple scheduler: it doesn't do anything in the next
        // inherited calls.
        void NotAssignedDlResources(const UePtrAndBufferReq& ue,
                                    const FTResources& notAssigned,
                                    const FTResources& totalAssigned) const
        {
        }

        void NotAssignedUlResources(const UePtrAndBufferReq& ue,
                                    const FTResources& notAssigned,
                                    const FTResources& totalAssigned) const
        {
        }

        void BeforeDlSched(const UePtrAndBufferReq& ue,
                        const FTResources& assignableInIteration) const
        {
        }

        void BeforeUlSched(const UePtrAndBufferReq& ue,
                        const FTResources& assignableInIteration) const
        {
        }

        // Advanced part
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

        /**
         * \brief Advance the starting point by the number of symbols specified,
         * resetting the RB count to 0
         * \param spoint Starting point
         * \param symOfBeam Number of symbols for the beam
         */
        void ChangeDlBeam(PointInFTPlane* spoint, uint32_t symOfBeam) const override;

        void ChangeUlBeam(PointInFTPlane* spoint, uint32_t symOfBeam) const override;

        CustomScheduler::BeamSymbolMap GetSymPerBeam(uint32_t symAvail,
                                                        const ActiveUeMap& activeDl) const;

        uint8_t GetTpc() const override;

    private:
        TracedValue<uint32_t> m_tracedValueSymPerBeam;
        static std::vector<UePtrAndBufferReq> GetUeVectorFromActiveUeMap(const ActiveUeMap& activeUes);
        typedef std::function<void(const UePtrAndBufferReq&, const FTResources&)>
        BeforeSchedFn; //!< Before scheduling function
        /**
         * \brief //!< Function to notify a successfull assignment
         */
        typedef std::function<void(const UePtrAndBufferReq&, const FTResources&, const FTResources&)>
            AfterSuccessfullAssignmentFn;
        /**
         * \brief Function to notify that the UE did not get any resource in one iteration
         */
        typedef std::function<void(const UePtrAndBufferReq&, const FTResources&, const FTResources&)>
            AfterUnsucessfullAssignmentFn;
        typedef std::function<uint32_t&(const UePtr& ue)> GetRBGFn; //!< Getter for the RBG of an UE
        typedef std::function<uint32_t(const UePtr& ue)> GetTBSFn;  //!< Getter for the TBS of an UE
        typedef std::function<uint8_t&(const UePtr& ue)>
            GetSymFn; //!< Getter for the number of symbols of an UE
        typedef std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                                const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
            CompareUeFn;
        typedef std::function<CompareUeFn()> GetCompareUeFn;

        // BeamSymbolMap AssignRBGTDMA(
        //     uint32_t symAvail,
        //     const ActiveUeMap& activeUe,
        //     const std::string& type,
        //     const BeforeSchedFn& BeforeSchedFn,
        //     const GetCompareUeFn& GetCompareFn,
        //     const GetTBSFn& GetTBSFn,
        //     const GetRBGFn& GetRBGFn,
        //     const GetSymFn& GetSymFn,
        //     const AfterSuccessfullAssignmentFn& SuccessfullAssignmentFn,
        //     const AfterUnsucessfullAssignmentFn& UnSuccessfullAssignmentFn) const;

        std::shared_ptr<DciInfoElementTdma> CreateDci(
            PointInFTPlane* spoint,
            const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
            std::vector<uint32_t> tbs,
            DciInfoElementTdma::DciFormat fmt,
            std::vector<uint8_t> mcs,
            std::vector<uint8_t> ndi,
            std::vector<uint8_t> rv,
            uint8_t numSym) const;
        
        float default_metric{0.0};
        
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
            // return (lue.first->m_dlRBG < rue.first->m_dlRBG);
            return (ue_metric[lue.first->m_rnti] < ue_metric[rue.first->m_rnti]);
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
