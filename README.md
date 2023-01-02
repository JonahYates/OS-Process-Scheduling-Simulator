# Process Scheduling Algorithms Project
#### Author: Jonah Yates, Drew Schulte, Selorm Sosuh
***

**Abstract** – This work explores the effect process selection function, number of cores, number of processes, and process length have on process starvation in a given system. The algorithms in question are First-In First-Out (FIFO), Round Robin (RR), Shortest Process Next (SPN), Shortest Remaining Time (SRT), and Highest Response Ratio Next (HRRN). The metric used to analyze the effectiveness of each algorithm is the percentage of assigned processes that starved.

**View ORIGINAL_OS_Report.pdf** to read the full project analysis.
This project was originally created for an operating systems class in Spring 2021. Since then, a number of changes have been made to fix issues outlined in the original report, and improve the overall quality and efficiency of the program. The changelog is as follows...
- **[Dec 2022]** Implemented threading, RR, SPN, SRT, HRRN improvements (***info***% improvement in execution speed)
    - rewrote part of the RR algorithm
        - Eliminated an unaccounted edge case
        - Decreased the C++ simulation runtime from ~6hrs → ~58m:49s, a 87.91% improvement
    - added thread usage and further optimizations to SPN, SRT, & HRRN.
        - Decreased the runtime from ~58m:49s → ***INFO***m:s, a further ***INFO***% improvement (***INFO***% overall).
    - changed the measured metric when graphing from number of starvations to proportion of processes
- **[Spring 2021]** Original Upload
    - uploaded files from school repo

***
**Installing necessary packages & software**
The following commands will assist with installing missing software or packages, if applicable.
```
choco upgrade python -y
choco install make
python -m ensurepip --upgrade
py -m pip install numpy
py -m pip install pandas
py -m pip install matplotlib
```