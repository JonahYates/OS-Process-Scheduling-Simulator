#!/bin/bash
#   Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
#   Purpose: bash script to run the simulation. Will create the necessary
#            files, compile and run program and then perform cleanup

# compiling/building necessary files
clear
if [ -f simulation.exe ]; then
    rm simulation.exe
fi
if [ ! -d data ]; then
    mkdir data1
fi
touch data/data1.csv data/data2.csv data/data4.csv data/data8.csv data/data16.csv data/data32.csv
if [ ! -d imgs ]; then
    mkdir imgs
fi
make

# running simulation program and exiting if a keyboard interrupt is encountered
echo "Enter 1 to proceed, or anything else to quit"
read proceed
if [ ! $proceed -eq 1 ]; then
    exit
fi
clear
time ./simulation.exe
if [ $? -eq 127 ]; then
    echo "Cancelling simulation: interrupt encountered"
    exit
fi

# saving the output if requested
echo "Enter 1 to save a copy of the data, or anything else to not"
read save
if [ $save -eq 1 ]; then
    cat data/*.csv > savedData.csv
fi

# creating the graphs via the python program
py graph.py

# cleanup
rm simulation.exe