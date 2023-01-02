// Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
// Purpose: implementation of process data-type's methods

#include "process.h"

bool allProcessesComplete(vector<Process> & processVector) {
    for (auto & proc : processVector) {
        if (proc.state != done && proc.state != starved) {
            return false;
        }
    }
    return true;
}

void unblock(vector<Process> & processList) {
    for (auto & proc : processList) {
        if (proc.state == blocked) {
            /*  Unblocking all processes that have been IO blocked
                for the IOEvent's duration and resetting wait time
                of these processes. Decreasing the IO duration for
                all other processes that have time still remaining
                in their current IOEvent. */
                
            if (proc.currentEvent.duration == 0) {
                proc.state = ready;
                proc.waitTime = 0;
            } else {
                proc.currentEvent.duration--;
            }
        }
    }
}

void readyUp(const long time, vector<Process> & processList) {
    // Reading all processes in the new state and have met their start time
    for (auto & proc : processList) {
        if (proc.state == notArrived && proc.readyTime == time) {
            proc.state = ready;
        }
    }
}

void increment(vector<Process> & processList, const vector<int> & selection) {
    // Iterating through currently selected/running processes and increasing their processorTime
    for (auto & sel : selection) {
        processList[sel].processingTime++;
        processList[sel].remTime--;
        processList[sel].slice++;
    }

    // Iterating through unselected ready processes and increasing their waitTime
    for (auto & proc : processList) {
        if (proc.state == ready) {
            proc.waitTime++;
        }
    }
}

void finisher(const long time, vector<Process> & processList, vector<int> selections) {
    /*  checking the currently selected/running processes for whether remTime
        is 0 and marking as finished and updating doneTime if so. */
    for (auto & sel : selections) {
        if (processList[sel].remTime == 0) {
            processList[sel].state = done;
            processList[sel].doneTime = time;
        }
    }
}

void ioInterrupts(vector<Process> & processList, vector<int> & selection) {
    // Searching all currently running processes for IOEvents.
    for (auto & sel : selection) {
        // Searching all IOEvents to see if one is to start at the current time
        for (int j = 0, numIOEvents = (processList[sel].ioEvents).size(); j < numIOEvents; j++) {
            if (processList[sel].state == processing &&
                (processList[sel].ioEvents[j]).time == processList[sel].processingTime)     // check this logic
            {
                /*  If the j-th interrupt is slated to occur at the current processing instance
                    of time, then storing the interrupt in currentEvent and switch to blocked.
                    Duration of this event is also decreased. */
                processList[sel].currentEvent = processList[sel].ioEvents[j];
                processList[sel].currentEvent.duration--;
                processList[sel].state = blocked;
            }
        }
    }
}

// use waitTime variable for starvation check, waitTime = 4*proc.reqTime += ioEvents
void checkStarvation(const long time, vector<Process> & processList, const int meanIOLen) {
    long expectedRunTime;

    for (auto & proc : processList) {
        expectedRunTime = proc.readyTime + 4*proc.reqTime;
        if (proc.ioEvents.size() > 0) {
            expectedRunTime += meanIOLen*proc.ioEvents.size();
        }
        if (proc.doneTime == -1 && time == expectedRunTime) {
            proc.state = starved;
        }
    }
}