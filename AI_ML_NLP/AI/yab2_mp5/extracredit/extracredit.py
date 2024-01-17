import torch, random, math, json
import torch.nn as nn
import torch.optim as optim
import numpy as np
from extracredit_embedding import ChessDataset, initialize_weights

DTYPE=torch.float32
DEVICE=torch.device("cpu")

###########################################################################################
def trainmodel():
    # Well, you might want to create a model a little better than this...
    model = torch.nn.Sequential(
    torch.nn.Flatten(),

    torch.nn.Linear(8*8*15, 1024),

    nn.LeakyReLU(),

    # torch.nn.Linear(256, 64),
    # #
    # nn.Sigmoid(),

    # torch.nn.Linear(256, 64),
    #
    torch.nn.Linear(1024,256),

    nn.ReLU(),

    torch.nn.Linear(256,64),

    nn.LeakyReLU(),

    torch.nn.Linear(64,1)

    )

    lossfn = nn.MSELoss()

    lrate = 0.0001

    opt =  optim.SGD(model.parameters(), lr=lrate)

    # ... and if you do, this initialization might not be relevant any more ...
    # model[1].weight.data = initialize_weights()
    # model[1].bias.data = torch.zeros(1)

    # ... and you might want to put some code here to train your model:
    lol = nn.Dropout(p= 0.005)
    trainset = ChessDataset(filename='extracredit_train.txt')
    trainloader = torch.utils.data.DataLoader(trainset, batch_size=1000, shuffle=True)

    sched = 90

    for epoch in range(8000):

        for x,y in trainloader:
            predict = model(x)
            loss = lossfn(predict, y)
            if epoch%100==0:
                print('loss is  ', loss)
            # if loss < 50:
            #     for g in opt.param_groups:
            #         g['lr'] = 0.000001
            # if loss < 40:
            #     for g in opt.param_groups:
            #         g['lr'] = 0.0000001

            loss.backward()
            opt.step()


    # trainset = ChessDataset(filename='extracredit_validation.txt')
    # trainloader = torch.utils.data.DataLoader(trainset, batch_size=1000, shuffle=True)
    # for epoch in range(2000):
    #     for x,y in trainloader:
    #         predict = model(x)
    #         loss = lossfn(predict, y)
    #         loss.backward()
    #         opt.step()

    # ... after which, you should save it as "model.pkl":
    torch.save(model, 'model.pkl')


###########################################################################################
if __name__=="__main__":
    trainmodel()
