#   Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
#   Purpose: To process the C++ simulation output into graphs.

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import projections
from os import getcwd

# input sources & output destinations
input = ["data/data1.csv","data/data2.csv","data/data4.csv","data/data8.csv","data/data16.csv","data/data32.csv"]
output = ["imgs/graph1-0.png","imgs/graph1-1.png","imgs/graph1-2.png","imgs/graph1-3.png","imgs/graph1-4.png",
          "imgs/graph2-0.png","imgs/graph2-1.png","imgs/graph2-2.png","imgs/graph2-3.png","imgs/graph2-4.png",
          "imgs/graph4-0.png","imgs/graph4-1.png","imgs/graph4-2.png","imgs/graph4-3.png","imgs/graph4-4.png",
          "imgs/graph8-0.png","imgs/graph8-1.png","imgs/graph8-2.png","imgs/graph8-3.png","imgs/graph8-4.png",
          "imgs/graph16-0.png","imgs/graph16-1.png","imgs/graph16-2.png","imgs/graph16-3.png","imgs/graph16-4.png",
          "imgs/graph32-0.png","imgs/graph32-1.png","imgs/graph32-2.png","imgs/graph32-3.png","imgs/graph32-4.png",]
outputPos = 0
C_MAX_NUM_PROCESS = 2048
C_MAX_PROCESS_LEN = 512

for input_file in input:
    for algo in range (5): # algo: scheduling algorithm used (values [0-4])
        data = pd.read_csv(input_file, header = 0)

        numProc = data[data['Algorithm'] == algo]
        numProc = numProc['Number_of_Processes'].values.tolist()

        procLen = data[data['Algorithm'] == algo]
        procLen = procLen['Avg_Process_Length'].values.tolist()
        
        starvations = data[data['Algorithm'] == algo]
        starvations = starvations['Starvations'].values.tolist()

        # converting number of starvations to a percentage starvations
        for x in range(len(starvations)):
            starvations[x] = round((starvations[x]*100/numProc[x]),3)

        # creating the graph
        axis = plt.axes(projection = '3d')
        axis.plot_trisurf(numProc, procLen, starvations, cmap=plt.cm.Spectral, linewidth=0.2)
        
        # graph stylization
        axis.set_xlabel("Number of Processes", fontsize = 12)
        axis.set_ylabel("Avg. Process Length", fontsize = 12)
        axis.set_zlabel("Percentage Starved (%)", fontsize = 12)
        axis.set_xticks([0, 512, 1024, 1536, C_MAX_NUM_PROCESS])
        axis.set_yticks([0, 128, 256, 384, C_MAX_PROCESS_LEN])
        axis.set_zticks([0,25,50,75,100])
        axis.view_init(15, -170)

        # saving the image ouput to the imgs folder
        plt.savefig(output[outputPos])
        axis.set_xlabel("")
        axis.set_ylabel("")
        axis.set_zlabel("")
        axis.set_xticks([])
        axis.set_yticks([])
        axis.set_zticks([])
        outputPos += 1