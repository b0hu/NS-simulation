#ifndef MY_GYM_ENTITY_H
#define MY_GYM_ENTITY_H

#include "ns3/opengym-module.h"
#include "ns3/nr-module.h"

namespace ns3{
    class MyGym : public OpenGymEnv
    {
    private:
        /* data */
    public:
        MyGym(/* args */);
        ~MyGym();
    };
    
}

#endif