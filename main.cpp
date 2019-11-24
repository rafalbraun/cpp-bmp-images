#include <iostream>
#include <iterator>
#include "BMP.h"

//
// g++ BMP.cpp -c -o BMP.o && g++ -c main.cpp -o main.o && g++ main.o BMP.o
//

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

void test3(std::vector<Sector>& sectors) {
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

void test6(const char* filename, int from, int to, std::vector<Sector>& sectors) {
	BMP bmp1(filename);	
	bmp1.colorY(from,to);
	bmp1.flatten();
	bmp1.findSectorsY(from,to,sectors);
	bmp1.findSectors(sectors);
	bmp1.write("zzz8.bmp");

    for(auto it = sectors.begin(); it != sectors.end(); ++it) {
        std::cout << "(" << it->left << "," << it->right << "," << it->top << "," << it->bottom << ")" << std::endl;
        std::cout << "start:" << bmp1.get_pos(it->left, it->top) << std::endl;
        std::cout << "end:" << bmp1.get_pos(it->right, it->bottom) << std::endl;
    }
}

int main() {
	std::vector<Sector> sectors;
	
	//BMP bmp9("zzz1.bmp");
	//BMP bmp9("red-box-background.bmp");
	
	//test1();
	//test2();
	//test5();
	//test6("test1.bmp", 0, 900);
	test6("xd.bmp", 0, 5405, sectors);
	
    
	return 0;
}
