/*  Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
    File: main.cpp
    Purpose: simulates process scheduling algorithms by generating
            randomized processes, scheduling them, and then outputting
            starvation count metrics to the designated file. */

#include <iostream>
#include <fstream>
#include <random>
#include <time.h>
#include <cmath>
#include <thread>
#include <mutex>
#include "process.h"
#include "scheduler.h"

using namespace std;

/* Global Constants */
const vector<string> C_OUTPUT_FILES = {
    "data/data1.csv",
    "data/data2.csv",
    "data/data4.csv",
    "data/data8.csv",
    "data/data16.csv",
    "data/data32.csv"};

const short C_NUM_ALGORITHMS = 5;

const short C_MAX_CORES = 32;
const short C_MIN_CORES = 1;

    /*
const vector<int> C_NUM_PROC_TESTING = {
    4, 8, 16, 32, 64, 128, 256, 512, 768, 1024, 1280, 1536, 1792, 2048}

const vector<int> C_PROC_LEN_TESTING = {
    4, 8, 16, 32, 64, 128, 192, 256, 320, 384, 448, 512}
*/

const short C_MAX_NUM_PROCESS = 2048;       // maximum number of processes needing to be ran
const short C_MAX_PROCESS_LEN = 512;        // maximum time-units an individual process can be 

const short C_MAX_NUM_IOEVENTS = 12;
const short C_MEAN_NUM_IOEVENTS = -2;
const short C_STDDEV_IOEVENTS = 5;

const short C_MEAN_IOEVENT_LEN = 15;
const short C_STDDEV_IOEVENT_LEN = 25;

/* Global Variables */
mutex outputMutex;
ofstream fout;

int main() {
    /* Variable Declaration Section */
    default_random_engine generator;
    vector<Process> processList;
    vector<thread> threadList;
    
    long pTime;                             // simulated computer time
    int numStarvations;                     // number of processes that starved (Not finished in 4*reqTime + numIOEvents*meanIOLen)

    int stdDevProcLen;                      // standard deviation for number of processes
    int randNum;
    short quanta;                           // time quanta for RR
    short outputPos = 0;

    generator.seed(time(NULL));             // seeding distribution generator
    srand(time(NULL));                      // seeding random number generator

    /*  LC_cores         - number of cores//processors available
        LC_numProcesses  - number of processes needing to be run
        LC_processLen    - average length of processes            */ 

    for (short LC_cores = C_MIN_CORES; LC_cores <= C_MAX_CORES; LC_cores*=2) {
        fout.open(C_OUTPUT_FILES[outputPos]);
        fout<<"Number_of_Cores,Number_of_Processes,Avg_Process_Length,Algorithm,Starvations"<<endl;
        for (int LC_numProcesses = 4; LC_numProcesses <= C_MAX_NUM_PROCESS; LC_numProcesses*=2) {
            for (int LC_processLen = 4; LC_processLen <= C_MAX_PROCESS_LEN; LC_processLen*=2) {
                for (int LC_algorithm = 0; LC_algorithm < C_NUM_ALGORITHMS; LC_algorithm++) {
                    // clearing process list, re-creating random processes and resetting pTime & numStarvations 
                    processList.clear();
                    pTime = 1;
                    numStarvations = 0;

                    stdDevProcLen = ceil(.1*LC_processLen);     // value will always be >= 1

                    normal_distribution<float> DIST_processesLen(LC_processLen, stdDevProcLen);
                    normal_distribution<float> DIST_numIOEvents(C_MEAN_NUM_IOEVENTS, C_STDDEV_IOEVENTS);
                    normal_distribution<float> DIST_IOEventLen(C_MEAN_IOEVENT_LEN, C_STDDEV_IOEVENT_LEN);

                    // generating the random processes
                    processList.reserve(LC_numProcesses);
                    for (int i = 0; i < LC_numProcesses; i++) {
                        Process toBeAdded;
                        toBeAdded.pid = i;

                        // generating random process length from normal distribution
                        randNum = ceil(DIST_processesLen(generator));
                        randNum < 0 ? toBeAdded.reqTime = 1 : toBeAdded.reqTime = randNum;
                        randNum < 0 ? toBeAdded.remTime = 1 : toBeAdded.remTime = randNum;

                        // generating random process start time using regular rand
                        randNum = rand() % (LC_numProcesses*LC_processLen) + 1;
                        toBeAdded.readyTime = randNum;

                        // generating random number IOEvents from normal distribution
                        randNum = floor(DIST_numIOEvents(generator));
                        randNum < 0 ? toBeAdded.ioEvents.reserve(0) : toBeAdded.ioEvents.reserve(randNum % C_MAX_NUM_IOEVENTS+1);
                        
                        // Generating a random number of I/O events of random lengths
                        for (int j = 0, numEvents = toBeAdded.ioEvents.capacity(); j < numEvents; j++) {
                            randNum = static_cast<int>(ceil(DIST_IOEventLen(generator)));
                            
                            /* Only adding events if the number of events is >= 1 */
                            if (randNum >= 1) {
                                IOEvent toBeAddedIO;
                                toBeAddedIO.duration = randNum;
                                toBeAddedIO.time = rand()%toBeAdded.reqTime;
                                toBeAdded.ioEvents.push_back(toBeAddedIO);
                            }
                        }
                        
                        // Adding the completed random process to the list
                        processList.push_back(toBeAdded);
                    }
                    
                    vector<int> selections = {};
                    vector<int> prev_Selections = {};

                    switch (LC_algorithm) { // Switch for algorithm tested prep
                        case 0: // FIFO
                            scheduler_FIFO(processList, prev_Selections, LC_cores, true);
                            break;
                        case 1: // Round Robin
                            scheduler_RR(processList, LC_cores, quanta, true);
                            break;
                        case 2: // Shortest Process Next
                            scheduler_SPN(processList, prev_Selections, LC_cores, true);
                            break;
                    }
                    
                    while(!allProcessesComplete(processList)) {
                        // Copying the previous selections into prev_Selections
                        prev_Selections.clear();
                        for (auto & pos : selections) {
                            prev_Selections.push_back(pos);
                        }
                        
                        unblock(processList);                                       // unblocking processes
                        readyUp(pTime, processList);                                // changing states to ready
                        switch (LC_algorithm) {                                     // Switching on tested algorithm
                            case 0:
                                selections = scheduler_FIFO(processList, prev_Selections, LC_cores, false);
                                break;
                            case 1:
                                /*  processLen/quanta
                                    4/3, 8/4, 16/7, 32/11, 64/16, 128/26, 256/41, 512/64 */
                                quanta = ceil(pow(LC_processLen, 2.0/3));
                                selections = scheduler_RR(processList, LC_cores, quanta, false);
                                break;
                            case 2:
                                selections = scheduler_SPN(processList, prev_Selections, LC_cores, false);
                                break;
                            case 3:
                                selections = scheduler_SRT(processList, prev_Selections, LC_cores);
                                break;
                            case 4:
                                selections = scheduler_HRRN(processList, prev_Selections, LC_cores);
                                break;
                        }
                        increment(processList, selections);                         // Incrementing processing time
                        finisher(pTime, processList, selections);                   // Marking if done
                        ioInterrupts(processList, selections);                      // Blocking if ioEvent
                        checkStarvation(pTime, processList, C_MEAN_IOEVENT_LEN);    // Checking if starved
                        pTime++;
                    }
                    
                    // Computing the number of processes that starved
                    for (auto & proc : processList) {
                        if (proc.state == starved) {
                            numStarvations++;
                        }
                    }
                    outputMutex.lock();
                    fout<<LC_cores<<","<<LC_numProcesses<<","<<LC_processLen<<","<<LC_algorithm<<","<<numStarvations<<endl;
                    cout<<"Iteration Finished: #Cores/#Proc/AvgLen/Algo/#Starved "<<LC_cores<<"/"<<LC_numProcesses<<"/"<<LC_processLen<<"/"<<LC_algorithm<<"/"<<numStarvations<<endl;
                    outputMutex.unlock();
                }
                cout<<endl;
            }
        }
        fout.close();
        outputPos++;
    }

    return 0;
}