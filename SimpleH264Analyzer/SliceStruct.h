#pragma once
class CSeqParamSet;
class CPicParamSet;
class CSliceHeader;
class CMacroBlock;


class CSliceStruct
{
public:
	CSliceStruct(UINT8	*pSODB, CSeqParamSet *sps, CPicParamSet *pps, UINT8	nalType, UINT32 sliceIdx);
	~CSliceStruct();

	int Parse();
	//CMacroblock *Get_macroblock_at_index(int mbIdx);

	CSliceHeader *sliceHeader;
	CSeqParamSet *sps_active;
	CPicParamSet *pps_active;

private:
	UINT8	*pSODB;
	UINT8   nalType;
	UINT32  sliceIdx;

	UINT16 max_mb_number;
	CMacroBlock **macroblocks;
};

