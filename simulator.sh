#!/bin/bash

# Programmers: Drew Schulte, Jonah Yates, & Selorm Sosuh
# File: simulator.sh
# Purpose: bash script to fully run the simulation. Will create
# necessary files, compile and run program and then perform cleanup

# compiling/building necessary files
clear

# running simulation program
make
./simulation

echo "[1 = yes] Do you wish to save a copy of the data?"
read save
if [ $save -eq 1] then
    cp data.csv savedData.csv
fi

# running graphing application
python3 graph.py

# cleaning up files no longer needed
rm simulation