#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <algorithm>

/*
int pdfSeparate(const char* filename, const char* dirname) {
	// we have to check if file is valid pdf
	system("pdfseparate " + filename + ".pdf " + dirname + " " + filename + "-%d.pdf");
	system("ls | wc -l");
}
*/

int readPagesNumber(const char* filename) {
	return 0;
}

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

void findAndReplaceAll(std::string& data, std::string toSearch, std::string replaceStr)
{
    // Get the first occurrence
    size_t pos = data.find(toSearch);
    // Repeat till end is reached
    while( pos != std::string::npos)
    {
        // Replace this occurrence of Sub String
        data.replace(pos, toSearch.size(), replaceStr);
        // Get the next occurrence from the current position
        pos =data.find(toSearch, pos + replaceStr.size());
    }
}

static std::string STAR  = "*";
static std::string EQUAL = "=";

/*
	wraz z * możemy umieścić info o formatowaniu paragrafu, w tej funkcji możemy sparsować to info
*/
void parseFormatting(std::string word, std::string& format) {
	format = "::format::" + word;
}

void extractText(const char* inputFilename, const char* outputFilename) {
	std::ifstream ifs{ inputFilename, std::ios_base::binary };
	std::ofstream ofs{ outputFilename, std::ios_base::binary };
    if (!ofs) {
        throw std::runtime_error("ofs error");
    }
    if (!ifs) {
        throw std::runtime_error("ifs error");
    }

	std::string text((std::istreambuf_iterator<char>(ifs)),
	                  std::istreambuf_iterator<char>());
	findAndReplaceAll(text, "***", STAR );

	std::istringstream iss(text);
	std::vector<std::string> results;
	std::string word, paragraph, format;
	while(!iss.eof()) {

		getline(iss, word);
		if (word != STAR) {
			paragraph = paragraph + word + '\n';
		} else {
			ofs << paragraph;
			paragraph = "";
			//std::cout << "---" << std::endl;
		}
	}
}

void extractTextWithFormatting(const char* inputFilename, const char* outputFilename) {
	std::ifstream ifs{ inputFilename, std::ios_base::binary };
	std::ofstream ofs{ outputFilename, std::ios_base::binary };
    if (!ofs) {
        throw std::runtime_error("ofs error");
    }
    if (!ifs) {
        throw std::runtime_error("ifs error");
    }

	std::string text((std::istreambuf_iterator<char>(ifs)),
	                  std::istreambuf_iterator<char>());
	findAndReplaceAll(text, "***", STAR );

	std::istringstream iss(text);
	std::vector<std::string> results;
	std::string word, paragraph, format;
	while(!iss.eof()) {

		getline(iss, word);
		if (word != STAR) {
			paragraph = paragraph + word + '\n';
		} else {
			getline(iss, word);
			parseFormatting(word, format);

			ofs << paragraph;
			paragraph = "";
			ofs << format << std::endl;
			ofs << "---" << std::endl;
		}
	}

}

bool compareFiles(const char* inputFilename1, const char* inputFilename2) {
	std::ifstream ifs1{ inputFilename1, std::ios_base::binary },
				  ifs2{ inputFilename2, std::ios_base::binary };

	std::string word1, word2;
	while(!ifs1.eof() && !ifs2.eof()) {
		getline(ifs1, word1);
		getline(ifs2, word2);
		if (word1 != word2) {
			return false;
		}
	}
	return true;
}