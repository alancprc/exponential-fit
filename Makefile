
TARGET=exponential-fit

all : ${TARGET} run

${TARGET} : exponential-fit.cpp exponential-fit.h unit-test.cpp
	g++ -std=c++11 $^ -o $@ -lgtest_main -lgtest
	
run : ${TARGET}
	./$^

