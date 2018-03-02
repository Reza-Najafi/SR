#pragma once
#include <string>
#include <iostream>

class YUV_frame {
public:
    YUV_frame() {}
    YUV_frame(int width_, int height_) :
        width(width_),
        height(height_)
    {
        init(width, height);
    }
    void init(int width_, int height_) {
        width = width_;
        height = height_;
        if (initialized) { return; }
        yuv = new unsigned char**[height_];
        rgb = new unsigned char**[height_];
        for (int i = 0; i < height_; i++) {
            yuv[i] = new unsigned char*[width_];
            rgb[i] = new unsigned char*[width_];
            for (int j = 0; j < width_; j++) {
                yuv[i][j] = new unsigned char[NUM_CHNLS];
                rgb[i][j] = new unsigned char[NUM_CHNLS];
            }
        }
        initialized = true;
    }

    void convert2rgb() {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                
                rgb[i][j][2] = 1.164*((int)yuv[i][j][0] - 16) + 2.018*((int)yuv[i][j][1] - 128);

                rgb[i][j][1] = 1.164*((int)yuv[i][j][0] - 16) - 0.813*((int)yuv[i][j][2] - 128) - 0.391*((int)yuv[i][j][1] - 128);

                rgb[i][j][0] = 1.164*((int)yuv[i][j][0] - 16) + 1.596*((int)yuv[i][j][2] - 128);
            }
        }


    }
    void clear() {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (yuv[i][j]) delete[] yuv[i][j];
                if (rgb[i][j]) delete[] rgb[i][j];
            }
            if (yuv[i]) delete[] yuv[i];
            if (rgb[i]) delete[] rgb[i];
        }
        if (yuv) delete[] yuv;
        if (rgb) delete[] rgb;
        initialized = false;
    }
    ~YUV_frame() {
        clear();
    }
    unsigned char*** yuv = nullptr;
    unsigned char*** rgb = nullptr;
    const static int NUM_CHNLS = 3;
    int width = 0;
    int height = 0;
    bool initialized = false;

};

struct YUV_Header {
    bool valid;
    int width;
    int height;
    int frame_rate_num;
    int frame_rate_denom;
    char interlaced;
    int aspect_num;
    int aspect_denom;
    std::string color_space;
    void print() {
        std::cout << "Width " << width << " Height " << height << " framerate " << (float)frame_rate_num / (float)frame_rate_denom;
        std::cout << " interlaced " << interlaced << " aspect ratio " << (float)aspect_num / (float)aspect_denom;
        std::cout << " color space " << color_space << std::endl;
    }
};
int yuv4mpeg2mov(std::string file, YUV_frame*& frames, int& frameCount);
int readyuvframe(YUV_frame& yuv, char* cdata, int width, int height, float uvwF, float uvhF);