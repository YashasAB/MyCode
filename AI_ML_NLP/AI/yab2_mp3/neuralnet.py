# neuralnet.py
# ---------------
# Licensing Information:  You are free to use or extend this projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to the University of Illinois at Urbana-Champaign
#
# Created by Justin Lizama (jlizama2@illinois.edu) on 10/29/2019
# Modified by Mahir Morshed for the spring 2021 semester

"""
This is the main entry point for MP3. You should only modify code
within this file and neuralnet_part2.py -- the unrevised staff files will be used for all other
files and classes when code is run, so be careful to not modify anything else.
"""

import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim

class1 = 3
class2 =13


class NeuralNet(nn.Module):
    def __init__(self, lrate, loss_fn, in_size, out_size):
        """
        Initializes the layers of your neural network.

        @param lrate: learning rate for the model
        @param loss_fn: A loss function defined as follows:
            @param yhat - an (N,out_size) Tensor
            @param y - an (N,) Tensor
            @return l(x,y) an () Tensor that is the mean loss
        @param in_size: input dimension
        @param out_size: output dimension

        For Part 1 the network should have the following architecture (in terms of hidden units):

        in_size -> 32 ->  out_size

        We recommend setting lrate to 0.01 for part 1.

        """
        super(NeuralNet, self).__init__()


        self.lrate = lrate
        self.loss_fn = loss_fn

        self.model = nn.Sequential(

            nn.Conv2d(3, 12, (4,4), stride=1, padding=1),

            nn.ReLU(),

            nn.Conv1d(12,6 ,(2,2), stride=1, padding=1),

            nn.Flatten(),

            nn.Linear(6144, out_size),

        )

        self.optimizer =  optim.SGD(self.model.parameters(), lr=lrate)


    def set_parameters(self, params):
        """ Sets the parameters of your network.

        @param params: a list of tensors containing all parameters of the network
        """
        # self.W1 = params[0]
        # self.W2 = params[1]
        # self.b1 = params[2]
        # self.b2 = params[3]
        return


    def get_parameters(self):
        """ Gets the parameters of your network.

        @return params: a list of tensors containing all parameters of the network
        """
        return self.model.parameters()


    def forward(self, x):
        """Performs a forward pass through your neural net (evaluates f(x)).

        @param x: an (N, in_size) Tensor
        @return y: an (N, out_size) Tensor of output from the network
        """

        lol = nn.Dropout(p= 0.05)

        x = lol(x)

        #reshape
        x = x.view(-1, 3,32,32)

        return self.model(x)


    def step(self, x,y):
        """
        Performs one gradient step through a batch of data x with labels y.

        @param x: an (N, in_size) Tensor
        @param y: an (N,) Tensor
        @return L: total empirical risk (mean of losses) at this timestep as a float

        """


        predict = self.forward(x)

        loss = self.loss_fn(predict, y)

        loss.backward()

        self.optimizer.step()

        return loss.detach().cpu().numpy()


def fit(train_set,train_labels,dev_set,n_iter,batch_size=100):
    """ Make NeuralNet object 'net' and use net.step() to train a neural net
    and net(x) to evaluate the neural net.

    @param train_set: an (N, in_size) Tensor
    @param train_labels: an (N,) Tensor
    @param dev_set: an (M,) Tensor
    @param n_iter: an int, the number of iterations of training
    @param batch_size: size of each batch to train on. (default 100)

    This method _must_ work for arbitrary M and N.

    @return losses: array of total loss at the beginning and after each iteration.
            Ensure that len(losses) == n_iter.
    @return yhats: an (M,) NumPy array of binary labels for dev_set
    @return net: a NeuralNet object
    """

    losses = []
    insize = train_set.size()

    NET = NeuralNet(0.02, nn.CrossEntropyLoss(), insize, 2)

    train_set = (train_set - np.mean(train_set.numpy()))/np.std(train_set.numpy())

    dev_set = (dev_set - np.mean(dev_set.numpy()))/np.std(dev_set.numpy())

    for epoch in range(0,5):
        iterloss = 0
        for n in range(n_iter):
            NET.optimizer.zero_grad()
            X = train_set[n * batch_size: (n + 1) * batch_size]
            Y = train_labels[n * batch_size: (n + 1) * batch_size]
            l = NET.step(X,Y)
            iterloss+=l
            losses.append(iterloss)

    dev_set = (dev_set - np.mean(dev_set.numpy()))/np.std(dev_set.numpy())
    pred = NET.forward(dev_set).detach().cpu().numpy()

    ypred = np.zeros(dev_set.shape[0])
    for i in range(0, pred.shape[0]):
        ypred[i]=np.argmax(pred[i])

    return (np.array(losses[-n_iter:]),ypred,NET)
