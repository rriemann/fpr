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
	echo "3 89.48 0 6.274 1 91.33 0 25.90 1 93.02 0 9.06 1 0 0" | ./bwigner
	echo "3 89.48 0 4.62 1 91.33 0 5.14 1 93.02 0 4.61 1 1 0 0 2.00875 1 91.1547 1 0" | ./bwigner
# 	last bool: do corr plots.; mu -> sigma0 = 9.267

clean:
	rm -f analyseZ0 analyse.o bwigner bwigner.o
	rm -r ../tmp

.PHONY: run clean fit
