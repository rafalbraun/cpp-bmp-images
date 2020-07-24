all: a.out

a.out: main.o BMP.o
	g++ main.o BMP.o -o a.out -std=c++17
	rm *.o

#a.out: main.o BMP.o tesseract.hpp
#	g++ main.o BMP.o tesseract.hpp -o a.out -llept -ltesseract
#	rm *.o

BMP.o: BMP.cpp
	g++ -c BMP.cpp -o BMP.o -std=c++17

main.o: main.cpp
	g++ -c main.cpp -o main.o -std=c++17

clean:
	rm a.out
