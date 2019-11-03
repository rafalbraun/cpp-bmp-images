#pragma once
#include <fstream>
#include <vector>
#include <stdexcept>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t file_type{ 0x4D42 };          // File type always BM which is 0x4D42 (stored as hex uint16_t in little endian)
    uint32_t file_size{ 0 };               // Size of the file (in bytes)
    uint16_t reserved1{ 0 };               // Reserved, always 0
    uint16_t reserved2{ 0 };               // Reserved, always 0
    uint32_t offset_data{ 0 };             // Start position of pixel data (bytes from the beginning of the file)
};

struct BMPInfoHeader {
    uint32_t size{ 0 };                      // Size of this header (in bytes)
    int32_t width{ 0 };                      // width of bitmap in pixels
    int32_t height{ 0 };                     // width of bitmap in pixels
                                             //       (if positive, bottom-up, with origin in lower left corner)
                                             //       (if negative, top-down, with origin in upper left corner)
    uint16_t planes{ 1 };                    // No. of planes for the target device, this is always 1
    uint16_t bit_count{ 0 };                 // No. of bits per pixel
    uint32_t compression{ 0 };               // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
    uint32_t size_image{ 0 };                // 0 - for uncompressed images
    int32_t x_pixels_per_meter{ 0 };
    int32_t y_pixels_per_meter{ 0 };
    uint32_t colors_used{ 0 };               // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
    uint32_t colors_important{ 0 };          // No. of colors used for displaying the bitmap. If 0 all colors are required
};

struct BMPColorHeader {
    uint32_t red_mask{ 0x00ff0000 };         // Bit mask for the red channel
    uint32_t green_mask{ 0x0000ff00 };       // Bit mask for the green channel
    uint32_t blue_mask{ 0x000000ff };        // Bit mask for the blue channel
    uint32_t alpha_mask{ 0xff000000 };       // Bit mask for the alpha channel
    uint32_t color_space_type{ 0x73524742 }; // Default "sRGB" (0x73524742)
    uint32_t unused[16]{ 0 };                // Unused data for sRGB color space
};
#pragma pack(pop)

class Pair : public std::pair<int,int> {
public: 
    Pair(int _first, int _second) : std::pair<int,int>(_first, _second) {}
    friend std::ostream& operator<<(std::ostream &out, const Pair &p) {
        std::cout << p.first << "," << p.second;
        return out;
    }
};
struct Sectors : public std::vector<Pair> {};
typedef std::vector<Pair>::iterator SectorsIterator;

struct BMP {
    BMPFileHeader file_header;
    BMPInfoHeader bmp_info_header;
    BMPColorHeader bmp_color_header;
    std::vector<uint8_t> data;
    
    BMP(const char *fname);
    BMP(int32_t width, int32_t height, bool has_alpha);
    void read(const char *fname);
    void write(const char *fname);
    void set_pixel(int row, int col, uint32_t r, uint32_t g, uint32_t b);
    void draw_line(int y0, int y1, int x0, int x1);
    void flatten();
    bool is_crossing(int x0, int y0, int x1, int y1);
    bool is_pixel_white(int row, int col);
    void color();
    Sectors* discover_sectors();
    bool IsSectorStart(int index);
    bool IsSectorEnd(int index);

private:
    uint32_t row_stride{ 0 };

    void write_headers(std::ofstream &of) {
        of.write((const char*)&file_header, sizeof(file_header));
        of.write((const char*)&bmp_info_header, sizeof(bmp_info_header));
        if(bmp_info_header.bit_count == 32) {
            of.write((const char*)&bmp_color_header, sizeof(bmp_color_header));
        }
    }

    void write_headers_and_data(std::ofstream &of) {
        write_headers(of);
        of.write((const char*)data.data(), data.size());
    }

    // Add 1 to the row_stride until it is divisible with align_stride
    uint32_t make_stride_aligned(uint32_t align_stride) {
        uint32_t new_stride = row_stride;
        while (new_stride % align_stride != 0) {
            new_stride++;
        }
        return new_stride;
    }

    // Check if the pixel data is stored as BGRA and if the color space type is sRGB
    void check_color_header(BMPColorHeader &bmp_color_header) {
        BMPColorHeader expected_color_header;
        if(expected_color_header.red_mask != bmp_color_header.red_mask ||
            expected_color_header.blue_mask != bmp_color_header.blue_mask ||
            expected_color_header.green_mask != bmp_color_header.green_mask ||
            expected_color_header.alpha_mask != bmp_color_header.alpha_mask) {
            throw std::runtime_error("Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
        }
        if(expected_color_header.color_space_type != bmp_color_header.color_space_type) {
            throw std::runtime_error("Unexpected color space type! The program expects sRGB values");
        }
    }

    void validate_pixel(int row, int col) {
        if (row < 0 || row >= bmp_info_header.width) {
            throw std::runtime_error("Wrong coordinates, the x coord is either too large or too small.");
        }
        if (col < 0 || col >= bmp_info_header.height) {
            throw std::runtime_error("Wrong coordinates, the y coord is either too large or too small.");
        }
    }
};


/*
    struct Pixel {
        uint8_t r,g,b,a;

        Pixel() {}
        Pixel(uint8_t B, uint8_t G, uint8_t R) 
        {
            this->r = R;
            this->g = G;
            this->b = B;
        }
        friend std::ostream& operator<<(std::ostream &out, const Pixel &p)
        {
            //out << p.R << "," << p.G << "," << p.B << "|";
            //std::cout << "R:0x" << std::setfill('0') << std::setw(2) << std::hex << unsigned(p.R) << " ";
            //std::cout << "G:0x" << std::setfill('0') << std::setw(2) << std::hex << unsigned(p.G) << " ";
            //std::cout << "B:0x" << std::setfill('0') << std::setw(2) << std::hex << unsigned(p.B) << std::endl;
            std::cout << (int)p.r << "," << (int)p.g << "," << (int)p.b << std::endl;
            return out;
        }
    };

*/