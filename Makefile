
TARGET=exponential-fit

all : ${TARGET} run

${TARGET} : exponential-fit.cpp exponential-fit.h
	g++ -std=c++11 $^ -o $@
	
run : ${TARGET}
	./$^

