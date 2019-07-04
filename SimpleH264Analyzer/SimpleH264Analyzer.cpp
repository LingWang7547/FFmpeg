// SimpleH264Analyzer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "Stream.h"

int main(int argc, char* argv[])
{	
	CStreamFile h264Stream(argv[1]);

	h264Stream.Parse_h264_bitstream();
	std::cout << "Hello World!\n"; 
	return 0;
}

