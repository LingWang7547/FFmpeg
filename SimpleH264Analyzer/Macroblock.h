#pragma once
class CPicParamSet;
class CSliceStruct;


typedef struct IntraPredStruct
{
	UINT8 block_mode; // block mode: 0-4x4 mode 1-8x8 mode
	bool prev_intra_pred_mode_flag;
	UINT8 rem_intra_pred_mode;

	IntraPredStruct()
	{
		block_mode = 0;
		prev_intra_pred_mode_flag = false;
		rem_intra_pred_mode = 0;
	}
}IntraPredStruct;

// ºê¿éÀà
class CMacroBlock
{
public:
	CMacroBlock(UINT8 *pSODB, UINT32 offset, UINT32 idx);
	~CMacroBlock();
	
	void Set_pic_param_set(CPicParamSet* pps);
	UINT32 Parse_macroblock();

	void Dump_macroblock_info();

private:
	UINT8  *pSODB;
	UINT32  byteOffset;
	UINT8  bitOffset;
	UINT32 mbDataSize;

	CPicParamSet* pps_active;

	UINT32 mb_idx;
	UINT8 mb_type;
	bool transform_size_8x8_flag;

	IntraPredStruct* pred_struct;
	UINT8 intra_chroma_pred_mode;

	UINT8 coded_block_pattern;
	UINT8 mb_qp_delta;

	UINT8 cbp_luma;
	UINT8 cbp_chroma;
	};

