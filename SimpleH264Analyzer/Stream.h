#pragma once

#include <iostream>
#include <vector>
class CStreamFile
{
public:
	CStreamFile(char* fileName);
	~CStreamFile();

	//open api
	int Parse_h264_bitstream();

private:
	FILE* inputFile;
	char* fileName;
	std::vector<uint8> nalVec;

	void file_info();
	void file_error(int idx);
	int find_nal_prefix();
	void ebsp_to_sodb();
};