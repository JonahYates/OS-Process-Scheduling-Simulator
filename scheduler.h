// Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
// File: schedule.h
// Purpose: header file for the various scheduling algorithms tested

#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <vector>
#include "process.h"

// First-In First-Out implementation                    - NON-PREEMPTIVE
vector<int> scheduler_FIFO(vector<Process> & procList, vector<int> & old_Selections, const int processLimit);

// Round Robin implementation                           - PREEMPTIVE
vector<int> scheduler_RR(vector<Process> & procList, vector<int> & old_Selections, const int processLimit,
                         const int slice_len, int & qSize);

// Shortest Process Next implementation                 - NON-PREEMPTIVE
vector<int> scheduler_SPN(vector<Process> & procList, vector<int> & old_Selections, const int processLimit);

// Shortest Remaining Time implementation               - PREEMPTIVE
vector<int> scheduler_SRT(vector<Process> & procList, vector<int> & old_Selections, const int processLimit);

// Highest Response Ratio Next implementation           - NON-PREEMPTIVE
vector<int> scheduler_HRRN(vector<Process> & procList, vector<int> & old_Selections, const int processLimit);

#endif