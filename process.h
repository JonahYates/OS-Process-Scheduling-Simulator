/*  Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
    Purpose: header file for the defined process data-type */

#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

struct IOEvent
{
    IOEvent() : time(-1), duration(0) {}

    long time;       // The time the event occurs during the process execution
    long duration;   // The duration that the process will be Blocked by this IOEvent
};

enum State {notArrived, ready, processing, blocked, starved, done};

struct Process {
    Process() : state(notArrived),
                id(-1),
                readyTime(-1),
                doneTime(-1),
                reqTime(0),
                processingTime(0),
                waitTime(0),
                inQueue(false),
                slice(0){}

    State state;                // current process state
    
    int id;                     // unique id (order of process creation) given to a process for identification

    long readyTime;             // when a process should be ready
    long doneTime;              // time when the process enters done state
    long reqTime;               // required amount of process time needed to mark a process as done    
    long processingTime;        // time spent running a process so far
    long waitTime;              // time process has spent in ready state

    vector<IOEvent> ioEvents;
    IOEvent currentEvent;
    
    bool inQueue;               // whether the process is in the queue for RR and SPN

    // RR vars
    short slice;                // how much of the quanta the process has used
};

void unblock(vector<Process> & processList);                                                // unblocks processes that have completed their io interrupt
void readyUp(const long time, vector<Process> & processList);                               // updates the state to ready for all processes that have hit ready time
void increment(vector<Process> & processList, const vector<int> & selection);               // increments processingTime for any selected/processing processes
void finisher(const long time, vector<Process> & processList, vector<int> selections);      // updates the state to done for all processes that have fully processed
void ioInterrupts(vector<Process> & processList, vector<int> & selection);                  // blocks any processes with interrupts slated to start
void checkStarvation(const long time, vector<Process> & processList, const int meanIOLen);  // updates the state to starved for all processes that starved

bool allProcessesComplete(vector<Process> & processVector);                                 // returns whether all processes have either starved or been completed

#endif