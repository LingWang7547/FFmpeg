#pragma once

#include <iostream>
#include <vector>

class CSeqParamSet;
class CPicParamSet;
class CSliceStruct;

class CStreamFile
{
public:
	CStreamFile(char* fileName);
	~CStreamFile();

	//open api
	int Parse_h264_bitstream();
	void dump_NAL_type(UINT8 nalType);

private:
	FILE* inputFile;
	char* fileName;
	std::vector<UINT8> nalVec;

	void file_info();
	void file_error(int idx);
	int  find_nal_prefix();
	void ebsp_to_sodb();

	

	CSeqParamSet *sps;
	CPicParamSet *pps;
	CSliceStruct *IDRSlice;
};