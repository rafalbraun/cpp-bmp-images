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
	bmp1.flatten(240);
	bmp1.findSectorsY(0,900,sectors);
	bmp1.write("zzz4.bmp");
}

void test4(const char* filename) {
	BMP bmp1(filename);
	//bmp1.draw_line(0,490,600,490, 0,0,255);
	bmp1.flatten(170);
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
	bmp1.flatten(170);
	bmp1.findSectorsY(from,to,sectors);
	bmp1.findSectors(sectors);
	bmp1.write("zzz8.bmp");

	int w, h, start_x, start_y, i=0;
    for(auto it = sectors.begin(); it != sectors.end(); ++it, ++i) {
        w = (it->right - it->left);
		h = (it->bottom - it->top);
		start_x = it->left;
		start_y = it->top;
        std::cout << "(" << w << "," << h << ")" << "[" << start_x << "," << start_y << "]" << std::endl;

	    BMP bmp2(bmp1, w, h, start_x, start_y);
	    std::string fname = "dupa_" + std::to_string(i) + ".bmp";
	    bmp2.write(fname.c_str());
    }
}

void test7(const char* filename) {
    //BMP bmp2(418, 14, 309326, 333272, bmp1);
    BMP bmp1(filename);
    BMP bmp2(bmp1, 400, 200, 100, 100);
    bmp2.write("dupa2.bmp");
}

void test8(const char* filename) {
    BMP bmp1(filename);
    bmp1.grayscale();
    bmp1.write("www.bmp");	
}

void test9(const char* filename, int from, int to) {
	BMP bmp1(filename);	
	bmp1.colorY(from,to);
	//bmp1.colorXxxxxLeft(from,to);
	//bmp1.colorXxxxxRight(from,to);
	bmp1.write("zzz8.bmp");
}

void test10(const char* filename, std::vector<Sector>& sectors) {
	BMP bmp1(filename);	
	bmp1.colorY(0,747);
	bmp1.flatten(170);
	bmp1.findSectorsY(0,747,sectors);
	bmp1.findSectors(sectors);
	bmp1.write("zzz8.bmp");
	
    for(auto it = sectors.begin(); it != sectors.end(); ++it) {
        std::cout << "(" << it->left << "," << it->right << "," << it->top << "," << it->bottom << ")" << std::endl;
    }
}

/**
 * 1 read file
 * 2 detect sectors
 * 3 write separate fiels (or read buffers?)
 * 4 join into one txt file
 * 5 write beautiful pdf (or tex)
 *
 * formats: chm, epub, pdf, djvu, aws3, cbz,
 */
int main() {
	std::vector<Sector> sectors;

	//BMP bmp9("zzz1.bmp");
	//BMP bmp9("red-box-background.bmp");
	
	//test1();
	//test2();
	//test5();
	//test6("xd.bmp", 0, 5405, sectors);
	
	//test6("test1.bmp", 0, 900, sectors);
	//test6("xd.bmp", 0, 5400, sectors);
	//test7("test1_crop_before.bmp");
	
	//test7("test1.bmp");

	//test6("/home/vanqyard/pdf-cpp/cpp-bmp-images/77363616_2394743740655482_6308787889551441920_o.bmp", 0, 960, sectors);
	//test8("/home/vanqyard/pdf-cpp/cpp-bmp-images/77363616_2394743740655482_6308787889551441920_o.bmp");

    //test4("www.bmp");

	//test6("www1.bmp", 0, 960, sectors);

	//test9("xd.bmp", 0, 5405);
	//test10("image1.bmp", sectors);
	test6("sdsd.bmp", 0, 408, sectors);
	//test4("sdsd.bmp");

	return 0;
}
