#include "BMP.h"

//
// https://web.archive.org/web/20130806153902/http://www.etechplanet.com/codesnippets/computer-graphics-draw-a-line-using-bresenham-algorithm.aspx
//

BMP::BMP(const char *fname) {
    filename = fname;
    read(fname);
    //std::cout << bmp_info_header.bit_count << std::endl;
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

class WartoscParzysta {
    int i = 1;
public:
    bool operator() (int value) {
        return i++ % 3 ? false : true;
    }
};

BMP::BMP(BMP& bmp, int32_t width, int32_t height, int start_x, int start_y) : BMP(width, height, false) {
    long rowsize = bmp_info_header.width * bmp_info_header.bit_count / 8;

    for(int i=0;i<height;i++) {
        int start = bmp.get_pos(start_x, start_y+height-i);
        int end = bmp.get_pos(start_x+width, start_y+height-i);
        std::copy(bmp.data.begin()+start, bmp.data.begin()+end, (char*)data.data()+rowsize*i);
    }
}

void BMP::readParagraphs() {

}

/*
18398556
13798917
*/
void BMP::removeAlpha() {
    if (bmp_info_header.bit_count == 24) {
        std::cout << "[INFO] Alpha canal already removed: " << filename << std::endl;
        return;
    } else {
        std::vector<uint8_t> _data;
        for(int i=0; i<data.size(); i+=4) {
            _data.push_back(data.data()[i]);
            _data.push_back(data.data()[i+1]);
            _data.push_back(data.data()[i+2]);
        }
        //std::cout << data.size() << std::endl;
        //std::cout << _data.size() << std::endl;
        data.clear();
        data =_data;

        bmp_info_header.bit_count = 24;
        bmp_info_header.compression = 0;
        row_stride = bmp_info_header.width * 3;
        data.resize(row_stride * bmp_info_header.height);

        uint32_t new_stride = make_stride_aligned(4);
        file_header.file_size = file_header.offset_data + data.size() + bmp_info_header.height * (new_stride - row_stride);
    
        std::cout << "[INFO] Removed alpha canal from " << filename << std::endl;
    }
}

//  L  =  0.2126 × R   +   0.7152 × G   +   0.0722 × B 
void BMP::grayscale() {
    for(int i=0; i<data.size(); i+=3) {
        uint8_t r = data.data()[i];
        uint8_t g = data.data()[i+1];
        uint8_t b = data.data()[i+2];
        uint8_t l = 0.2126 * r   +   0.7152 * g   +   0.0722 * b;
        data.data()[i] = l;
        data.data()[i+1] = l;
        data.data()[i+2] = l;
    }
    std::cout << "[INFO] Converted " << filename << " into grayscale " << std::endl;
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
    std::cout << "[INFO] written into " << fname << std::endl;
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

/*
    usuń przerwy o wysokości jednego pixela
*/
void BMP::removeArtifacts(bool (*func)(Pixel p)) {
    Pixel pixel_1, pixel_2, pixel_3;
    for(int i=0; i<bmp_info_header.height-3; i++) {
        pixel_1 = get_pixel(0, i);
        pixel_2 = get_pixel(0, i+1);
        pixel_3 = get_pixel(0, i+2);
        //std::cout << i << " " << pixel << " isWhite?: " << func(pixel) << std::endl;
        if (func(pixel_1) == false && func(pixel_2) == true && func(pixel_3) == false) {
            draw_line(0,i+1,bmp_info_header.width-1,i+1,0,0,255);
        }
    }
}

void BMP::findThreshold(int threshold) {
    int index, max = 0;
    for(int color=0; color<255; color++) {
        int counter = 0;
        BMP bmp(filename);
        bmp.flatten(color);
        for(int i=0; i<bmp_info_header.width-1; i++) {
            if(get_pixel(i, 0).isWhite()) {
                counter++;
            }
            if(get_pixel(i, bmp_info_header.height-1).isWhite()) {
                counter++;
            }
        } 
        for(int j=0; j<bmp_info_header.height-1; j++) {
            if(get_pixel(0, j).isWhite()) {
                counter++;
            }
            if(get_pixel(bmp_info_header.width-1, 0).isWhite()) {
                counter++;
            }
        }
        std::cout << color << ": " << counter << " / " << bmp_info_header.height * bmp_info_header.width << std::endl;
        if(counter>max) {
            max = counter;
            index = color;
        }
    }
    threshold = index;
    std::cout << "------------" << std::endl;
    std::cout << max << " / " << bmp_info_header.height * bmp_info_header.width << std::endl;
    std::cout << threshold << std::endl;
}

void BMP::readParagraph(Sector s, Paragraph p) {
    // crop image
    // use tesseract to read (or sth else)
    // return text with image and all in Paragraph struct
}

void BMP::saveSectorToFile(Sector s, const char* _filename) {
    int w = (s.right - s.left);
    int h = (s.bottom - s.top);
    int start_x = s.left;
    int start_y = s.top;
    BMP bmp(*this, w, h, start_x, start_y);
    bmp.write(_filename);
}

// call only after colorY()
void BMP::countLines(std::vector<std::pair<int,int>> lines, bool (*func)(Pixel p)) {
    Pixel pixel = get_pixel(0, 0);
    int start = 0;
    bool tmp = func(pixel), curr;
    for(int i=1; i<bmp_info_header.height; i++) {
        pixel = get_pixel(0, i);
        //std::cout << i << " " << pixel << " isWhite?: " << func(pixel) << std::endl;
        curr = func(pixel);
        if (tmp == true && curr == false) {
            start = i;
        } else if (tmp == false && curr == true) {
            lines.push_back(std::pair<int,int>(start, i));
        }
        tmp = curr;
    }
    for(auto line : lines) {
        std::cout << line.first << " " << line.second << " " << line.second - line.first << std::endl;
    }
}

void BMP::flatten(int threshold) {
    std::replace_if(data.begin(), data.end(),
                    bind2nd(std::greater<int>(), threshold), 255);
    std::replace_if(data.begin(), data.end(),
                    bind2nd(std::less<int>(), threshold), 0);
    std::cout << "[INFO] Flattened " << filename << std::endl;
}

void BMP::histogram() {
    std::map<std::string, int> histogram;
    for (int i=0; i<data.size(); i+=3) {
        
        std::ostringstream out; 
        out << std::hex << std::setw(2) << std::setfill('0') << (int) data[i];
        out << std::hex << std::setw(2) << std::setfill('0') << (int) data[i+1];
        out << std::hex << std::setw(2) << std::setfill('0') << (int) data[i+2];
        std::string color = out.str();
        //std::cout << color << std::endl;
        
        auto it = histogram.find(color);
        if(it != histogram.end()) {
            //alert user key exists
            histogram[color]++;
        }
        else {
            //key doesn't exist
            histogram.insert({color, 1});
        }
    }

    for (auto itr = histogram.begin(); itr != histogram.end(); ++itr) { 
        std::cout << itr->first 
             << '\t' << itr->second << '\n'; 
    }
}

void BMP::colorY(int yStart, int yEnd) {
    int width = bmp_info_header.width;
    for(int i=yStart; i<yEnd; i++) {
        if(this->is_crossing(0,i,width-1,i) <= thr) {
            this->draw_line(0,i,width-1,i,0,0,255);
        }
    }
    std::cout << "[INFO] Applied ColorY to " << filename << std::endl;
}

void BMP::colorX(int xStart, int xEnd) {
    int height = bmp_info_header.height;
    for(int i=xStart; i<xEnd; i++) {
        if(this->is_crossing(i,0,i,height-1) <= thr) {
            this->draw_line(i,0,i,height-1,0,0,255);
        }
    }
}

int BMP::colorXxxxxLeft(int yStart, int yEnd) {
    int width = bmp_info_header.width;
    int tmp;
    for(int i=yStart; i<yEnd; i++) {
        for(int j=0; j<width-1; j++) {
            if(this->is_crossing(j,yStart,j,yEnd+1) == 0) {  // do weryfikacji dlaczego +1
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
            if(this->is_crossing(j,yStart,j,yEnd+1) == 0) {
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

void BMP::draw_line(int x1, int y1, int x2, int y2, int colR, int colG, int colB) {
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

int BMP::is_crossing(int x1, int y1, int x2, int y2) {
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    int points_of_crossing = 0;
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
            points_of_crossing++;
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
                points_of_crossing++;
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
            points_of_crossing++;
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
                points_of_crossing++;
            }
        }
    }
    return points_of_crossing;
}

void BMP::fragment(const char* dirname) {
    
}