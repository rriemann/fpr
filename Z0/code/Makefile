CXXFLAGS=`root-config --cflags`
LDLIBS=`root-config --libs`
CXX=g++
CC=g++

analyseZ0: analyse.o
	$(CXX) analyse.o $(LDLIBS) -o analyseZ0

run:
	./analyseZ0

clean:
	rm -f analyseZ0 analyse.o

.PHONY: run clean