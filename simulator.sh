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
    mkdir data
fi
touch data/data1.csv
touch data/data2.csv
touch data/data4.csv
touch data/data8.csv
touch data/data16.csv
touch data/data32.csv
if [ ! -d imgs ]; then
    mkdir imgs
fi
make


# running simulation program
echo "Enter 1 to proceed, or anything else to quit"
read proceed
if [ ! $proceed -eq 1 ]; then
    exit
fi
clear
time ./simulation.exe

echo "Enter 1 to save a copy of the data, or anything else to not"
read save
if [ $save -eq 1 ]; then
    cat data/data1.csv data/data2.csv data/data4.csv data/data8.csv data/data16.csv data/data32.csv > data/SavedData.csv
fi
python3 graph.py

# cleanup
rm simulation.exe