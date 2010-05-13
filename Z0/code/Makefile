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
	echo "3 89.48 0 10.54938 0.17671 91.33 0 29.97568 0.17827 93.02 0 14.55719 0.17675 0 0" | ./bwigner
	echo "3 89.48 0 0.6363 0.02751 91.33 0 1.52865 0.02961 93.02 0 0.86775 0.02929 1 0 0 2.60481 1 91.156 1 0" | ./bwigner
# 	last bool: do corr plots.

clean:
	rm -f analyseZ0 analyse.o bwigner bwigner.o
	rm -r ../tmp

.PHONY: run clean fit
