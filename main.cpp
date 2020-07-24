#include <iostream>
#include <iterator>
#include "BMP.h"
//#include "tesseract.hpp"

#include "helpers.cpp"
//#include "test.cpp"

/*
//
// g++ BMP.cpp -c -o BMP.o && g++ -c main.cpp -o main.o && g++ main.o BMP.o
//

pdfseparate -f 1 -l 1 sample.pdf sample-1.pdf
pdfseparate sample.pdf sample-%d.pdf

pdfseparate "/home/vanqyard/Dokumenty/Ibn Ishaq - Sirat Rasul Allah.pdf" ./sample-%d.pdf
pdfseparate "/home/vanqyard/Dokumenty/Świat powojenny i Polska.pdf" ./sample-%d.pdf
pdfseparate "/home/vanqyard/Dokumenty/Wprowadzenie do algorytmow - Thomas Cormen.pdf" ./sample-%d.pdf

convert -density 150 sample-25.pdf output.bmp
convert -density 150 sample[25].pdf output.bmp

convert -density 150  sample-25.pdf output.png
convert output.png -background white -flatten output.png 

*/



/**
 * 1 read file
 * 2 detect sectors
 * 3 write separate fiels (or read buffers?)
 * 4 join into one txt file
 * 5 write beautiful pdf (or tex)
 *
 * formats: chm, epub, pdf, djvu, aws3, cbz,
 */
/*
makefile cpp multiple main
*/
int main(int argc, char* argv[]) {
	//mainTest();

	//extractText("./text1.txt", "out.txt");
	//extractTextWithFormatting("./text2.txt", "out.txt");
	return 0;

	//pdfSeparate("./documents/rozmowa.pdf", "./tmp");
	//int count = countFiles("./tmp");
	//convertPdfToBmp("./images/filename-20.bmp", "./output.bmp");

	//std::vector<Sector> sectors;
	//const char* filename = (argc == 2) ? argv[1] : "decline.bmp";

	//BMP bmp("./images/aaa.bmp");
	std::vector<std::pair<int,int>> strips;
	BMP bmp("./documents/Preface_clear.bmp");
	bmp.removeAlpha();
	bmp.flatten(150);
	//bmp.colorX(0, bmp.bmp_info_header.width);
	bmp.colorY(0, bmp.bmp_info_header.height);
	bmp.stripsY(0, bmp.bmp_info_header.height, strips);

	int tmp = -1;
	for(auto iter : strips) {
		if (tmp > 0) {
			std::cout << tmp << " - " << iter.first << " : " << iter.first - tmp << std::endl;
		}
		
		//std::cout << iter.first << " " << iter.second << " : " << iter.second - iter.first << std::endl;
		
		int diff = iter.first - tmp;
		if (diff < 25) {
			std::cout << "diff: " << diff << std::endl;
			bmp.fillWithColor(0, tmp, bmp.bmp_info_header.width, iter.first, 255,255,255);
		}

		tmp = iter.second;
	}

	//bmp.fragment("./tmp");
	bmp.write("out.bmp");
	return 0;

	// (216, 78)
	// (1309, 78)

	//convertPdfToBmp("./dir2/sample-10");

	/*
	BMP bmp("input.bmp");
	bmp.removeAlpha();
	bmp.flatten(150);
	bmp.write("out.bmp");
	*/

	/*
	const char* filename = "images/cccccc.bmp";
	BMP bmp(filename);
	bmp.grayscale();
	bmp.write("images/out.bmp");
	*/


	//printInfo(bmp);

	//std::cout << ocr(filename, "pol");


	// bmp.findSectorsY(0, height, sectors);
	// bmp.findSectors(sectors);
	// printSectors(sectors);
	
	/*
	for (int i=1; i<sectors.size(); i++) {
		//const char* fname = ("tmp_" + std::to_string(i) + ".bmp").c_str();
		//std::cout << fname << std::endl;
		bmp.cropToFile(sectors[i], "tmp.bmp");
		std::cout << ocr("tmp.bmp", "pol");
	}*/

	//testSaveSectorToFile(sectors[3], "out.bmp", "tmp.bmp");

	//test4(argv[1], 100);
	//test10(argv[1], sectors);

	//testThreshold(argv[1]);
	//test2(argv[1]);

	//const char* pagename = separatePage(argv[1], 0);
	//testRemoveAlpha(argv[1]);

	//test4(argv[1], 130);
	//grayscaleTest(argv[1]);
	//test1(argv[1]);
	//test5("output.bmp", 1020, 1080);
	//mainTest(argv[1]);
	return 0;
}
















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
	//test6("sdsd.bmp", 0, 408, sectors);
	//test4("sdsd.bmp");

