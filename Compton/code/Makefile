CXXFLAGS=`root-config --cflags`
LDLIBS=`root-config --libs` -lMinuit
CXX=g++
CC=g++

main: main.o

run: main
	if ! [ -d ../tmp ]; then mkdir ../tmp; fi
	./main
	
time: source
	./source

clean:
	rm -f main main.o source source.o
	rm -r ../tmp

.PHONY: run clean time
