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
	echo "3 89.48 0 10.54938 0.17671 91.33 0 29.97568 0.48955 93.02 0 14.55719 0.195 0 0" | ./bwigner
# 	echo "3 89.48 0 10.54938 0.17671 91.33 0 29.97568 0.48955 93.02 0 14.55719 0.24172 0 0" | ./bwigner
# 	echo "3 89.48 0 0.6363 0.02751 91.33 0 1.52865 0.03919 93.02 0 0.86775 0.03477 1 0 0 2.60481 1 91.156 1 0" | ./bwigner
# 	last bool: do corr plots.

fitstat: bwigner
	if ! [ -d ../tmp ]; then mkdir ../tmp; fi
	echo "3 89.48 0 10.54938 0.15677 91.33 0 29.97568 0.43058 93.02 0 14.55719 0.24172 0 0" | ./bwigner       #TODO, andere fehler (stat. halt)
	echo "3 89.48 0 0.6363 0.03325 91.33 0 1.52865 0.04863 93.02 0 0.86775 0.04007 1 0 0 2.60481 1 91.156 1 0" | ./bwigner

clean:
	rm -f analyseZ0 analyse.o bwigner bwigner.o
	rm -r ../tmp

.PHONY: run clean fit
