#!/bin/bash
# Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
# Purpose: bash script to fully run the simulation. Will create
# necessary files, compile and run program and then perform cleanup

# compiling/building necessary files
clear
if [ ! -d data ]; then
    mkdir data
fi
touch data/data1.csv
touch data/data2.csv
touch data/data4.csv
touch data/data8.csv
touch data/data12.csv
touch data/data16.csv
make


# running simulation program
echo "Enter 1 to proceed, or anything else to quit"
read proceed
if [ ! $proceed -eq 1 ]; then
    exit
fi
./simulation

echo "Enter 1 to save a copy of the data, or anything else to not"
read save
if [ $save -eq 1 ]; then
    cat data/data1.csv data/data2.csv data/data4.csv data/data8.csv data/data12.csv data/data16.csv > data/SavedData.txt
fi
python3 graph.py

# cleanup
rm simulation