#include "pch.h"
#include "SeqParamSet.h"
#include "PicParamSet.h"
#include "SliceStruct.h"
#include "SliceHeader.h"
#include "Macroblock.h"

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

	max_mb_number = sps_active->Get_pic_width_in_mbs() * sps_active->Get_pic_height_in_mbs();
	macroblocks = new CMacroBlock *[max_mb_number];
	memset(macroblocks, NULL, max_mb_number * sizeof(CMacroBlock *));
}

CSliceStruct::~CSliceStruct()
{
	if (sliceHeader != NULL)
	{
		delete sliceHeader;
		sliceHeader = NULL;
	}
	if (macroblocks)
	{
		for (int idx = 0; idx < max_mb_number; idx++)
		{
			if (macroblocks[idx])
			{
				delete macroblocks[idx];
				macroblocks[idx] = NULL;
			}
		}

		delete macroblocks;
		macroblocks = NULL;
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
	for (int idx = 0; idx < max_mb_number; idx++)
	{
		macroblocks[idx] = new CMacroBlock(pSODB, macroblockOffset, idx);

		macroblocks[idx]->Set_pic_param_set(pps_active);
		macroblocks[idx]->Set_slice_struct(this);

		macroblockOffset += macroblocks[idx]->Parse_macroblock();
		//macroblocks[idx]->Decode_macroblock();
		break;
	}
	return -1;
}

