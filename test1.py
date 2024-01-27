#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
from ns3gym import ns3env
import gym

import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

__author__ = "Piotr Gawlowicz"
__copyright__ = "Copyright (c) 2018, Technische Universität Berlin"
__version__ = "0.1.0"
__email__ = "gawlowicz@tkn.tu-berlin.de"


parser = argparse.ArgumentParser(description='Start simulation script on/off')
parser.add_argument('--start',
                    type=int,
                    default=1,
                    help='Start ns-3 simulation script 0/1, Default: 1')
parser.add_argument('--iterations',
                    type=int,
                    default=1,
                    help='Number of iterations, Default: 1')
args = parser.parse_args()
startSim = bool(args.start)
iterationNum = int(args.iterations)

port =1111
simTime = 2000 # seconds
stepTime = 200  # seconds
seed = 0
simArgs = {"--simTime": simTime,
           "--testArg": 123}
debug = False
#rewards = []
returns = []
values = []
gamma = 0.85
trace_lambda = 0.5
td_target = 0
trace_size = 16
alpha = 0.05

#values: predicted rewards by value network
#returns: calculated discounted returns
#advantages: returns - values
#advantages = sum of future discounted rewards

env = ns3env.Ns3Env(port=port, stepTime=stepTime, startSim=startSim, simSeed=seed, simArgs=simArgs, debug=debug)
# env = gym.make('ns3-v0')
# simpler:
#env = ns3env.Ns3Env()
env.reset()

# set up matplotlib
is_ipython = 'inline' in matplotlib.get_backend()
if is_ipython:
    from IPython import display

plt.ion()

# if GPU is to be used
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

ob_space = env.observation_space
ac_space = env.action_space
print("Observation space: ", ob_space,  ob_space.dtype)
print("Action space: ", ac_space, ac_space.dtype)

stepIdx = 0
currIt = 0
total_episodes = 200

#design a LStm Network as the actor network
class ActorNetwork(nn.Module):

    def __init__(self,input_size,hidden_size,action_size):
        super(ActorNetwork, self).__init__()
        self.input_size = input_size
        self.hidden_size = hidden_size
        self.action_size = action_size
        self.rnn1 = nn.LSTM(self.input_size, self.hidden_size, self.action_size, bidirectional=True)
        self.rnn2 = nn.LSTM(2, self.hidden_size, self.action_size, bidirectional=False)

    def forward(self,x):
        x = x.cuda()
        x, (_, _) = self.rnn1(x)
        x, (_, _) = self.rnn2(x)
        #attention mechanism
        y = F.softmax(x, dim = 0)
        x = torch.div(x,y)
        return x
    
actor = ActorNetwork(1, 1, 1)
actor = actor.to(device)
    
class ValueNetwork(nn.Module):

    def __init__(self,input_size,hidden_size,action_size):
        super(ValueNetwork, self).__init__()
        self.input_size = input_size
        self.hidden_size = hidden_size
        self.action_size = action_size
        self.rnn1 = nn.LSTM(self.input_size, self.hidden_size, self.action_size, bidirectional=True)
        self.rnn2 = nn.LSTM(2, self.hidden_size, self.action_size, bidirectional=False)
        self.rnn3 = nn.LSTM(self.input_size, self.hidden_size, self.action_size, bidirectional=False)
        self.linear = nn.Linear(2,1)

    def forward(self,x):
        x = x.cuda()
        x, (_, _) = self.rnn1(x)
        _, (h, _) = self.rnn2(x)
        return h.data

value = ValueNetwork(1, 1, 1)
value = value.to(device)

def roll_out(actor_network,task,sample_nums,value_network,init_state):
    #task.reset()
    states = []
    actions = []
    rewards = []
    is_done = False
    final_r = 0
    state = init_state


'''class A2C():
    def __init__(self):
        super(A2C, self).__init__()
        self.actor = ActorNetwork(1, 1, 1)
        self.value = ValueNetwork(1, 1, 1)
        self.value_network_optimizer = optim.RMSprop(self.value.parameters(), lr=0.0001)
        self.actor_network_optimizer = optim.RMSprop(self.actor.parameters(), lr=0.0001)
        self.criterion = nn.CrossEntropyLoss()
    
    def learn(self, obs, reward):
        rewards.append(reward)
        returns.append(reward + gamma * values[-1])
        v = value(obs_tensor).detach()
        values.append(v)'''



#value_criterion = nn.CrossEntropyLoss()
value_criterion = nn.MSELoss().to(device)
value_network_optimizer = optim.RMSprop(value.parameters(), lr=1e-4)

#actor_criterion = nn.CrossEntropyLoss()
actor_criterion = nn.MSELoss().to(device)
actor_network_optimizer = optim.RMSprop(actor.parameters(), lr=1e-4)

try:
    while True:
        cumu_v = 0
        print("Start iteration: ", currIt)
        obs = env.reset()
        print("Step: ", stepIdx)
        print("---obs:", obs)

        obs_tensor = torch.tensor(obs, dtype=torch.float)
        obs_tensor = torch.reshape(obs_tensor, (obs_tensor.shape[0],1))

        action = actor(obs_tensor)
        #action = actor(obs_tensor).cpu()
       
        v = value(obs_tensor)
        #v = value(obs_tensor).cpu()
        #values.append(v)

        while True:
            stepIdx += 1

            print("Step: ", stepIdx)
            #obs, reward, done, info = env.step(env.action_space.sample())
            #info of next state (can be used to calculate the predicted value of the next state) (value network)
            obs, reward, done, info = env.step(action)
            obs_tensor = torch.tensor(obs, dtype=torch.float)
            obs_tensor = torch.reshape(obs_tensor, (obs_tensor.shape[0],1))
            
            td_target += (reward + v * gamma)
            #td_target = torch.tensor(td_target, dtype=torch.float).requires_grad_(True)
            #td_target = td_target.to(device)
            
            #pred_v = values[-1]
            v = value(obs_tensor)
            
            print("--value: ", v)
            

            cumu_v += v

            trace = torch.tensor(cumu_v/(currIt + 1), dtype=torch.float)
            trace.to(device)
            rewards = torch.tensor(td_target/(currIt + 1), dtype=torch.float)
            rewards.to(device)
            

            #values.append(v)

            #if currIt % 15 == 0 and currIt != 0:

                
            #trace = np.zeros([trace_size])
            #rewards = np.zeros([trace_size])

            actor_loss = actor_criterion(trace, rewards)
            value_loss = value_criterion(trace, rewards)

            
            #optimize actor network(based on advantage)
            with torch.autograd.set_detect_anomaly(True):
                actor_network_optimizer.zero_grad()
                actor_loss.backward(retain_graph=True)
                actor_network_optimizer.step()

            #optimize actor network(based on td error)
            with torch.autograd.set_detect_anomaly(True):
                value_network_optimizer.zero_grad()
                value_loss.backward()
                value_network_optimizer.step()
            
            print("---obs, reward, done, info: ", obs, reward, done, info)
    
            action = actor(obs_tensor)
            
            print("---action: ", action)
            
            if done:
                stepIdx = 0
                if currIt + 1 < iterationNum:
                    env.reset()
                break

        currIt += 1
        if currIt == iterationNum:
            break

except KeyboardInterrupt:
    print("Ctrl-C -> Exit")
finally:
    env.close()
    print("Done")
