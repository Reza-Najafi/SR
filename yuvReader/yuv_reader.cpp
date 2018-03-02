// mscthesis.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
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
	string color_space;
	void print() {
		cout << "Width " << width << " Height " << height << " framerate " << (float)frame_rate_num / (float)frame_rate_denom ;
		cout << " interlaced " << interlaced << " aspect ratio " << (float)aspect_num / (float)aspect_denom;
		cout << " color space " << color_space <<endl;
	}
};

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
		memcpy(yuv.y[i], data + off, width);
		off += width;
	}
		
#if 0
	for (int i = 0; i < height*uvhF; i++) {
		for (int j = 0; j < width*uvwF; j++) {
			yuv.u[i][j] = data[off];
			off++;
		}
	}
#endif	
	return 0;
}

int yuv4mpeg2mov( string file) { 
	ifstream fs(file.c_str(), std::fstream::in); 

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
	int curr_pos = fs.gcount();
	fs.seekg(0, fs.end);
	int remaining = (int)fs.tellg() - curr_pos;
	fs.seekg(curr_pos);
	if (!fs.good()) { 
		return -1; 
	}
	int frameCount = remaining/ fLength;
	if (frameCount <= 0) {
		return -1;
	}
	YUV_frame* frames = new YUV_frame[frameCount];
	char* buff =  new char[fLength];
	int currFrame = 0;
	fs.get(buff, fLength);
	while (fs.good()) {
		readyuvframe(frames[currFrame], buff, hdr.width, hdr.height, uvwFactor,uvhFactor);
		currFrame++;
		fs.get(buff, fLength);
	} 
	delete[] buff;
	return 0;
}







int main(int argc, char** argv)
{
	string input_file;
	if (argc < 2) {
		cin >> input_file;
	}
	else {
		input_file = argv[1];
	}

	yuv4mpeg2mov(input_file);
	int temp;
	cin >> temp;
    return 0;
}

