// Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
// File: process.cpp
// Purpose: implementation of process data-type's methods

#include "process.h"

bool allProcessesComplete(vector<Process> & processVector)
{
    for(auto & proc : processVector)
        if(proc.state != done && proc.state != starved)
            return false;
    return true;
}

void unblock(vector<Process> & processList)
{
    for (auto & proc : processList)
        if (proc.state == blocked)
        {
            /*  Unblocking all processes that have been IO blocked
                for the IOEvent's duration and resetting wait time
                of these processes. Decreasing the IO duration for
                all other processes that have time still remaining
                in their current IOEvent. */
                
            if (proc.currentEvent.duration == 0)
            {
                proc.state = ready;
                proc.waitTime = 0;
            }
            else
                proc.currentEvent.duration--;
        }
}

void readyUp(const long time, vector<Process> & processList)
{
    // Reading all processes in the new state and have met their start time
    for (auto & proc : processList)
        if (proc.state == notArrived && proc.readyTime == time)
            proc.state = ready;
}

void increment(vector<Process> & processList, const vector<int> & selection)
{
    // Iterating through currently selected/running processes and increasing their processorTime
    for (auto & sel : selection)
        processList[sel].processingTime++;

    // Iterating through unselected ready processes and increasing their waitTime
    for (auto & proc : processList)
        if (proc.state == ready)
            proc.waitTime++;
}

void finisher(const long time, vector<Process> & processList, vector<int> selection)
{
    // Checking the currently selected/running processes for whether processesTime
    // meets reqTime. Marking them as finished and updating doneTime if so.
    for (auto & sel : selection)
        if (processList[sel].processingTime == processList[sel].reqTime)
        {
            processList[sel].state = done;
            processList[sel].doneTime = time;
        }
}

void ioInterrupts(vector<Process> & processList, vector<int> & selection)
{
    // Searching all currently running processes for IOEvents.
    for (auto & sel : selection)
    {
        // Searching all IOEvents to see if one is to start at the current time
        for (int j = 0, numIOEvents = (processList[sel].ioEvents).size(); j < numIOEvents; j++)
            if (processList[sel].state == processing &&
                (processList[sel].ioEvents[j]).time == processList[sel].processingTime)
            {
                /*  If the j-th interrupt is slated to occur at the current processing instance
                    of time, then storing the interrupt in currentEvent and switch to blocked.
                    Duration of this event is also decreased. */
                processList[sel].currentEvent = processList[sel].ioEvents[j];
                processList[sel].currentEvent.duration--;
                processList[sel].state = blocked;
                processList[sel].ioBlocked = true;
            }
    }
}

void checkStarvation(const long time, vector<Process> & processList, const int meanIOLen)
{
    /* Variable Declaration Section */
    long expEndTime;

    for (auto & proc : processList)
    {
        expEndTime = proc.readyTime + 4*proc.reqTime;
        if (static_cast<int>(proc.ioEvents.size()) > 0)
            expEndTime += proc.ioEvents.size() * meanIOLen;
        if (proc.doneTime == -1 && time == expEndTime)
            proc.state = starved;
    }
}