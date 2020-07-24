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

template<char delimiter>
class WordDelimitedBy : public std::string
{};

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
	findAndReplaceAll(text, "***", "*");
	//ofs.write(text.c_str(), text.size());

	std::istringstream iss(text);
	std::vector<std::string> results;
	std::string word, paragraph;
	while(!iss.eof()) {

		//getline(iss, word);
		//std::cout << word << std::endl;

		getline(iss, word);
		if (word != "*") {
			//std::cout << word << std::endl;
			word += '\n';
			paragraph += word;
		} else {
			std::cout << paragraph << std::endl;
			paragraph = "";
		}



		/*
		if (word == "*") {
			while(word != "*" && !iss.eof()) {
				paragraph += word;
				std::cout << word << std::endl;
				getline(iss, word);
			}
		} else {
			std::cout << word << std::endl;
		}*/


		//std::cout << paragraph;
		//paragraph = "";

		/*
		ifs >> std::noskipws >> word;
		std::cout << "|||" << word << ":::";

		if (word == "*") {
			std::cout << word;
			word = "";
		}*/
	}


	/*
	std::istringstream iss(text, std::noskipws);
	std::vector<std::string> results((std::istream_iterator<WordDelimitedBy<'*'>>(iss)),
	                                  std::istream_iterator<WordDelimitedBy<'*'>>());	
	for (std::string word : results) {
	    ofs.write(word.c_str(), word.size());
	    ofs << " ";
	}
	*/

}

