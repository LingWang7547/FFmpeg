#include "pch.h"
#include "SliceHeader.h"
#include "SeqParamSet.h"
#include "PicParamSet.h"

using namespace std;

CSliceHeader::CSliceHeader(UINT8 *pSODB, CSeqParamSet *sps, CPicParamSet *pps, UINT8 nalType)
{
	this->pSODB = pSODB;
	sps_active = sps;
	pps_active = pps;
	this->nalType = nalType;

	first_mb_in_slice = 0;
	slice_type = 0;
	pps_id = 0;
	colour_plane_id = 0;
	frame_num = 0;
	field_pic_flag = 0;
	bottom_field_flag = 0;
	idr_pic_id = 0;
	poc = 0;
	delta_poc_bottom = 0;
	dec_ref_pic_marking = { 0, 0 };
	slice_qp_delta = 0;
	disable_deblocking_filter_idc = 0;
	slice_alpha_c0_offset = 0;
	slice_beta_offset = 0;
}

CSliceHeader::~CSliceHeader()
{
}

UINT32 CSliceHeader::Parse_slice_header()
{
	UINT32 sliceHeaderLengthInBits = 0;
	UINT8  bitPosition = 0;
	UINT32 bytePosition = 0;

	first_mb_in_slice = Get_uev_code_num(pSODB, bytePosition, bitPosition);
	slice_type = Get_uev_code_num(pSODB, bytePosition, bitPosition);
	//slice_type %= 5;
	pps_id = Get_uev_code_num(pSODB, bytePosition, bitPosition);

	if (sps_active->Get_separate_colour_plane_flag())
	{
		colour_plane_id = Get_uint_code_num(pSODB, bytePosition, bitPosition, 2);
	}

	frame_num = Get_uint_code_num(pSODB, bytePosition, bitPosition, sps_active->Get_log2_max_frame_num());

	if (!sps_active->Get_frame_mbs_only_flag())
	{
		field_pic_flag = Get_bit_at_position(pSODB, bytePosition, bitPosition);
		if (field_pic_flag)
		{
			bottom_field_flag = Get_bit_at_position(pSODB, bytePosition, bitPosition);
		}
	}

	if (nalType == 5)
	{
		idr_pic_id = Get_uev_code_num(pSODB, bytePosition, bitPosition);
	}

	if (sps_active->Get_poc_type() == 0)
	{
		poc = Get_uint_code_num(pSODB, bytePosition, bitPosition, sps_active->Get_log2_max_poc_cnt());
		if ((!field_pic_flag) && pps_active->Get_bottom_field_pic_order_in_frame_present_flag())
		{
			delta_poc_bottom = Get_sev_code_num(pSODB, bytePosition, bitPosition);
		}
	}

	if (nalType == 5)
	{
		dec_ref_pic_marking.no_output_of_prior_pics_flag = Get_bit_at_position(pSODB, bytePosition, bitPosition);
		dec_ref_pic_marking.long_term_reference_flag = Get_bit_at_position(pSODB, bytePosition, bitPosition);
	}

	slice_qp_delta = Get_sev_code_num(pSODB, bytePosition, bitPosition);

	if (pps_active->Get_deblocking_filter_control_present_flag())
	{
		disable_deblocking_filter_idc = Get_uev_code_num(pSODB, bytePosition, bitPosition);
		if (disable_deblocking_filter_idc != 1)
		{
			slice_alpha_c0_offset = 2 * Get_sev_code_num(pSODB, bytePosition, bitPosition);
			slice_beta_offset = 2 * Get_sev_code_num(pSODB, bytePosition, bitPosition);
		}
	}

	sliceHeaderLengthInBits = 8 * bytePosition + bitPosition;

	return sliceHeaderLengthInBits;
}

void CSliceHeader::Dump_slice_header_info()
{
#if TRACE_CONFIG_SLICE_HEADER

#if TRACE_CONFIG_LOGOUT
	g_traceFile << "--------------- Slice Header ----------------" << endl;
	g_traceFile << "First MB In Slice: " << to_string(first_mb_in_slice) << endl;
	g_traceFile << "Slice Type: " << to_string(slice_type) << endl;
	g_traceFile << "Picture Parameter Set ID: " << to_string(pps_id) << endl;
	if (sps_active->Get_separate_colour_plane_flag())
	{
		g_traceFile << "Color Plane ID: " << to_string(colour_plane_id) << endl;
	}
	g_traceFile << "Frame Num: " << to_string(frame_num) << endl;
	if (!sps_active->Get_frame_mbs_only_flag())
	{
		g_traceFile << "field_pic_flag: " << to_string(field_pic_flag) << endl;
		if (field_pic_flag)
		{
			g_traceFile << "bottom_field_flag: " << to_string(bottom_field_flag) << endl;
		}
	}

	if (nalType == 5)
	{
		g_traceFile << "IDR Picture Flag: " << to_string(idr_pic_id) << endl;
	}

	if (sps_active->Get_poc_type() == 0)
	{
		g_traceFile << "Picture Order Count: " << to_string(poc) << endl;
		if ((!field_pic_flag) && pps_active->Get_bottom_field_pic_order_in_frame_present_flag())
		{
			g_traceFile << "delta_pic_order_cnt_bottom: " << to_string(delta_poc_bottom) << endl;
		}
	}

	if (nalType == 5)
	{
		g_traceFile << "output_of_prior_pics_flag: " << to_string(dec_ref_pic_marking.no_output_of_prior_pics_flag) << endl;
		g_traceFile << "long_term_reference_flag: " << to_string(dec_ref_pic_marking.long_term_reference_flag) << endl;
	}

	g_traceFile << "Slice QP Delta: " << to_string(slice_qp_delta) << endl;
	g_traceFile << "-------------------------------------------" << endl;
#endif

#endif
}

UINT8 CSliceHeader::Get_slice_type()
{
	return slice_type;
}

int CSliceHeader::Get_slice_qp_delta()
{
	return slice_qp_delta;
}