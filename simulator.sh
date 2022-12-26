#!/bin/bash
#   Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
#   Purpose: bash script to run the simulation. Will create the necessary
#            files, compile and run program and then perform cleanup

# compiling/building necessary files
clear
[ -f simulation.exe ] && rm simulation.exe;
[ ! -d data ] && mkdir data
touch data/data1.csv data/data2.csv data/data4.csv data/data8.csv data/data16.csv data/data32.csv
[ ! -d imgs ] && mkdir imgs
make

# running simulation program and saving the output if requested
echo "Enter 1 to proceed, or anything else to quit"
read proceed
[ ! $proceed -eq 1 ] && exit
clear
time ./simulation.exe

echo "Enter 1 to save a copy of the data, or anything else to not"
read save
[ $save -eq 1 ] && cat data/*.csv > savedData.csv

# creating the graphs via the python program
py graph.py

# cleanup
rm simulation.exe