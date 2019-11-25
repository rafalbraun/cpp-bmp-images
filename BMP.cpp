#include "BMP.h"

//
// https://web.archive.org/web/20130806153902/http://www.etechplanet.com/codesnippets/computer-graphics-draw-a-line-using-bresenham-algorithm.aspx
//

BMP::BMP(const char *fname) {
    read(fname);
    //std::cout << bmp_info_header.bit_count << std::endl;
}

BMP::BMP(int32_t width, int32_t height, bool has_alpha = true) {
    std::cout << "dupa" << std::endl;
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
    } else {
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

/*
198272−174323 = 23949
417 * 14 * 24 / 8 = 17514
*/
BMP::BMP(const char* fname, int32_t width, int32_t height, int32_t start, int32_t end) : BMP(width, height, false) {
    // this->data.resize(0);
    // std::cout << "start" << std::endl;
    // for( std::vector<uint8_t>::const_reverse_iterator it = bmp.data.rbegin()+198272; it != bmp.data.rbegin()+174323; it++ ) {
    //     //printf( "%d ", *it );
    //     this->data.push_back(*it);
    // }
    // std::cout << "end" << std::endl;
    long size = bmp_info_header.width * bmp_info_header.bit_count / 8;
    
    std::ifstream inp{ fname, std::ios_base::binary };
    data.resize(3*size);
    
    inp.seekg( -size, std::ios::end );
    inp.read((char*)data.data() + 2*size, data.size());

    // std::cout << "endzzzz" << std::endl;
}
/*
bool BMP::region(const unsigned int& x, const unsigned int& y, const unsigned int& width, const unsigned int& height, bitmap_image& dest_image) const {
    if ((x + width ) > width_ ) { return false; }
    if ((y + height) > height_) { return false; }
    
    if (
       (dest_image.width_  < width_ ) ||
       (dest_image.height_ < height_)
     ) {
     dest_image.setwidth_height(width,height);
    }

    for (unsigned int r = 0; r < height; ++r) {
        unsigned char* itr1     = row(r + y) + x * bytes_per_pixel_;
        unsigned char* itr1_end = itr1 + (width * bytes_per_pixel_);
        unsigned char* itr2     = dest_image.row(r);

        std::copy(itr1, itr1_end, itr2);
    }

    return true;
}*/

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

int BMP::get_pos(int row, int col) {
    this->validate_pixel(row, col);
    long size = data.size();
    int width = bmp_info_header.width;
    int trans = width - 1 - row;
    int start = size - 3*width*col - 3*trans -1;
    return start;
}

Pixel BMP::get_pixel(int row, int col) {
    this->validate_pixel(row, col);
    long size = data.size();
    int width = bmp_info_header.width;
    int trans = width - 1 - row;    
    return Pixel(data[size - 3*width*col - 3*trans -1],
                 data[size - 3*width*col - 3*trans -2],
                 data[size - 3*width*col - 3*trans -3]);
}

bool BMP::is_pixel_white(int row, int col) {
    this->validate_pixel(row, col);
    long size = data.size();
    int width = bmp_info_header.width;
    int trans = width - 1 - row;
    int r = data[size - 3*width*col - 3*trans -1];
    int g = data[size - 3*width*col - 3*trans -2];
    int b = data[size - 3*width*col - 3*trans -3];
    //std::cout << row << " , " << col << std::endl;
    return (r == 255 && g == 255 && b == 255) ? true : false;
}

void BMP::flatten() {
    std::replace_if(data.begin(), data.end(),
                    bind2nd(std::greater<int>(),240), 255);
}

void BMP::draw_line(int x1, int y1, int x2, int y2, int colR, int colG, int colB)
{
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = x2 - x1;
    dy = y2 - y1;
    dx1 = fabs(dx);
    dy1 = fabs(dy);
    px = 2 * dy1 - dx1;
    py = 2 * dx1 - dy1;
    if (dy1 <= dx1) {
        if (dx >= 0) {
            x = x1;
            y = y1;
            xe = x2;
        }
        else {
            x = x2;
            y = y2;
            xe = x1;
        }
        this->set_pixel(x, y, colR, colG, colB);
        for (i = 0; x < xe; i++) {
            x = x + 1;
            if (px < 0) {
                px = px + 2 * dy1;
            }
            else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    y = y + 1;
                }
                else {
                    y = y - 1;
                }
                px = px + 2 * (dy1 - dx1);
            }
            this->set_pixel(x, y, colR, colG, colB);
        }
    }
    else {
        if (dy >= 0) {
            x = x1;
            y = y1;
            ye = y2;
        }
        else {
            x = x2;
            y = y2;
            ye = y1;
        }
        this->set_pixel(x, y, colR, colG, colB);
        for (i = 0; y < ye; i++) {
            y = y + 1;
            if (py <= 0) {
                py = py + 2 * dx1;
            }
            else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    x = x + 1;
                }
                else {
                    x = x - 1;
                }
                py = py + 2 * (dx1 - dy1);
            }
            this->set_pixel(x, y, colR, colG, colB);
        }
    }
}

/*
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
}*/

bool BMP::is_crossing(int x1, int y1, int x2, int y2)
{
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = x2 - x1;
    dy = y2 - y1;
    dx1 = fabs(dx);
    dy1 = fabs(dy);
    px = 2 * dy1 - dx1;
    py = 2 * dx1 - dy1;
    if (dy1 <= dx1) {
        if (dx >= 0) {
            x = x1;
            y = y1;
            xe = x2;
        }
        else {
            x = x2;
            y = y2;
            xe = x1;
        }
        if (this->is_pixel_white(x, y) != true) {
            return true;
        }
        for (i = 0; x < xe; i++) {
            x = x + 1;
            if (px < 0) {
                px = px + 2 * dy1;
            }
            else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    y = y + 1;
                }
                else {
                    y = y - 1;
                }
                px = px + 2 * (dy1 - dx1);
            }
            if (this->is_pixel_white(x, y) != true) {
                return true;
            }
        }
    }
    else {
        if (dy >= 0) {
            x = x1;
            y = y1;
            ye = y2;
        }
        else {
            x = x2;
            y = y2;
            ye = y1;
        }
        if (this->is_pixel_white(x, y) != true) {
            return true;
        }
        for (i = 0; y < ye; i++) {
            y = y + 1;
            if (py <= 0) {
                py = py + 2 * dx1;
            }
            else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    x = x + 1;
                }
                else {
                    x = x - 1;
                }
                py = py + 2 * (dx1 - dy1);
            }
            if (this->is_pixel_white(x, y) != true) {
                return true;
            }
        }
    }
    return false;
}

void BMP::colorY(int yStart, int yEnd) {
    int width = bmp_info_header.width;
    for(int i=yStart; i<yEnd; i++) {
        if(this->is_crossing(0,i,width-1,i) != true) {
            this->draw_line(0,i,width-1,i,0,0,255);
        }
    }
}

void BMP::colorX(int xStart, int xEnd) {
    int height = bmp_info_header.height;
    for(int i=xStart; i<xEnd; i++) {
        if(this->is_crossing(i,0,i,height) != true) {
            this->draw_line(i,0,i,height,0,0,255);
        }
    }
}

int BMP::colorXxxxxLeft(int yStart, int yEnd) {
    // int width = bmp_info_header.width;
    // for(int i=yStart; i<yEnd; i++) {
    //     if(this->is_crossing(0,i,width-1,i) != true) {
    //         this->draw_line(0,i,width-1,i,255,255,0);
    //     }
    // }
    int width = bmp_info_header.width;
    int tmp;
    for(int i=yStart; i<yEnd; i++) {
        for(int j=0; j<width-1; j++) {
            if(this->is_crossing(j,yStart,j,yEnd+1) != true) {  // do weryfikacji dlaczego +1
                this->draw_line(j,yStart,j,yEnd+1,0,0,255);     // do weryfikacji dlaczego +1
                tmp = j;
            } else {
                return tmp;
            }
        }
    }
    return tmp;
}

int BMP::colorXxxxxRight(int yStart, int yEnd) {
    int width = bmp_info_header.width;
    int tmp;
    for(int i=yStart; i<yEnd-1; i++) {
        for(int j=width-1; j>0; j--) {
            if(this->is_crossing(j,yStart,j,yEnd+1) != true) {
                this->draw_line(j,yStart,j,yEnd+1,0,0,255);
                tmp = j;
            } else {
                return tmp;
            }
        }
    }
    return tmp;
}

void BMP::findSectorsY(int yStart, int yEnd, std::vector<Sector>& sectors) {
    int tmp;
    for(int i=yStart; i<yEnd-1; i++) {
        Pixel p1 = get_pixel(0, i);
        Pixel p2 = get_pixel(0, i+1);
        //std::cout << p1 << " \t\t| " << p2 << std::endl;
        if (p1.isBlack() && p2.isWhite()) {
            tmp = i;
        }
        if (p1.isWhite() && p2.isBlack()) {
            sectors.push_back(Sector(0, 0, tmp, i));
            //std::cout << "(" << tmp << "," << i << ",0,0)" << std::endl;
        }
    }
}

void BMP::findSectors(std::vector<Sector>& sectors) {
    int borderLeft=100, borderRight=100;
    for(auto it = sectors.begin(); it != sectors.end(); ++it) {
        borderLeft = colorXxxxxLeft(it->top+1, it->bottom-1);
        borderRight = colorXxxxxRight(it->top+1, it->bottom-1);
        if (borderLeft > 0) {
            it->left = borderLeft;
        }
        if (borderRight > 0) {
            it->right = borderRight;
        }
    }
}




















// void BMP::findSectorsX(int xStart, int xEnd, std::vector<Sector>& sectors) {
//     int tmp;
//     for(int i=xStart; i<xEnd; i++) {
//         Pixel p1 = get_pixel(0, i);
//         Pixel p2 = get_pixel(0, i+1);
//         if (p1.isBlack() && p2.isWhite()) {
//             tmp = i;
//         }
//         if (p1.isWhite() && p2.isBlack()) {
//             sectors.push_back(Sector(0, 0, tmp, i)); 
//         }
//     }
// }










/*
bool BMP::IsSectorStartY(int index) {
    return (this->is_pixel_white(0,index) == false && this->is_pixel_white(0,index+1) == true);
}
bool BMP::IsSectorEndY(int index) {
    return (this->is_pixel_white(0,index) == true && this->is_pixel_white(0,index+1) == false);
}
bool BMP::IsSectorStartX(int index) {
    return (this->is_pixel_white(index,0) == false && this->is_pixel_white(index+1,0) == true);
}
bool BMP::IsSectorEndX(int index) {
    return (this->is_pixel_white(index,0) == true && this->is_pixel_white(index+1,0) == false);
}

Sectors* BMP::discover_sectors_y() {
    Sectors* sectors_ptr = new Sectors();
    int height = bmp_info_header.height;
    int sec_start = is_pixel_white(0,0);
    for(int i=1; i<height-1; i++) {
        if (IsSectorStartY(i)) {
            sec_start = i;
        } else if (IsSectorEndY(i)) {
            Pair pair(sec_start,i);
            sectors_ptr->push_back(pair);
        }
    }
    return sectors_ptr;
}

Sectors* BMP::discover_sectors_x() {
    Sectors* sectors_ptr = new Sectors();
    int height = bmp_info_header.height;
    int sec_start = is_pixel_white(0,0);
    for(int i=1; i<height-1; i++) {
        if (IsSectorStartX(i)) {
            sec_start = i;
        } else if (IsSectorEndX(i)) {
            Pair pair(sec_start,i);
            sectors_ptr->push_back(pair);
        }
    }
    return sectors_ptr;
}
*/