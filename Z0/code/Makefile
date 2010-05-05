CXXFLAGS=`root-config --cflags`
LDLIBS=`root-config --libs`
CXX=g++
CC=g++

analyseZ0: analyse.o
	$(CXX) analyse.o $(LDLIBS) -o analyseZ0

run: analyseZ0
	./run_all.sh

clean:
	rm -f analyseZ0 analyse.o
	rm -r ../tmp

.PHONY: run clean