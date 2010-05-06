CXXFLAGS=`root-config --cflags`
LDLIBS=`root-config --libs` -lMinuit
CXX=g++
CC=g++

analyseZ0: analyse.o
	$(CXX) analyse.o $(LDLIBS) -o analyseZ0

run: analyseZ0
	./run_all.sh

fit: bwigner
	if ! [ -d ../tmp ]; then mkdir ../tmp; fi
	echo "3 89.48 0 6.274 1 91.33 0 24.88 1 93.02 0 9.33 1 0 0" | ./bwigner
# 	last bool: do corr plots.

clean:
	rm -f analyseZ0 analyse.o bwigner bwigner.o
	rm -r ../tmp

.PHONY: run clean fit
