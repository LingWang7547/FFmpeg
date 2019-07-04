#pragma once

#include <iostream>
#include <vector>

class CSeqParamSet;
class CPicParamSet;

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
	std::vector<UINT8> nalVec;

	void file_info();
	void file_error(int idx);
	int find_nal_prefix();
	void ebsp_to_sodb();

	void dump_NAL_type(UINT8 nalType);

	CSeqParamSet *sps;
	CPicParamSet *pps;
};