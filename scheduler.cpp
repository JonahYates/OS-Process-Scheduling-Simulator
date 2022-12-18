// Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
// Purpose: implementation of the various scheduling algorithms

#include "scheduler.h"

vector<int> scheduler_FIFO(vector<Process> & procList, vector<int> & old_Selections, const int processLimit)
{
    /* Variable Declaration Section */
    vector<int> new_Selections = {};
    int numProcGrabbed = 0;

    // Continue processing running processes (NOT PREEMPTIVE)
    for (int i = 0, numProc = old_Selections.size(); i < numProc && numProcGrabbed < processLimit; i++)
    {
        if (procList[old_Selections[i]].state == processing)
        {
            new_Selections.push_back(old_Selections[i]);
            numProcGrabbed++;
            if (numProcGrabbed == processLimit)
                return new_Selections;
        }
    }

    // Grabbing from pool of ready processes next if processLimit not yet met
    for (int i = 0, numProc = procList.size(); i < numProc; i++)
        if (procList[i].state == ready)
        {
            new_Selections.push_back(i); // selecting the i-th process from processList for processing
            procList[i].state = processing;
            numProcGrabbed++;
            if (numProcGrabbed == processLimit)
                return new_Selections;
        }
    return new_Selections;
}

vector<int> scheduler_RR(vector<Process> & procList, vector<int> & old_Selections, const int processLimit,
                         const int slice_len, int & qSize)
{
    /* Variable Declaration Section */
    vector<int> new_Selections = {};
    int numProcGrabbed = 0;
    int qFront;
    bool lowest_found;                  // whether we've found the lowest qPos

    // Adding to the queue any processes in the process list that are ready and not
    // already in the queue and removing any starved or done processes from the queue
    for (auto & proc : procList)
    {
        if(proc.state == ready && proc.qPos == -1)
        {
            proc.qPos = qSize;
            qSize++;
        }
        if ((proc.state == starved || proc.state == done) && proc.qPos != -1)
            proc.qPos = -1;
    }
    // Marking all previously selected processes as ready and moving any that have
    // used up all of their time slice to the back of the queue
    for (auto & prevSel : old_Selections)
    {
        if (procList[prevSel].slice == slice_len && procList[prevSel].state != done)
        {
            procList[prevSel].state = ready;
            procList[prevSel].slice = 0;
            procList[prevSel].qPos = qSize;
            qSize++;
        }
        else if (procList[prevSel].state == processing && !procList[prevSel].ioBlocked)
        {
            numProcGrabbed++;
            procList[prevSel].slice++;
            procList[prevSel].isSelected = true;
            new_Selections.push_back(prevSel);
            if (numProcGrabbed == processLimit)
            {
                // Resetting flags of grabbed processes before returning selections
                for (auto & newSel : new_Selections)
                    procList[newSel].isSelected = false;

                return new_Selections;
            }   
        }
    }

    // determining the front of the queue
    qFront = qSize;
    for (auto & proc : procList)
        if (proc.qPos != -1 && proc.qPos < qFront)
            qFront = proc.qPos;
    
    if (qFront == qSize)
        return new_Selections;

    // each iteration of the for loop, one process, with the lowest qPos is
    // grabbed and added to the selections if not already selected
    for (int b = numProcGrabbed; b < processLimit; b++)
    {
        lowest_found = false;
        
        // finds the lowest qPos not already grabbed
        for (int lowest_num = qFront; (!lowest_found && lowest_num <= qSize); lowest_num++)   // increasing lowest_num
            for (int a = 0, numProc = procList.size(); (a < numProc && !lowest_found); a++)     // checking if any in procLis match lowest_num
                if (!(procList[a].isSelected) && (procList[a].qPos == lowest_num))
                {
                    //check front/lowest qPos to see if it is blocked
                    if(!procList[a].isSelected && (procList[a].state == blocked || procList[a].ioBlocked))
                    {
                        if (procList[a].ioBlocked)
                            procList[a].ioBlocked = false;
                        procList[a].isSelected = true;
                        procList[a].qPos = qSize;
                        qSize++;
                        procList[a].slice = 0;
                    }
                    else
                    {
                        numProcGrabbed++;
                        procList[a].state = processing;
                        procList[a].slice++;
                        procList[a].isSelected = true;
                        lowest_found = true;
                        new_Selections.push_back(a);
                        if (numProcGrabbed == processLimit)
                        {
                            // Resetting flags of grabbed processes before returning selections
                            for (auto & newSel : new_Selections)
                                procList[newSel].isSelected = false;

                            // Shifting down qPos of all processes to increase efficiency
                            if (qFront >= processLimit)
                            {
                                for (auto & proc : procList)
                                    proc.qPos %= qFront;
                                qSize %= qFront;
                            }

                            return new_Selections;
                        }
                    }
                }
    }

    // Resetting the isSelected flag
    for (auto & proc : procList)
        proc.isSelected = false;
    
    if (qFront >= processLimit)
    {
        for (auto & proc : procList)
            proc.qPos %= qFront;
        qSize %= qFront;
    }
    return new_Selections;
}

vector<int> scheduler_SPN(vector<Process> & procList, vector<int> & old_Selections, const int processLimit)
{
    /* Variable Declaration Section */
    vector<int> new_Selections = {};
    int numProcGrabbed = 0;
    int spPos;                          // position of shortest process in procList
    bool alreadyInSel;

    // Continue processing running processes (NOT PREEMPTIVE)
    for (int i = 0, numProc = old_Selections.size(); i < numProc && numProcGrabbed < processLimit; i++)
    {
        if (procList[old_Selections[i]].state == processing)
        {
            new_Selections.push_back(old_Selections[i]);
            numProcGrabbed++;
            if (numProcGrabbed == processLimit)
                return new_Selections;
        }
    }
    
    // Grabbing the next shortest processes
    for (int i = numProcGrabbed; i < processLimit; i++)
    {
        spPos = -1;

        // determining the position of the shortest process
        for (int j = 0, numProc = procList.size(); j < numProc; j++)
            if (procList[j].state == ready)
            {
                if (spPos == -1)
                    spPos = j;
                else if (procList[spPos].reqTime > procList[j].reqTime)
                {   
                    alreadyInSel = false;
                    for (auto & newlySel : new_Selections)
                        if (newlySel == j)
                            alreadyInSel = true;
                    if (!alreadyInSel)
                        spPos = j;
                }
            }

        if (spPos != -1)
        {
            numProcGrabbed++;
            procList[spPos].state = processing;
            new_Selections.push_back(spPos);
        }
        if (numProcGrabbed == processLimit)
            return new_Selections;
    }
    return new_Selections;
}

vector<int> scheduler_SRT(vector<Process> & procList, vector<int> & old_Selections, const int processLimit)
{
    /* Variable Declaration Section */
    vector<int> new_Selections = {};
    int numProcGrabbed = 0;
    int srtPos;                     // position of shortest remaining time in procList
    bool alreadyInSel;

    // Marking all previously processing processes as ready (PREEMPTIVE)
    for (auto & pos : old_Selections)
        if (procList[pos].state == processing)
            procList[pos].state = ready;

    /* Computing the remaining time for every process in procList, selecting and
       replacing the selections until we have processLimit (#) of shortest processes.
       Tie-breaker criteria is SPN followed by FIFO procList order. */
    for (int i = numProcGrabbed; i < processLimit; i++)
    {
        srtPos = -1;

        // determining the position of the shortest remaining time process
        for (int j = 0, numProc = procList.size(); j < numProc; j++)
            if (procList[j].state == ready)
            {
                if (srtPos == -1)
                    srtPos = j;
                else if ((procList[srtPos].reqTime - procList[srtPos].processingTime > procList[j].reqTime - procList[j].processingTime) ||
                    (procList[srtPos].reqTime - procList[srtPos].processingTime == procList[j].reqTime - procList[j].processingTime
                    && procList[srtPos].reqTime > procList[j].reqTime))
                {
                    alreadyInSel = false;
                    for (auto & newlySel : new_Selections)
                        if (newlySel == j)
                            alreadyInSel = true;
                    if (!alreadyInSel)
                        srtPos = j;
                }
            }
        
        if (srtPos != -1)
        {
            numProcGrabbed++;
            procList[srtPos].state = processing;
            new_Selections.push_back(srtPos);
        }
        if (numProcGrabbed == processLimit)
            return new_Selections;
    }
    return new_Selections;
}

vector<int> scheduler_HRRN(vector<Process> & procList, vector<int> & old_Selections, const int processLimit)
{
    /* Variable Declaration Section */
    vector<int> new_Selections = {};
    int numProcGrabbed = 0;
    float lrHRRN, inspHRRN;         // hrrn ratio values of current largest hrrn and the one we're inspecting
    int lrPos;                      // position of largest ratio in procList
    bool alreadyInSel;
    
    // Continue processing running processes (NOT PREEMPTIVE)
    for (int i = 0, numProc = old_Selections.size(); i < numProc && numProcGrabbed < processLimit; i++)
    {
        if (procList[old_Selections[i]].state == processing)
        {
            new_Selections.push_back(old_Selections[i]);
            numProcGrabbed++;
            if (numProcGrabbed == processLimit)
                return new_Selections;
        }
    }

    // selecting the remaining number of processes with the highest hrrn
    for (int i = numProcGrabbed; i < processLimit; i++)
    {
        lrPos = -1;

        // determining the position of the process with the largest ratio
        for (int j = 0, numProc = procList.size(); j < numProc; j++)
            if (procList[j].state == ready)
            {
                if (lrPos == -1)
                    lrPos = j;
                else
                {
                    lrHRRN = (1.0*procList[lrPos].waitTime + (procList[lrPos].reqTime - procList[lrPos].processingTime)) / (procList[lrPos].reqTime - procList[lrPos].processingTime);
                    inspHRRN = (1.0*procList[j].waitTime + (procList[j].reqTime - procList[j].processingTime)) / (procList[j].reqTime - procList[j].processingTime);
                    if (lrHRRN < inspHRRN)
                    {   
                        alreadyInSel = false;
                        for (auto & newlySel : new_Selections)
                            if (newlySel == j)
                                alreadyInSel = true;
                        if (!alreadyInSel)
                            lrPos = j;
                    }
                }
            }

        if (lrPos != -1)
        {
            numProcGrabbed++;
            procList[lrPos].state = processing;
            new_Selections.push_back(lrPos);
        }
        if (numProcGrabbed == processLimit)
            return new_Selections;
    }
    return new_Selections;
}