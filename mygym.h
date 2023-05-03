#ifndef MY_GYM_ENTITY_H
#define MY_GYM_ENTITY_H

#include "ns3/opengym-module.h"
#include "ns3/net-device-container.h"

namespace ns3{
    
    class MyGym : public OpenGymEnv
    {
    private:
        /* data */
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
