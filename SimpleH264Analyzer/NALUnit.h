#pragma once

class CSeqParamSet;
class CPicParamSet;

class CNALUnit
{
public:
	CNALUnit(UINT8	*pSODB, UINT32	SODBLength, UINT8 nalType);
	~CNALUnit();

	//sps
	int Parse_as_seq_param_set(CSeqParamSet *sps);
	//pps
	int Parse_as_pic_param_set(CPicParamSet *pps);

	UINT8* Get_SODB();

private:
	UINT8	*m_pSODB;
	UINT32	m_SODBLength;

	UINT8	m_nalType;
};

