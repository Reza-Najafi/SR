#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <yuv_reader.h>
using namespace std;

YUV_Header readYuv4MpegHeader( string& input)
{
	istringstream header(input);

	YUV_Header hdr;
	hdr.valid = false;
	hdr.color_space = "420";
	//parts = strsplit(char(header{ 1 }), ' ');
	string str;
	header >> str;

	if (str.compare("YUV4MPEG2") != 0) {
		return hdr;
	}
	hdr.valid = true;

	while (header >> str) {
		char c;
		istringstream istr(str);
		istr >> c;
		if (c == 'W') {
			istr >> hdr.width;
		}
		if (c == 'H') {
			istr >> hdr.height;
		}
		if (c == 'F') {
			char skip;
			istr >> hdr.frame_rate_num;
			istr >> skip;
			istr >> hdr.frame_rate_denom;
		}
		if (c == 'I') {
			istr >> hdr.interlaced;
		}
		if (c == 'A') {
			char skip;
			istr >> hdr.aspect_num;
			istr >> skip;
			istr >> hdr.aspect_denom;
		}
		if (c == 'C') {
			hdr.color_space.clear();
			istr >> hdr.color_space;
		}
	}
	return hdr;
}

int readyuvframe(YUV_frame& yuv, char* cdata, int width, int height, float uvwF, float uvhF)
{
	unsigned char* data = (unsigned char*)cdata;
	int uvwidth = width * uvwF;
	int uvheight = height * uvhF;
	yuv.init(width, height);
	int off = 0;
	for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            yuv.yuv[i][j][0] = data[off];
            off++;
        }
	}
		
#if 1 // only works for 420 currently
	for (int i = 0; i < height*uvhF; i++) {
		for (int j = 0; j < width*uvwF; j++) {
            yuv.yuv[2 * i][2 * j][1] = data[off];
            yuv.yuv[2 * i + 1][2 * j][1] = data[off];
            yuv.yuv[2 * i][2 * j + 1][1] = data[off];
            yuv.yuv[2 * i + 1][2 * j + 1][1] = data[off];
			off++;
		}
	}
    for (int i = 0; i < height*uvhF; i++) {
        for (int j = 0; j < width*uvwF; j++) {
            yuv.yuv[2 * i][2 * j][2] = data[off];
            yuv.yuv[2 * i + 1][2 * j][2] = data[off];
            yuv.yuv[2 * i][2 * j + 1][2] = data[off];
            yuv.yuv[2 * i + 1][2 * j + 1][2] = data[off];
            off++;
        }
    }
#endif	
	return 0;
}

int yuv4mpeg2mov( string file, YUV_frame*& frames, int& frameCount) {
	ifstream fs(file.c_str(), std::fstream::binary); 

	if (!fs.good()) {
		return -1;
	}

	string header;
	getline(fs, header);
	YUV_Header hdr = readYuv4MpegHeader(header);
	hdr.print();
	int fLength = hdr.width* hdr.height;
	float uvwFactor = 0.5 ,uvhFactor = 0.5;
	if (hdr.color_space.compare("420") == 0) {
		uvwFactor = 0.5;
		uvhFactor = 0.5;
		fLength = (fLength * 3)  / 2;// 1 uv sample per 4 y
	}
	if (hdr.color_space.compare("422") == 0) {
		uvwFactor = 0.5;
		uvhFactor = 1;
		fLength = fLength * 2;// 1 uv sample per 2 y
	}
	if (hdr.color_space.compare("444") == 0) {
		uvwFactor = 1;
		uvhFactor = 1;
		fLength = fLength * 3;// 1uv sample per y
	}
	string str;
	fs >> str;
	if (str.compare("FRAME") != 0) {
		return -1;
	}
    
	int curr_pos = fs.tellg();
	fs.seekg(0, fs.end);
	int remaining = (int)fs.tellg() - curr_pos;
	fs.seekg(curr_pos);
	if (!fs.good()) { 
		return -1; 
	}
	int maxFrameCount = remaining/ fLength;
	if (maxFrameCount <= 0 || frameCount > maxFrameCount) {
		return -1;
	}
    
	
	char* buff =  new char[fLength];
	int currFrame = 0;
	while (fs.read(buff, fLength) && (fs.rdstate() == 0) && currFrame< frameCount) {
        frames[currFrame].init(hdr.width, hdr.height);
		readyuvframe(frames[currFrame], buff, hdr.width, hdr.height, uvwFactor,uvhFactor);
		currFrame++;
	} 
	delete[] buff;
	return 0;
}

