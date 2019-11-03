#include "BMP.h"

BMP::BMP(const char *fname) {
    read(fname);
}

BMP::BMP(int32_t width, int32_t height, bool has_alpha = true) {
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("The image width and height must be positive numbers.");
    }

    bmp_info_header.width = width;
    bmp_info_header.height = height;
    if (has_alpha) {
        bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
        file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);

        bmp_info_header.bit_count = 32;
        bmp_info_header.compression = 3;
        row_stride = width * 4;
        data.resize(row_stride * height);
        file_header.file_size = file_header.offset_data + data.size();
    }
    else {
        bmp_info_header.size = sizeof(BMPInfoHeader);
        file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

        bmp_info_header.bit_count = 24;
        bmp_info_header.compression = 0;
        row_stride = width * 3;
        data.resize(row_stride * height);

        uint32_t new_stride = make_stride_aligned(4);
        file_header.file_size = file_header.offset_data + data.size() + bmp_info_header.height * (new_stride - row_stride);
    }
}

void BMP::read(const char *fname) {
    std::ifstream inp{ fname, std::ios_base::binary };
    if (!inp) {
        throw std::runtime_error("Unable to open the input image file.");
    }
    inp.read((char*)&file_header, sizeof(file_header));
    if(file_header.file_type != 0x4D42) {
        throw std::runtime_error("Error! Unrecognized file format.");
    }
    inp.read((char*)&bmp_info_header, sizeof(bmp_info_header));

    // The BMPColorHeader is used only for transparent images
    if(bmp_info_header.bit_count == 32) {
        // Check if the file has bit mask color information
        if(bmp_info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) {
            inp.read((char*)&bmp_color_header, sizeof(bmp_color_header));
            // Check if the pixel data is stored as BGRA and if the color space type is sRGB
            check_color_header(bmp_color_header);
        } else {
            std::cerr << "Error! The file \"" << fname << "\" does not seem to contain bit mask information\n";
            throw std::runtime_error("Error! Unrecognized file format.");
        }
    }
    
    // Jump to the pixel data location
    inp.seekg(file_header.offset_data, inp.beg);

    // Adjust the header fields for output.
    // Some editors will put extra info in the image file, we only save the headers and the data.
    if(bmp_info_header.bit_count == 32) {
        bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
        file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
    } else {
        bmp_info_header.size = sizeof(BMPInfoHeader);
        file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    }
    file_header.file_size = file_header.offset_data;

    if (bmp_info_header.height < 0) {
        throw std::runtime_error("The program can treat only BMP images with the origin in the bottom left corner!");
    }

    data.resize(bmp_info_header.width * bmp_info_header.height * bmp_info_header.bit_count / 8);

    // Here we check if we need to take into account row padding
    if (bmp_info_header.width % 4 == 0) {
        inp.read((char*)data.data(), data.size());
        file_header.file_size += data.size();
    }
    else {
        row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
        uint32_t new_stride = make_stride_aligned(4);
        std::vector<uint8_t> padding_row(new_stride - row_stride);

        for (int y = 0; y < bmp_info_header.height; ++y) {
            inp.read((char*)(data.data() + row_stride * y), row_stride);
            inp.read((char*)padding_row.data(), padding_row.size());
        }
        file_header.file_size += data.size() + bmp_info_header.height * padding_row.size();
    }
}

void BMP::write(const char *fname) {
    std::ofstream of{ fname, std::ios_base::binary };
    if (!of) {
        throw std::runtime_error("The program can treat only 24 or 32 bits per pixel BMP files");
    }
    if (bmp_info_header.bit_count != 32 && bmp_info_header.bit_count != 24) {
        throw std::runtime_error("Unable to open the output image file.");
    }
    if (bmp_info_header.bit_count == 32) {
        write_headers_and_data(of);
    }
    if (bmp_info_header.bit_count == 24) {
        if (bmp_info_header.width % 4 == 0) {
            write_headers_and_data(of);
        }
        else {
            uint32_t new_stride = make_stride_aligned(4);
            std::vector<uint8_t> padding_row(new_stride - row_stride);
            
            write_headers(of);
             
            for (int y = 0; y < bmp_info_header.height; ++y) {
                of.write((const char*)(data.data() + row_stride * y), row_stride);
                of.write((const char*)padding_row.data(), padding_row.size());
            }
        }
    }
}

void BMP::set_pixel(int row, int col, uint32_t r, uint32_t g, uint32_t b) {
    this->validate_pixel(row, col);
    long size = data.size();
    int width = bmp_info_header.width;
    int trans = width - 1 - row;
    data[size - 3*width*col - 3*trans -1] = r;
    data[size - 3*width*col - 3*trans -2] = g;
    data[size - 3*width*col - 3*trans -3] = b;
}

bool BMP::is_pixel_white(int row, int col) {
    this->validate_pixel(row, col);
    long size = data.size();
    int width = bmp_info_header.width;
    int trans = width - 1 - row;
    int r = data[size - 3*width*col - 3*trans -1];
    int g = data[size - 3*width*col - 3*trans -2];
    int b = data[size - 3*width*col - 3*trans -3];
    return (r == 255 && g == 255 && b == 255) ? true : false;
}

void BMP::flatten() {
    std::replace_if(data.begin(), data.end(),
                    bind2nd(std::greater<int>(),240), 255);
}

void BMP::draw_line(int x0, int y0, int x1, int y1) {
    int dx, dy, p, x, y;

    dx = x1-x0;
    dy = y1-y0;

    x = x0;
    y = y0;

    p = 2*dy-dx;

    while(x<x1) {
        if (p>=0) {
            this->set_pixel(x,y,0,0,255);
            y=y+1;
            p=p+2*dy-2*dx;
        } else {
            this->set_pixel(x,y,0,0,255);
            p=p+2*dy;           
        }
        x=x+1;
    }
}

bool BMP::is_crossing(int x0, int y0, int x1, int y1) {
    int dx, dy, p, x, y;

    dx = x1-x0;
    dy = y1-y0;

    x = x0;
    y = y0;

    p = 2*dy-dx;

    while(x<x1) {
        if (p>=0) {
            if (this->is_pixel_white(x,y) != true) {
                return true;
            }
            y=y+1;
            p=p+2*dy-2*dx;
        } else {
            if (this->is_pixel_white(x,y) != true) {
                return true;
            }
            p=p+2*dy;           
        }
        x=x+1;
    }
    return false;
}

void BMP::color() {
    int width = bmp_info_header.width;
    int height = bmp_info_header.height;

    for(int i=0; i<height; i++) {
        if(this->is_crossing(0,i,width,i) != true) {
            this->draw_line(0,i,width,i);
        }
    }
}

bool BMP::IsSectorStart(int index) {
    return (this->is_pixel_white(0,index) == false && this->is_pixel_white(0,index+1) == true);
}
bool BMP::IsSectorEnd(int index) {
    return (this->is_pixel_white(0,index) == true && this->is_pixel_white(0,index+1) == false);
}

Sectors* BMP::discover_sectors() {
    Sectors* sectors_ptr = new Sectors();
    int height = bmp_info_header.height;
    int sec_start = is_pixel_white(0,0);
    for(int i=1; i<height-1; i++) {
        if (IsSectorStart(i)) {
            sec_start = i;
        } else if (IsSectorEnd(i)) {
            Pair pair(sec_start,i);
            sectors_ptr->push_back(pair);
        }
        // if(is_pixel_white(0,i) == true) {
        //     std::cout << "true " << i << std::endl;
        // } else {
        //     std::cout << "false " << i << std::endl;            
        // }
    }
    //Pair pair(1,1);
    //sectors_ptr->push_back(pair);
    return sectors_ptr;
}
