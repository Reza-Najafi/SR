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
    YUV_frame* frames = nullptr;
    int frameCount = 0;
    int res = yuv4mpeg2mov(input_file, frames, frameCount);
    cout <<"Number of frames read from the file: " << frameCount << endl;
    int temp;
    makeCheckImage();
    init();
    display();
    cin >> temp;
    return 0;
}