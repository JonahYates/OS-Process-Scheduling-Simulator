CXX = g++
FLAGS = -W -Wall -Wextra

.PHONY: default run

default: simulate

simulate:
	${CXX} ${FLAGS} *.cpp -o simulation