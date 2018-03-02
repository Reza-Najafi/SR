// SR.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <yuv_reader.h>
#include "yuvGlDraw.h"
using namespace std;

int main(int argc, char** argv)
{
    string input_file;
    if (argc < 2) {
        cin >> input_file;
    }
    else {
        input_file = argv[1];
    }
    int frameCount = 10;
    YUV_frame* frames = new YUV_frame[frameCount];
    
    int res = yuv4mpeg2mov(input_file, frames, frameCount);
    cout <<"Number of frames read from the file: " << frameCount << endl;
    int temp;
    imageWidth = frames[0].width;
    frames[0].convert2rgb();
    imageData = frames[0].rgb;
    imageHeight = frames[0].height;
    init();
    draw(argc,argv);
    return 0;
}