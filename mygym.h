#ifndef MY_GYM_ENTITY_H
#define MY_GYM_ENTITY_H

#include "ns3/opengym-module.h"
#include "ns3/nr-module.h"

namespace ns3{
    class MyGym : public OpenGymEnv
    {
    private:
        /* data */
        TypeId GetTypeId ();
        Ptr<OpenGymSpace> GetActionSpace();
        Ptr<OpenGymSpace> GetObservationSpace();
        Ptr<OpenGymSpace> GetObservation();
        float GetReward(void);
        bool GetGameOver(void);
        bool ExecuteActions(Ptr<OpenGymDataContainer> action);
        std::string GetExtraInfo(void);
    public:
        MyGym(/* args */);
        ~MyGym();
    };
    
}

#endif