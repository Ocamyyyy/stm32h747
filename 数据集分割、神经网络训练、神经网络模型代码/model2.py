import torch
from torch import nn
import torch.nn.functional as F

class MyAlexNet(nn.Module):
    """Feedfoward neural network with 1 hidden layer"""

    def __init__(self):
        super().__init__()
        self.cn1 = nn.Conv2d(3, 3, 1, 1,0)
        self.relu2 = nn.ReLU()
        self.max3=nn.MaxPool2d(2, 2)
        self.cn4=nn.Conv2d(3, 3, 3, 1,1)
        self.relu5=nn.ReLU()
        self.max6=nn.MaxPool2d(2, 2)
        self.flatten = nn.Flatten()
        self.fc7=nn.Linear(1200,16)
        self.norm8=nn.BatchNorm1d(16)
        self.relu9=nn.ReLU()
        self.fc10 = nn.Linear(16,6)

    def forward(self, xb):
        x=self.cn1(xb)
        x=self.relu2(x)
        x=self.max3(x)
        x=self.cn4(x)
        x=self.relu5(x)
        x=self.max6(x)
        x=self.flatten(x)
        x=self.fc7(x)
        x=self.norm8(x)
        x=self.relu9(x)
        x=self.fc10(x)
        return x