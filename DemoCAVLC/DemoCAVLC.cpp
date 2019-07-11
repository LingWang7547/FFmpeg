// DemoCAVLC.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#include "CAVLC.h"
#include <string>
using namespace std;

int main()
{
	int coeffs[16] = { 3, 2, 1, -1, 0, -1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	string cavlc_code = Encoding_cavlc_16x16(coeffs);
	std::cout << cavlc_code << endl;
	return 0;
}