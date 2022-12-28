# Process Scheduling Algorithms Project
#### Author: Jonah Yates, Drew Schulte, Selorm Sosuh
***

**Abstract** – This work explores the effect process selection function, number of cores, number of processes, and process length have on process starvation in a given system. The algorithms in question are First-In First-Out (FIFO), Round Robin (RR), Shortest Process Next (SPN), Shortest Remaining Time (SRT), and Highest Response Ratio Next (HRRN). These are examined with respect to 

**View ORIGINAL_OS_Report.pdf** to view the full analysis.
This project was originally created for an operating systems class in Spring 2021. Since then, a number of changes have been made to fix issues outlined in the original report and improve the overall quality and efficiency of the program. The changelog is as follows...
- **[Dec 2022]** Implemented threading, RR & SPN reworked
    - rewrote the inefficient RR algorithm to decrease the C++ simulation runtime from ~6hrs → ~58mins, a 87.91% improvement
    - implemented threading to further reduce the runtime from ~
    - changed the measured metric when graphing from number of starvations to proportion of processes
- **[Spring 2021]** Original Upload
    - uploaded files from school repo

***
**Installing necessary packages & software**
The following commands will assist with installing missing software or packages, if applicable.
```
pip install pandas
pip install numpy
pip install matplotlib
choco install make
```