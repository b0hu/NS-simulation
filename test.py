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
rewards = []
returns = []
values = []
gamma = 0.85
td_target = 0

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

'''class DeepIO(nn.Module):
    def __init__(self):
        super(DeepIO, self).__init__()
        self.rnn = nn.LSTM(input_size=6, hidden_size=512,
                           num_layers=2, bidirectional=True)
        self.drop_out = nn.Dropout(0.25)
        self.fc1 = nn.Linear(512, 256)
        self.bn1 = nn.BatchNorm1d(256)
        self.fc_out = nn.Linear(256, 7)

    def forward(self, x):
        """
        args:
        x:  a list of inputs of diemension [BxTx6]
        """
        outputs = []
        # iterate in the batch through all sequences
        for xx in x:
            s, n = xx.shape
            out, hiden = self.rnn(xx.unsqueeze(1))
            out = out.view(s, 1, 2, 512)
            out = out[-1, :, 0]
            outputs.append(out.squeeze())
        outputs = torch.stack(outputs)

        y = F.relu(self.fc1(outputs), inplace=True)
        y = self.bn1(y)
        y = self.drop_out(y)
        y = self.out(y)
        return y

def edit_data(data):
    data = torch.tensor(data, dtype=torch.float)
    data = torch.reshape(data, (data.shape[0],1))
    return data'''

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
        #h = tuple([h.data])
        #x, (_, _) = self.rnn3(h)
        #_, (h, _) = self.rnn3(x)
        #y = x.reshape(1, x.shape[0]).clone()
        #x = x.view(1, x.shape[0]).clone()
        #x = torch.cat([x, torch.zeros(1, 20 - x.shape[1])], dim = 1)
        #pad = nn.ZeroPad2d((0,15 - x.shape[1], 0, 0))
        #x = pad(x)
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


class A2C():
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
        #advantage = returns[-1] - values[-1]

        v = value(obs_tensor).detach()
        values.append(v)



#value_criterion = nn.CrossEntropyLoss().to(device)
value_criterion = nn.CrossEntropyLoss().to(device)
#value_network_optimizer = optim.RMSprop(value.parameters(), lr=1e-4)
value_network_optimizer = optim.Adam(value.parameters(), lr=1e-4)

#actor_criterion = nn.CrossEntropyLoss().to(device)
actor_criterion = nn.CrossEntropyLoss().to(device)
#actor_network_optimizer = optim.RMSprop(actor.parameters(), lr=1e-4)
actor_network_optimizer = optim.Adam(actor.parameters(), lr=1e-4)

try:
    while True:
        print("Start iteration: ", currIt)
        obs = env.reset()
        print("Step: ", stepIdx)
        print("---obs:", obs)

        #actor = actor.train()
        #value = value.train()
        obs_tensor = torch.tensor(obs, dtype=torch.float)
        obs_tensor = torch.reshape(obs_tensor, (obs_tensor.shape[0],1))     

        action = actor(obs_tensor)
        #action = actor(obs_tensor).cpu()
        #obs_tensor = torch.cat([obs_tensor, torch.zeros(1, 20 - obs_tensor.shape[1])], dim = 0)
        #x = torch.cat([obs_tensor, torch.zeros(20 - obs_tensor.shape[0], 1)], dim = 0)
        v = value(obs_tensor)
        #v = value(obs_tensor).cpu()
        values.append(v)

        while True:
            stepIdx += 1

            print("Step: ", stepIdx)
            #obs, reward, done, info = env.step(env.action_space.sample())
            #info of next state (can be used to calculate the predicted value of the next state) (value network)
            obs, reward, done, info = env.step(action)
            obs_tensor = torch.tensor(obs, dtype=torch.float)
            obs_tensor = torch.reshape(obs_tensor, (obs_tensor.shape[0],1))

            '''obs_tensor = torch.empty([0], dtype = torch.float32)

            for i in range(len(obs)):
                obs_tensor = torch.cat([obs_tensor, torch.Tensor([obs[i]])])'''

            rewards.append(reward)
            #value_loss = value_criterion(torch.tensor(v, dtype=torch.float), td_target)

            pred_v = values[-1]
            v = value(obs_tensor)
            #v = value(obs_tensor).cpu()
            
            print("--value: ", v)
            #print("--shape of value: ", v.shape)
            #advantage = reward + gamma * v - values[-1]
            td_target = reward + gamma * v
            td_target = torch.tensor(td_target, dtype=torch.float).requires_grad_(True)

            #actor_loss = actor_criterion(values[-1], td_target)
            actor_loss = actor_criterion(pred_v, td_target)
            value_loss = value_criterion(pred_v, td_target)

            values.append(v)

            '''returns.append(values[-1])
            for i in reversed(range(1, len(rewards))):
                returns[i-1] = rewards[i-1] + gamma * returns[i]
            returns = returns[:-1]'''
            
            '''for i in returns:
                td_target = td_target + i'''

            #advantage = returns[-1] - values[-1]
            #td_error
            #value_loss = actor_criterion(returns[-1], values[-1])
            
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
            #action = actor(obs_tensor).cpu()
            
            #temp = torch.cat([obs_tensor, action],dim = 1)
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
