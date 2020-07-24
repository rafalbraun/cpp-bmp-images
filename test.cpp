#include <stdlib.h>
#include <string>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <iterator>

#include "BMP.h"
#include "helpers.cpp"

static int passedTests = 0;

void testFileReadWrite() {
	std::string filename = "./images/output.bmp", output = "out.bmp";
	std::remove(output.c_str());
	BMP bmp1(filename.c_str());
	bmp1.colorX(0, bmp1.bmp_info_header.width);
	bmp1.write(output.c_str());
    std::ifstream ifs(output.c_str());
    std::cout << "[TEST] testFileReadWrite: ";
    if (ifs.good()) {
    	std::cout << "1" << std::endl;
		passedTests++;
    } else {
    	std::cout << "0" << std::endl;
    }
}

void grayscaleTest(const char* filename) {
    BMP bmp1(filename);
    bmp1.grayscale();
    bmp1.write("out.bmp");	
}

void testRemoveAlpha(const char* filename) {
	BMP bmp1(filename);
	bmp1.removeAlpha();
	bmp1.write("tmp.bmp");
}

void testCrop() {
	std::string filename = "./images/output.bmp", output = "out.bmp";
	std::remove(output.c_str());
    BMP bmp1(filename.c_str());
    BMP bmp2(bmp1, 100, 100, 100, 100);
    bmp2.write(output.c_str());
    BMP bmp3(output.c_str());
    std::cout << "[TEST] testCrop: ";
    if (bmp3.bmp_info_header.width == 100 && bmp3.bmp_info_header.height == 100) {
    	std::cout << "1" << std::endl;
		passedTests++;
    } else {
    	std::cout << "0" << std::endl;
    }
}

void testExtractText() {
	extractText("./test/text1.txt", "out.txt");
	bool result = compareFiles("./test/text1.ref", "out.txt");
    std::cout << "[TEST] testExtractText: ";
    if (result == true) {
    	std::cout << "1" << std::endl;
		passedTests++;
    } else {
    	std::cout << "0" << std::endl;
    }
}

void testExtractTextWithFormatting() {
	extractTextWithFormatting("./test/text2.txt", "out.txt");
	bool result = compareFiles("./test/text2.ref", "out.txt");
    std::cout << "[TEST] testExtractTextWithFormatting: ";
    if (result == true) {
    	std::cout << "1" << std::endl;
		passedTests++;
    } else {
    	std::cout << "0" << std::endl;
    }
}

void testPaintStrips() {
	//std::string filename = "./documents/Preface_clear.bmp", output = "out.bmp";
	std::string filename = "./images/output.bmp", output = "out.bmp";
	std::vector<std::pair<int,int>> strips;
	BMP bmp(filename.c_str());
	bmp.removeAlpha();
	bmp.flatten(150);
	//bmp.colorX(0, bmp.bmp_info_header.width);
	bmp.colorY(0, bmp.bmp_info_header.height);
	bmp.stripsY(0, bmp.bmp_info_header.height, strips);
	std::cout << "[TEST] testPaintStrips: ";
	if (strips.size() == 28) {
		std::cout << "1" << std::endl;
		passedTests++;
	} else {
		std::cout << "0" << std::endl;		
	}
	bmp.write(output.c_str());
}

// all effects at once
int mainTest() {
	testCrop();
	testFileReadWrite();
	testExtractText();
	testExtractTextWithFormatting();
	testPaintStrips();
	return passedTests;

	/*
	BMP bmp1(filename);	
	int height = bmp1.bmp_info_header.height-10;
	bmp1.grayscale();
	//bmp1.flatten(170);
	bmp1.colorY(0,height);
	bmp1.colorXxxxxLeft(0, height);
	bmp1.colorXxxxxRight(0, height);
	bmp1.write("out.bmp");
	*/
}

int main(int argc, char* argv[]) {
	return mainTest();
}
	/*
}
std::string testReadBook(const char* filename) {

	int pages = readPagesNumber(filename), threshold;
	std::vector<Paragraph> paragraphs;
	std::vector<std::pair<int,int>> lines;
	std::vector<Sector> sectors;
	Paragraph paragraph;
	
	for(int i=0; i<pages; i++) {
		const char* pagename = separatePage(filename, i);
		// separate
		// convert to bmp
		BMP bmp(pagename);
		bmp.removeAlpha();
	    bmp.grayscale();
	    bmp.findThreshold(threshold);
		bmp.flatten(threshold);
		bmp.colorY(0, bmp.bmp_info_header.height);		
		bmp.countLines(lines, Pixel::_isWhite);
		bmp.removeArtifacts(Pixel::_isWhite);
		//bmp.findSectors(0, bmp.bmp_info_header.height, sectors);

		for(auto iter : sectors) {
			bmp.readParagraph(iter, paragraph);
			paragraphs.push_back(paragraph);
		}
	}
	std::string book = joinParagraphs(paragraphs);
	return book;

	return "";
}
	*/
/*
void testSaveSectorToFile(Sector& s, const char* filename, const char* cropname) {
	BMP bmp(filename);
	bmp.cropToFile(s, cropname);
}*/


/*

void test2(const char* filename) {
	BMP bmp(filename);
	std::vector<std::pair<int,int>> lines;
	printInfo(bmp);
	bmp.colorY(0, bmp.bmp_info_header.height);
	//bmp1.removeArtifacts(Pixel::_isWhite);
	bmp.countLines(lines, Pixel::_isWhite);
	bmp.write("out.bmp");
}

// "test1.bmp"
void test3(const char* filename, std::vector<Sector>& sectors) {
	BMP bmp1(filename);	
	bmp1.colorY(0,900);
	bmp1.flatten(240);
	bmp1.findSectorsY(0,900,sectors);
	bmp1.write("zzz4.bmp");
}

void test4(const char* filename, int threshold) {
	BMP bmp1(filename);
	//bmp1.draw_line(0,490,600,490, 0,0,255);
	bmp1.flatten(threshold);
	bmp1.write("out.bmp");
}

// "xdxd.bmp"
void test5(const char* filename, int y1, int y2) {
	BMP bmp1(filename);
	bmp1.colorXxxxxLeft(y1, y2);
	bmp1.write("out.bmp");
}

void test6(const char* filename, std::vector<Sector>& sectors) {
	BMP bmp1(filename);
	int height = bmp1.bmp_info_header.height;
	bmp1.colorY(0, height);
	bmp1.grayscale();
	bmp1.flatten(170);
	bmp1.findSectorsY(0, height,sectors);
	bmp1.findSectors(sectors);
	bmp1.write("out.bmp");
	
	// int w, h, start_x, start_y, i=0;
 //    for(auto it = sectors.begin(); it != sectors.end(); ++it, ++i) {
 //        w = (it->right - it->left);
	// 	h = (it->bottom - it->top);
	// 	start_x = it->left;
	// 	start_y = it->top;
 //        std::cout << "(" << w << "," << h << ")" << "[" << start_x << "," << start_y << "]" << std::endl;

	//     BMP bmp2(bmp1, w, h, start_x, start_y);
	//     std::string fname = "dupa_" + std::to_string(i) + ".bmp";
	//     bmp2.write(fname.c_str());
 //    }
}

void test10(const char* filename, std::vector<Sector>& sectors) {
	BMP bmp1(filename);	
	int height = bmp1.bmp_info_header.height;
	//bmp1.colorY(0,height);
	//bmp1.flatten(170);
	bmp1.findSectorsY(0,height,sectors);
	bmp1.findSectors(sectors);
	bmp1.write("out.bmp");	
}

*/