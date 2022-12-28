CXX = g++
FLAGS = -W -Wall -Wextra

simulate: main.o process.o scheduler.o
	${CXX} ${FLAGS} main.o process.o scheduler.o -o simulation.exe

process.o: process.h

scheduler.o: scheduler.h