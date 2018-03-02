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
    void init(int width, int height) {
        if (initialized) { return; }
        y = new unsigned char*[height];
        u = new unsigned char*[height];
        v = new unsigned char*[height];
        for (int i = 0; i < height; i++) {
            y[i] = new unsigned char[width];
            u[i] = new unsigned char[width];
            v[i] = new unsigned char[width];
        }
        initialized = true;
    }
    void clear() {
        for (int i = 0; i < height; i++) {
            if (y)delete[] y[i];
            if (u)delete[] u[i];
            if (v)delete[] v[i];
        }
        delete[] y;
        delete[] u;
        delete[] v;
        initialized = false;
    }
    ~YUV_frame() {
        clear();
    }
    unsigned char** y = nullptr;
    unsigned char** u = nullptr;
    unsigned char** v = nullptr;
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