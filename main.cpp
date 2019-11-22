#include <iostream>
#include <iterator>
#include "BMP.h"

//
// g++ BMP.cpp -c -o BMP.o && g++ -c main.cpp -o main.o && g++ main.o BMP.o
//
std::vector<Sector> sectors;

void test1() {
	BMP bmp1("test1.bmp");
	bmp1.colorX(0,600);
	bmp1.write("zzz5.bmp");
}

void test2() {
	BMP bmp1("test1.bmp");
	bmp1.colorY(0,900);
	bmp1.write("zzz6.bmp");
}

void test3() {
	BMP bmp1("test1.bmp");	
	bmp1.colorY(0,900);
	bmp1.flatten();
	bmp1.findSectorsY(0,900,sectors);
	bmp1.write("zzz4.bmp");
}

void test4() {
	BMP bmp1("test1.bmp");
	bmp1.draw_line(0,490,600,490, 0,0,255);
	bmp1.flatten();
	bmp1.write("zzz.bmp");	
}

void test5() {
	BMP bmp1("xdxd.bmp");
	bmp1.colorXxxxxLeft(900,1500);
	bmp1.write("zzz7.bmp");
}

void test6() {
	BMP bmp1("test1.bmp");	
	bmp1.colorY(0,900);
	bmp1.flatten();
	bmp1.findSectorsY(0,900,sectors);
	bmp1.findSectors(sectors);
	bmp1.write("zzz8.bmp");
}

int main() {
	
	//BMP bmp9("zzz1.bmp");
	//BMP bmp9("red-box-background.bmp");
	
	//test1();
	//test2();
	//test5();
	test6();

	return 0;
}
