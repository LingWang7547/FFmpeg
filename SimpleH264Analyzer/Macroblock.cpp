#include "pch.h"
#include "MacroBlock.h"
#include "PicParamSet.h"
#include "Residual.h"
#include "SliceStruct.h"
#include "SliceHeader.h"
#include "MacroBlock_Defines.h"

using namespace std;

CMacroBlock::CMacroBlock(UINT8* pSODB, UINT32 offset, UINT32 idx)
{
	this->pSODB = pSODB;
	this->byteOffset = offset / 8;
	this->bitOffset = offset % 8;
	this->pps_active = NULL;
	this->mb_idx = idx;

	mbDataSize = offset;

	mb_type = 0;
	transform_size_8x8_flag = false;

	pred_struct = NULL;
	intra_chroma_pred_mode = 0;

	coded_block_pattern = 0;
	mb_qp_delta = 0;

	cbp_luma = 0;
	cbp_chroma = 0;

	residual = NULL;
}

CMacroBlock::~CMacroBlock()
{
	if (pred_struct)
	{
		delete pred_struct;
		pred_struct = NULL;
	}
	if (!residual)
	{
		delete residual;
		residual = NULL;
	}
}

void CMacroBlock::Set_pic_param_set(CPicParamSet * pps)
{
	pps_active = pps;
}

void CMacroBlock::Set_slice_struct(CSliceStruct * slicestruct)
{
	this->slice = slicestruct;
}

UINT32 CMacroBlock::Parse_macroblock()
{
	mb_type = Get_uev_code_num(pSODB, byteOffset, bitOffset);

	if (mb_type == 25)
	{
		// To do: I-PCM mode...
	}
	else if (mb_type == 0)
	{
		// Intra_NxN mode...
		if (pps_active->Get_transform_8x8_mode_flag())
		{
			transform_size_8x8_flag = Get_bit_at_position(pSODB, byteOffset, bitOffset);
		}

		// Get prediction-block num...
		if (transform_size_8x8_flag)
		{
			// Using intra_8x8
			pred_struct = new IntraPredStruct[4];
			for (int luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx++)
			{
				pred_struct[luma8x8BlkIdx].block_mode = 0;
				pred_struct[luma8x8BlkIdx].prev_intra_pred_mode_flag = Get_bit_at_position(pSODB, byteOffset, bitOffset);
				if (!pred_struct[luma8x8BlkIdx].prev_intra_pred_mode_flag)
				{
					pred_struct[luma8x8BlkIdx].rem_intra_pred_mode = Get_uint_code_num(pSODB, byteOffset, bitOffset, 3);
				}
			}
		}
		else
		{
			// Using intra_4x4
			pred_struct = new IntraPredStruct[16];
			for (int luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++)
			{
				pred_struct[luma4x4BlkIdx].block_mode = 0;
				pred_struct[luma4x4BlkIdx].prev_intra_pred_mode_flag = Get_bit_at_position(pSODB, byteOffset, bitOffset);
				if (!pred_struct[luma4x4BlkIdx].prev_intra_pred_mode_flag)
				{
					pred_struct[luma4x4BlkIdx].rem_intra_pred_mode = Get_uint_code_num(pSODB, byteOffset, bitOffset, 3);
				}
			}
		}
		// intra_chroma_pred_mode
		intra_chroma_pred_mode = Get_uev_code_num(pSODB, byteOffset, bitOffset);
	}
	else
	{
		// To do: Intra_16x16 mode
	}

	if (mb_type == 0 || mb_type == 25)
	{
		coded_block_pattern = Get_me_code_num(pSODB, byteOffset, bitOffset, 1);
		cbp_luma = coded_block_pattern % 16;
		cbp_chroma = coded_block_pattern / 16;
		
	}
	if (cbp_luma > 0 || cbp_chroma > 0 || (mb_type > 0 && mb_type < 25))
	{
		interpret_mb_mode();
		mb_qp_delta = Get_sev_code_num(pSODB, byteOffset, bitOffset);
		residual = new CResidual(pSODB, byteOffset * 8 + bitOffset, this);
		residual->Parse_macroblock_residual();
	}
	// 输出macroblock 基本信息
	Dump_macroblock_info();
	mbDataSize = byteOffset * 8 + bitOffset - mbDataSize;
	return mbDataSize;
}

CPicParamSet * CMacroBlock::get_pps_active()
{
	return pps_active;
}

int CMacroBlock::Get_number_current(int block_idx_x, int block_idx_y)
{
	int nc = 0, topIdx = 0, leftIdx = 0, leftNum = 0;
	bool available_top = false, available_left = false;


	return nc;
}

void CMacroBlock::Dump_macroblock_info()
{
#if TRACE_CONFIG_LOGOUT
#if TRACE_CONFIG_MACROBLOCK

		g_traceFile << "***** MacroBlock: " << to_string(mb_idx) << " *****" << endl;
		g_traceFile << "mb_type: " << to_string(mb_type) << endl;
		if (mb_type == 0)
		{
			if (!transform_size_8x8_flag)
			{
				for (int idx = 0; idx < 16; idx++)
				{
					g_traceFile << "prev_intra4x4_pred_mode_flag: " << pred_struct[idx].prev_intra_pred_mode_flag << endl;
					if (!pred_struct[idx].prev_intra_pred_mode_flag)
					{
						g_traceFile << "rem_intra4x4_pred_mode: " << to_string(pred_struct[idx].rem_intra_pred_mode) << endl;
					}
				}
			}
		}

		g_traceFile << "intra_chroma_pred_mode: " << to_string(intra_chroma_pred_mode) << endl;
		if (mb_type == 0 || mb_type == 25)
		{
			g_traceFile << "coded_block_pattern: " << to_string(coded_block_pattern) << endl;
		}
		g_traceFile << "mb_qp_delta: " << to_string(mb_qp_delta) << endl;
#endif

#endif
}

void CMacroBlock::interpret_mb_mode()
{
	UINT8 slice_type = slice->sliceHeader->Get_slice_type();
	switch (slice_type)
	{
	case SLICE_TYPE_I:
		if (mb_type == 0)
		{
			mb_type = I4MB;
		}
		else if (mb_type == 25)
		{
			mb_type = IPCM;
		}
		else
		{
			mb_type = I16MB;
		}
	default:
		break;
	}
}

int CMacroBlock::get_neighbor_available(bool & available_top, bool & available_left, int & topIdx, int & leftIdx, int block_idc_x, int block_idc_y)
{
	int _mb_idx = mb_idx;

	return 0;
}
