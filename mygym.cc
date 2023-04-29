#include "mygym.h"
#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/node-list.h"
#include "ns3/log.h"
#include <iostream>

namespace ns3{

    NS_LOG_COMPONENT_DEFINE ("MyGym");

    NS_OBJECT_ENSURE_REGISTERED (MyGym);

    MyGym::MyGym (/* args */){
        NS_LOG_FUNCTION (this);
    }
    MyGym::~MyGym (){
        NS_LOG_FUNCTION (this);
    }

    TypeId MyGym::GetTypeId (){
        static TypeId tid = TypeId ("MyGymEnv").SetParent<OpenGymEnv> ().SetGroupName ("OpenGym").AddConstructor<MyGymEnv> ();
        return tid;
    }
    /*
    Define observation space
    */
    Ptr<OpenGymSpace>
    MyGym::GetObservation()
    {
    uint32_t nodeNum = 4;
    float low = 0.0;
    float high = 10.0;
    std::vector<uint32_t> shape = {nodeNum,};
    std::string dtype = TypeNameGet<uint32_t> ();
    Ptr<OpenGymBoxSpace> space = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);
    NS_LOG_UNCOND ("MyGetObservationSpace: " << space);
    return space;
    }

    /*
    Define action space
    */
    Ptr<OpenGymSpace>
    MyGym::GetActionSpace(void)
    {
    uint32_t nodeNum = 4;

    Ptr<OpenGymDiscreteSpace> space = CreateObject<OpenGymDiscreteSpace> (nodeNum);
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
    uint32_t nodeNum = 4;
    uint32_t low = 0.0;
    uint32_t high = 10.0;
    Ptr<UniformRandomVariable> rngInt = CreateObject<UniformRandomVariable> ();

    std::vector<uint32_t> shape = {nodeNum,};
    Ptr<OpenGymBoxContainer<uint32_t> > box = CreateObject<OpenGymBoxContainer<uint32_t> >(shape);

    // generate random data
    for (uint32_t i = 0; i<nodeNum; i++){
        uint32_t value = rngInt->GetInteger(low, high);
        box->AddValue(value);
    }

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

    void ScheduleNextStateRead(double envStepTime, Ptr<OpenGymInterface> openGym)
    {
    Simulator::Schedule (MilliSeconds(envStepTime), &ScheduleNextStateRead, envStepTime, openGym);
    openGym->NotifyCurrentState();
    }
    
}