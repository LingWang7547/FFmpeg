// NALUnit.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// NALUnit解析，从YUV数据中读取一段NAL Unit数据，并进行保存操作
// NAL Unit码流一般以00 00 00 01 xxxxxx 00 00 00 01 起始

#include "pch.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <iomanip>

typedef unsigned char uint8;

static int find_nal_prefix(FILE **pFileIn, std::vector<uint8> &nalBytes)
{
	FILE *pFile = *pFileIn;
	// 00 00 00 01 x x x x x 00 00 00 01
	uint8 prefix[3] = { 0 };
	uint8 fileByte;
	/*
	依次比较 [0][1][2] = {0 0 0};  若不是，将下一个字符放到[0]的位置 -> [1][2][0] = {0 0 0} ; 下次放到[1]的位置，以此类推
	找到三个连0之后，还需判断下一个字符是否为1， getc() = 1  -> 00 00 00 01
	以及判断 [0][1][2] = {0 0 1} -> [1][2][0] = {0 0 1} 等，若出现这种序列则表示找到文件头
	*/

	nalBytes.clear();

	// 标记当前文件指针位置以及标记查找的状态
	int pos = 0, getPrefix = 0;
	// 读取三个字节
	for (int idx = 0; idx < 3; idx++)
	{
		prefix[idx] = getc(pFile);
		nalBytes.push_back(prefix[idx]);
	}

	while (!feof(pFile))
	{
		if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 1))
		{
			// 0x 00 00 01 found
			getPrefix = 1;
			nalBytes.pop_back();
			nalBytes.pop_back();
			nalBytes.pop_back();
			break;
		}
		else if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 0))
		{
			if (1 == getc(pFile))
			{
				// 0x 00 00 00 01 found
				getPrefix = 2;
				nalBytes.pop_back();
				nalBytes.pop_back();
				nalBytes.pop_back();
				break;
			}
		}
		else
		{
			fileByte = getc(pFile);
			prefix[(pos++) % 3] = fileByte;
			nalBytes.push_back(fileByte);
		}
	}

	return getPrefix;
}

int main(int argc, char* argv[])
{
	FILE *pFileIn = fopen(argv[1], "rb");
	if (!pFileIn)
	{
		std::cout << "Error: Opening input file failed." << std::endl;
	}

	std::vector<uint8> nalBytes;
	find_nal_prefix(&pFileIn, nalBytes);
	find_nal_prefix(&pFileIn, nalBytes);
	for (int idx = 0; idx < nalBytes.size(); idx++)
	{
		printf("%x ", nalBytes.at(idx));
	}
	printf("\n");

	find_nal_prefix(&pFileIn, nalBytes);
	for (int idx = 0; idx < nalBytes.size(); idx++)
	{
		printf("%x ", nalBytes.at(idx));
	}
	printf("\n");
	fclose(pFileIn);
    std::cout << "Hello World!\n"; 
}

