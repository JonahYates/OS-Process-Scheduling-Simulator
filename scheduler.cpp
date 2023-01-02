/*  Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
    Purpose: implement the various scheduling algorithms */

#include "scheduler.h"
#include <algorithm>

vector<int> scheduler_FIFO(vector<Process> & procList, vector<int> & prev_Selections, const int processLimit, const bool sortProcList) {
    vector<int> new_Selections = {};
    short numProcGrabbed = 0;

    if (sortProcList) { // Sorting procList by start time
        sort(procList.begin(), procList.end(), [] (const Process & p1, const Process & p2) {
            return p1.readyTime < p2.readyTime;
        });
    } else {
        // Continue processing running processes (NON-PREEMPTIVE ALGORITHM)
        for (auto & pos : prev_Selections) {
            if (procList[pos].state == processing) {
                new_Selections.push_back(pos);
                numProcGrabbed++;
                if (numProcGrabbed == processLimit) {
                    return new_Selections;
                }
            }
        }
            
        /*  grabbing up to processLimit ready processes with the lowest readyTime
            LOOP INFO: procList size does not change */
        for (int i = 0, numProc = procList.size(); i < numProc; i++) {
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

vector<int> scheduler_RR(vector<Process> & procList, const int processLimit, const int quanta, const bool clearQueue) {
    static vector<int> queue = {};
    vector<int> new_Selections = {};
    short numProcGrabbed = 0;

    if (clearQueue) { // Clearing the queue for a new simulation
        queue.clear();
    } else {

        /*  adding any ready process to the queue and removing any starved, done, or blocked processes 
            LOOP INFO: procList size does not change */
        for (int i = 0, numProc = procList.size(); i < numProc; i++) {
            if (procList[i].state == ready && !procList[i].inQueue) {
                queue.push_back(i);
                procList[i].inQueue = true;
            } else if (procList[i].inQueue && (procList[i].state == starved || procList[i].state == done || procList[i].state == blocked)) {
                
                /*  removing the process i in procList from the queue via pid comparison 
                    QUEUE SIZE MANIPULATED: break out of loop after removal, no further traversal */
                int removePid = procList[i].pid;
                for (int removePos = 0, queueSize = queue.size(); removePos < queueSize; removePos++) {
                    if (procList[queue[removePos]].pid == removePid) {
                        queue.erase(queue.begin()+removePos);
                        break;
                    }
                }
                procList[i].inQueue = false;
            }
        }
        
        /*  moving any previous selection that has used up its quanta to the back of the queue
            QUEUE MANIPULATED: queue size does not change */
        for (int i = 0, queueSize = queue.size(); i < queueSize; i++) {
            if (procList[queue[i]].slice == quanta && procList[queue[i]].state == processing) {
                queue.push_back(queue[i]);
                procList[queue[i]].state = ready;
                procList[queue[i]].slice = 0;
                queue.erase(queue.begin()+i);
            }
        }

        // Selecting the first processLimit processes in the queue
        for (auto & pos : queue) {
            new_Selections.push_back(pos);
            numProcGrabbed++;
            procList[pos].state = processing;
            if (numProcGrabbed == processLimit) {
                return new_Selections;
            }
        }
    }

    return new_Selections;
}

vector<int> scheduler_SPN(vector<Process> & procList, vector<int> & old_Selections, const int processLimit, const bool sortProcList) {
    vector<int> new_Selections = {};
    short numProcGrabbed = 0;

    if (sortProcList) { // Sorting procList by required time
        sort(procList.begin(), procList.end(), [] (const Process & p1, const Process & p2) {
            return p1.reqTime < p2.reqTime;
        });
    } else {
        // Continue processing running processes (NON-PREEMPTIVE ALGORITHM)
        for (auto & pos : old_Selections) {
            if (procList[pos].state == processing) {
                new_Selections.push_back(pos);
                numProcGrabbed++;
                if (numProcGrabbed == processLimit) {
                    return new_Selections;
                }
            }
        }

        /*  selecting a max of processLimit processes
            LOOP INFO: procList size does not change */
        for (int i = 0, numProc = procList.size(); i < numProc; i++) {
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
    int srtPos;

    // Marking all previously processing processes as ready (PREEMPTIVE ALGORITHM)
    for (auto & pos : old_Selections) {
        procList[pos].inQueue = false;
        if (procList[pos].state == processing) {
            procList[pos].state = ready;
        }
    }

    /*  selecting at most processLimit processes with the least remaining time, disregarding whether
        they were previously processing. Tie-breaker criteria is SPN, followed by procList order. */
    for (int i = 0; i < processLimit; i++) {
        srtPos = -1;

        /*  determining the position of the shortest remaining time process
            LOOP INFO: procList size does not change */
        for (int j = 0, numProc = procList.size(); j < numProc; j++) {
            if (procList[j].state == ready
                && !procList[j].inQueue
                && (srtPos == -1
                    || (procList[j].remTime < procList[srtPos].remTime
                        || (procList[j].remTime == procList[srtPos].remTime
                            && procList[j].reqTime < procList[srtPos].reqTime)))) {
                srtPos = j;
            }
        }

        if (srtPos != -1) {
            new_Selections.push_back(srtPos);
            numProcGrabbed++;
            procList[srtPos].state = processing;
            procList[srtPos].inQueue = true;
        }
        
        /*  returning the selections if we've grabbed processLimit
            processes, or if there are no more possible selections */
        if (numProcGrabbed == processLimit || srtPos == -1) {
            return new_Selections;
        }
    }

    return new_Selections;
}

vector<int> scheduler_HRRN(vector<Process> & procList, vector<int> & old_Selections, const int processLimit) {
    vector<int> new_Selections = {};
    short numProcGrabbed = 0;
    int lrPos;
    
    // Continue processing running processes (NON-PREEMPTIVE ALGORITHM)
    for (auto & pos : old_Selections) {
        procList[pos].inQueue = false;
        if (procList[pos].state == processing) {
            new_Selections.push_back(pos);
            numProcGrabbed++;
            procList[pos].inQueue = true;
            if (numProcGrabbed == processLimit) {
                return new_Selections;
            }
        }
    }

    // Updating the hrrn ratio for all processes
    for (auto & proc : procList) {
        proc.hrrnRatio = (1.0*proc.waitTime + proc.remTime) / proc.remTime;
    }

    // Selecting at most processLimit processes with the largest HRRN ratio
    for (int i = numProcGrabbed; i < processLimit; i++) {
        lrPos = -1;

        /*  determining the position of the process with the largest ratio
            LOOP INFO: procList size does not change */
        for (int j = 0, numProc = procList.size(); j < numProc; j++) {
            if (procList[j].state == ready) {
                if (lrPos == -1
                    && !procList[j].inQueue
                    && (lrPos == -1
                        || procList[j].hrrnRatio > procList[lrPos].hrrnRatio)) {
                    lrPos = j;
                }
            }
        }

        if (lrPos != -1) {
            new_Selections.push_back(lrPos);
            numProcGrabbed++;
            procList[lrPos].state = processing;
            procList[lrPos].inQueue = true;
        }

        /*  returning the selections if we've grabbed processLimit
            processes, or if there are no more possible selections */
        if (numProcGrabbed == processLimit || lrPos == -1) {
            return new_Selections;
        }
    }

    return new_Selections;
}