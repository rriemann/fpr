CXXFLAGS=`root-config --cflags`
LDLIBS=`root-config --libs` -lMinuit
CXX=g++
CC=g++

analyseZ0: analyse.o
	$(CXX) analyse.o $(LDLIBS) -o analyseZ0

run: analyseZ0
	./run_all.sh

fit: bwigner
# 	$(CXX) bwigner.o convolution.o $(LDLIBS) -o bwigner

clean:
	rm -f analyseZ0 analyse.o
	rm -r ../tmp

.PHONY: run clean