#pragma once
class CSeqParamSet;
class CPicParamSet;
class CSliceHeader;

class CSliceStruct
{
public:
	CSliceStruct(UINT8	*pSODB, CSeqParamSet *sps, CPicParamSet *pps, UINT8	nalType, UINT32 sliceIdx);
	~CSliceStruct();

	int Parse();

	CSliceHeader *sliceHeader;
	CSeqParamSet *sps_active;
	CPicParamSet *pps_active;

private:
	UINT8	*pSODB;
	UINT8   nalType;
	UINT32  sliceIdx;

};

