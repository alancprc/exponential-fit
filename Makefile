
TARGET=exponential-fit

all : ${TARGET} run

${TARGET} : exponential-fit.cpp
	g++ -std=c++11 $^ -o $@
	
run : ${TARGET}
	./$^

