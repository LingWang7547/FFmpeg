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

	CSliceHeader *m_sliceHeader;

	int Parse();
	CMacroBlock *Get_macroblock_at_index(int mbIdx);

	CSeqParamSet *m_sps_active;
	CPicParamSet *m_pps_active;

private:
	UINT8	*m_pSODB;
	UINT8   m_nalType;
	UINT32  m_sliceIdx;

	UINT16 m_max_mb_number;
	CMacroBlock **m_macroblocks;
};
