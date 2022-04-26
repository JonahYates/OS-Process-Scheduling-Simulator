# Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
# File: graph.py
# Purpose: To manipulate the data output by the c++ process simulation
# program into graphs that can be viewed visually.

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import projections
from os import getcwd

# input sources & output destinations
input = ["data/data1.csv","data/data2.csv","data/data4.csv","data/data8.csv","data/data12.csv","data/data16.csv"]

output = ["imgs/graph1-0.png","imgs/graph1-1.png","imgs/graph1-2.png","imgs/graph1-3.png","imgs/graph1-4.png",
          "imgs/graph2-0.png","imgs/graph2-1.png","imgs/graph2-2.png","imgs/graph2-3.png","imgs/graph2-4.png",
          "imgs/graph4-0.png","imgs/graph4-1.png","imgs/graph4-2.png","imgs/graph4-3.png","imgs/graph4-4.png",
          "imgs/graph8-0.png","imgs/graph8-1.png","imgs/graph8-2.png","imgs/graph8-3.png","imgs/graph8-4.png",
          "imgs/graph12-0.png","imgs/graph12-1.png","imgs/graph12-2.png","imgs/graph12-3.png","imgs/graph12-4.png",
          "imgs/graph16-0.png","imgs/graph16-1.png","imgs/graph16-2.png","imgs/graph16-3.png","imgs/graph16-4.png",]
outputNum = 0

for x in range (6):         # values 0-5
    for y in range (5):     # values 0-4

        # getting the data
        data = pd.read_csv(input[x], header = 0)

        numProc = data[data['Algorithm'] == y]
        numProc = numProc['Number of Processes']

        procLen = data[(data['Algorithm'] == y)]
        procLen = procLen['Average Process Length']
        
        starvations = data[(data['Algorithm'] == y)]
        starvations = starvations['Number of Starvations']

        # creating the graph
        axis = plt.axes(projection = '3d')
        axis.plot_trisurf(numProc, procLen, starvations, cmap=plt.cm.Spectral, linewidth=0.2)
        
        # graph stylization
        axis.set_xlabel("")
        axis.set_ylabel("")
        axis.set_xticks([])
        axis.set_yticks([])
        axis.set_zlabel("Number of Starvations", fontsize = 12)
        axis.set_zticks([0,512,1024,1536,2048])
        axis.view_init(15, -170)

        # saving the image ouput to the imgs folder
        plt.savefig(output[outputNum])
        axis.set_zlabel("")
        axis.set_zticks([])
        outputNum += 1