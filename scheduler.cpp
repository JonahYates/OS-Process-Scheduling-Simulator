/*  Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
    Purpose: implementation of the various scheduling algorithms and helpers */

#include "scheduler.h"
#include <algorithm>

vector<int> scheduler_FIFO(vector<Process> & procList, vector<int> & prev_Selections, const int processLimit) {
    static vector<int> queue = {};
    vector<int> new_Selections = {};
    int numProcGrabbed = 0;

    // Continue processing running processes (NON-PREEMPTIVE ALGORITHM)
    for (size_t i = 0; i < prev_Selections.size(); i++) {
        if (procList[prev_Selections[i]].state == processing) {
            new_Selections.push_back(prev_Selections[i]);
            numProcGrabbed++;
            if (numProcGrabbed == processLimit) {
                return new_Selections;
            }
        }
    }

    // Grabbing from pool of ready processes next if processLimit not yet met
    for (size_t i = 0; i < procList.size(); i++) {
        if (procList[i].state == ready) {
            new_Selections.push_back(i); // selecting the i-th process from procList for processing
            numProcGrabbed++;
            procList[i].state = processing;
            if (numProcGrabbed == processLimit) {
                return new_Selections;
            }
        }
    }

    return new_Selections;
}

vector<int> scheduler_RR(vector<Process> & procList, const int processLimit, const int quanta, const bool reset) {
    static vector<int> queue = {};
    vector<int> new_Selections = {};
    int numProcGrabbed = 0;

    if (reset) { // Clearing the queue for a new simulation
        queue.clear();
    } else {
        // Adding any ready process to the queue and removing any starved, done, or blocked processes
        for (size_t i = 0; i < procList.size(); i++) {
            if (procList[i].state == ready && !procList[i].inQueue) {
                queue.push_back(i);
                procList[i].inQueue = true;
            } else if (procList[i].inQueue && (procList[i].state == starved || procList[i].state == done || procList[i].state == blocked)) {
                /*  removing the process i in procList from the queue via pid comparison 
                    QUEUE SIZE MANIPULATED: break out of loop after removal, no further traversal */
                int removePid = procList[i].pid;
                for (size_t removePos = 0; removePos < queue.size(); removePos++) {
                    if (procList[queue[removePos]].pid == removePid) {
                        queue.erase(queue.begin()+removePos);
                        break;
                    }
                }
                procList[i].inQueue = false;
            }
        }
        
        /*  moving any previous selection that has used up its quanta to the back of the queue
            QUEUE SIZE MANIPULATED: size remains constant regardless of action, no effect on traversal */
        for (size_t i = 0; i < queue.size(); i++) {
            if (procList[queue[i]].slice == quanta && procList[queue[i]].state == processing) {
                queue.push_back(queue[i]);
                procList[queue[i]].state = ready;
                procList[queue[i]].slice = 0;
                queue.erase(queue.begin()+i);
            }
        }

        /*  selecting the first n (processLimit) processes in the queue
            QUEUE SIZE NOT MANIPULATED: no effect on traversal */
        for (size_t i = 0; (i < queue.size() && numProcGrabbed < processLimit); i++) {
            new_Selections.push_back(queue[i]);
            numProcGrabbed++;
            procList[queue[i]].state = processing;
            if (numProcGrabbed == processLimit) {
                return new_Selections;
            }
        }
    }

    return new_Selections;
}

vector<int> scheduler_SPN(vector<Process> & procList, vector<int> & old_Selections, const int processLimit, const bool sortProcList) {
    vector<int> new_Selections = {};
    int numProcGrabbed = 0;

    // Sorting procList by requiredTime
    if (sortProcList) {
        sort(procList.begin(), procList.end(), [] (const Process & p1, const Process & p2) {
            return p1.reqTime < p2.reqTime;
        });
    } else {
        // Continue processing running processes (NON-PREEMPTIVE ALGORITHM)
        for (size_t i = 0; i < old_Selections.size(); i++) {
            if (procList[old_Selections[i]].state == processing) {
                new_Selections.push_back(old_Selections[i]);
                numProcGrabbed++;
                if (numProcGrabbed == processLimit) {
                    return new_Selections;
                }
            }
        }

        // Selecting the first n (processLimit) processes
        for (size_t i = 0; i < procList.size(); i++) {
            if (procList[i].state == ready) {
                new_Selections.push_back(i);
                numProcGrabbed++;
                procList[i].state = processing;
                if (numProcGrabbed == processLimit) {
                    return new_Selections;
                }
            }
        }
    }
    
    return new_Selections;
}

vector<int> scheduler_SRT(vector<Process> & procList, vector<int> & old_Selections, const int processLimit) {
    vector<int> new_Selections = {};
    int numProcGrabbed = 0;
    int srtPos;                     // position of shortest remaining time in procList
    bool alreadyInSel;

    // Marking all previously processing processes as ready (PREEMPTIVE ALGORITHM)
    for (auto & pos : old_Selections) {
        if (procList[pos].state == processing) {
            procList[pos].state = ready;
        }
    }

    /* computing the remaining time for every process in procList, selecting and
       replacing the selections until we have processLimit (#) of shortest processes.
       Tie-breaker criteria is SPN followed by FIFO procList order. */
    for (int i = numProcGrabbed; i < processLimit; i++) {
        srtPos = -1;

        // Determining the position of the shortest remaining time process
        for (int j = 0, numProc = procList.size(); j < numProc; j++) {
            if (procList[j].state == ready)
            {
                if (srtPos == -1) {
                    srtPos = j;
                } else if ((procList[srtPos].reqTime - procList[srtPos].processingTime > procList[j].reqTime - procList[j].processingTime)
                    || (procList[srtPos].reqTime - procList[srtPos].processingTime == procList[j].reqTime - procList[j].processingTime
                        && procList[srtPos].reqTime > procList[j].reqTime)) 
                {
                    alreadyInSel = false;
                    for (auto & newlySel : new_Selections) {
                        if (newlySel == j) {
                            alreadyInSel = true;
                        }
                    }
                    if (!alreadyInSel) {
                        srtPos = j;
                    }
                }
            }
        }
        if (srtPos != -1) {
            new_Selections.push_back(srtPos);
            numProcGrabbed++;
            procList[srtPos].state = processing;
        }
        if (numProcGrabbed == processLimit) {
            return new_Selections;
        }
    }

    return new_Selections;
}

vector<int> scheduler_HRRN(vector<Process> & procList, vector<int> & old_Selections, const int processLimit) {
    vector<int> new_Selections = {};
    int numProcGrabbed = 0;
    float lrHRRN, inspHRRN;         // hrrn ratio values of current largest hrrn and the one we're inspecting
    int lrPos;                      // position of largest ratio in procList
    bool alreadyInSel;
    
    // Continue processing running processes (NON-PREEMPTIVE ALGORITHM)
    for (size_t i = 0; i < old_Selections.size(); i++) {
        if (procList[old_Selections[i]].state == processing) {
            new_Selections.push_back(old_Selections[i]);
            numProcGrabbed++;
            if (numProcGrabbed == processLimit) {
                return new_Selections;
            }
        }
    }

    // Selecting the remaining number of processes with the highest hrrn
    for (int i = numProcGrabbed; i < processLimit; i++) {
        lrPos = -1;

        // Determining the position of the process with the largest ratio
        for (int j = 0, numProc = procList.size(); j < numProc; j++) {
            if (procList[j].state == ready) {
                if (lrPos == -1) {
                    lrPos = j;
                } else {
                    lrHRRN = (1.0*procList[lrPos].waitTime + (procList[lrPos].reqTime - procList[lrPos].processingTime)) / (procList[lrPos].reqTime - procList[lrPos].processingTime);
                    inspHRRN = (1.0*procList[j].waitTime + (procList[j].reqTime - procList[j].processingTime)) / (procList[j].reqTime - procList[j].processingTime);
                    if (lrHRRN < inspHRRN) {   
                        alreadyInSel = false;
                        for (auto & newlySel : new_Selections) {
                            if (newlySel == j) {
                                alreadyInSel = true;
                            }
                        }
                        if (!alreadyInSel) {
                            lrPos = j;
                        }
                    }
                }
            }
        }
        if (lrPos != -1) {
            new_Selections.push_back(lrPos);
            numProcGrabbed++;
            procList[lrPos].state = processing;
        }
        if (numProcGrabbed == processLimit) {
            return new_Selections;
        }
    }

    return new_Selections;
}