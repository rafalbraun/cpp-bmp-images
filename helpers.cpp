#include <stdlib.h>

/*
int pdfSeparate(const char* filename, const char* dirname) {
	// we have to check if file is valid pdf
	system("pdfseparate " + filename + ".pdf " + dirname + " " + filename + "-%d.pdf");
	system("ls | wc -l");
}
*/

// tutaj zrobić string.format
const char* separatePage(const char* filename, int pageNumber) {
	std::string command = "convert -density 300 " + std::string(filename) + " tmp.bmp";
	system(command.c_str());
	return "tmp.bmp";
}

int pdfSeparate(std::string filename, std::string dirname) {
	// we have to check if file is valid pdf
	std::string command = "pdfseparate " + filename + " " + dirname + "/filename-%d.pdf";
	system(command.c_str());
}

int convertPdfToBmp(std::string filename, std::string output) {
	// check for extension, if it is there strip it
	std::string command = "convert -density 150 " + filename + " " + output;
	system(command.c_str());
}

int countFiles(std::string dirname) {
	FILE *fp;
	char count[256];

	std::string command = "/bin/ls " + dirname + " | wc -l";

	fp = popen(command.c_str(), "r");
	if (fp == NULL) {
		printf("Failed to run command\n");
		exit(1);
	}

	fgets(count, sizeof(count), fp);
	pclose(fp);

	return atoi(count);
}

std::string joinParagraphs(std::vector<Paragraph> paragraphs) {
	std::cout << "[INFO] Joined " << paragraphs.size() << " paragraphs." << std::endl;
	return "";
}

void printInfo(BMP bmp) {
	std::cout << "Info:" << std::endl;
	std::cout << "\t width: "  << bmp.bmp_info_header.width << std::endl;
	std::cout << "\t height: " << bmp.bmp_info_header.height << std::endl;
	std::cout << "\t bit_count: " << bmp.bmp_info_header.bit_count << std::endl;
	std::cout << "\t format: " << (float)bmp.bmp_info_header.width / (float)bmp.bmp_info_header.height << std::endl;	

	std::cout << "\t compressed: " << bmp.bmp_info_header.compression << std::endl;
}

void printSectors(std::vector<Sector>& sectors) {
	std::cout << "[INFO] Found " << sectors.size() << " sectors." << std::endl;
	for(auto it = sectors.begin(); it != sectors.end(); ++it) {
        std::cout << "(" << it->left << "," << it->right << "," << it->top << "," << it->bottom << ")" << std::endl;
    }
}

