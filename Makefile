
TARGET=exponential-fit

all : ${TARGET} run

${TARGET} : exponential-fit.cpp exponential-fit.h unit-test.cpp
	g++ -std=c++11 -O2 exponential-fit.cpp unit-test.cpp -o $@ -lgtest_main -lgtest
	
run : ${TARGET}
	./$^

clean :
	rm -f ${TARGET}
