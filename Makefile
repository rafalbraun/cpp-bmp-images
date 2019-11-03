all: a.out

BMP.o: BMP.cpp
	g++ -c BMP.cpp -o BMP.o

main.o: main.cpp
	g++ -c main.cpp -o main.o

a.out: main.o BMP.o
	g++ main.o BMP.o -o a.out

clean:
	rm *.o a.out