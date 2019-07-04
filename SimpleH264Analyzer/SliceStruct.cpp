#include "pch.h"
#include "SeqParamSet.h"
#include "PicParamSet.h"
#include "SliceStruct.h"
#include "SliceHeader.h"

#include <iostream>
using namespace std;

CSliceStruct::CSliceStruct(UINT8 *pSODB, CSeqParamSet *sps, CPicParamSet *pps, UINT8	nalType, UINT32 sliceIdx)
{

	this->pSODB = pSODB;
	sps_active = sps;
	pps_active = pps;
	this->nalType = nalType;

	sliceHeader = NULL;
	this->sliceIdx = sliceIdx;

}

CSliceStruct::~CSliceStruct()
{
	if (sliceHeader)
	{
		delete sliceHeader;
		sliceHeader = NULL;
	}
}

int CSliceStruct::Parse()
{
	UINT32 sliceHeaderLength = 0, macroblockOffset = 0;
	sliceHeader = new CSliceHeader(pSODB, sps_active, pps_active, nalType);
	macroblockOffset = sliceHeaderLength = sliceHeader->Parse_slice_header();
	sliceHeader->Dump_slice_header_info();

	if (sliceIdx != 0)
	{
		return -1;
	}


	return -1;
}

