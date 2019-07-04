#include "pch.h"
#include "SeqParamSet.h"


CSeqParamSet::CSeqParamSet()
{
	memset(this, 0, sizeof(CSeqParamSet));
	chroma_format_idc = 1;
}


CSeqParamSet::~CSeqParamSet()
{
}


void CSeqParamSet::Dump_sps_info()
{
#if TRACE_CONFIG_SEQ_PARAM_SET

#if TRACE_CONFIG_LOGOUT
	g_traceFile << "==========Sequence Paramater Set==========" << std::endl;
	g_traceFile << "Profile: " << std::to_string(profile_idc) << std::endl;
	g_traceFile << "Level: " << std::to_string(level_idc) << std::endl;
	g_traceFile << "SPS ID: " << std::to_string(sps_id) << std::endl;
	if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 244 || profile_idc == 44 ||
		profile_idc == 83 || profile_idc == 86 || profile_idc == 118 || profile_idc == 128)
	{
		g_traceFile << "chroma_format_idc: " << std::to_string(chroma_format_idc) << std::endl;
		if (chroma_format_idc == 3)
		{
			g_traceFile << "separate_colour_plane_flag: " << std::to_string(separate_colour_plane_flag) << std::endl;
		}
		g_traceFile << "bit_depth_luma: " << std::to_string(bit_depth_luma) << std::endl;
		g_traceFile << "bit_depth_chroma: " << std::to_string(bit_depth_chroma) << std::endl;
		g_traceFile << "qpprime_y_zero_transform_bypass_flag: " << std::to_string(qpprime_y_zero_transform_bypass_flag) << std::endl;
		g_traceFile << "seq_scaling_matrix_present_flag: " << std::to_string(seq_scaling_matrix_present_flag) << std::endl;
	}
	g_traceFile << "log2_max_frame_num: " << std::to_string(log2_max_frame_num) << std::endl;
	g_traceFile << "pic_order_cnt_type: " << std::to_string(poc_type) << std::endl;
	if (poc_type == 0)
	{
		g_traceFile << "log2_max_poc_cnt: " << std::to_string(log2_max_poc_cnt) << std::endl;
	}
	g_traceFile << "log2_max_num_ref_frames: " << std::to_string(log2_max_num_ref_frames) << std::endl;
	g_traceFile << "gaps_in_frame_num_value_allowed_flag: " << std::to_string(gaps_in_frame_num_value_allowed_flag) << std::endl;
	g_traceFile << "pic_width_in_mbs_minus1: " << std::to_string(pic_width_in_mbs) << std::endl;
	g_traceFile << "pic_height_in_map_units_minus1: " << std::to_string(pic_height_in_map_units) << std::endl;
	g_traceFile << "(picture resolution: " << std::to_string(pic_width_in_mbs * 16) << " x " << std::to_string(pic_height_in_mbs * 16) << ")" << std::endl;
	g_traceFile << "frame_mbs_only_flag: " << std::to_string(frame_mbs_only_flag) << std::endl;
	if (!frame_mbs_only_flag)
	{
		g_traceFile << "mb_adaptive_frame_field_flag: " << std::to_string(mb_adaptive_frame_field_flag) << std::endl;
	}
	g_traceFile << "direct_8x8_inference_flag: " << std::to_string(direct_8x8_inference_flag) << std::endl;
	g_traceFile << "frame_cropping_flag: " << std::to_string(frame_cropping_flag) << std::endl;
	if (frame_cropping_flag)
	{
		g_traceFile << "frame_crop_left_offset: " << std::to_string(frame_crop_offset[0]) << std::endl;
		g_traceFile << "frame_crop_right_offset: " << std::to_string(frame_crop_offset[1]) << std::endl;
		g_traceFile << "frame_crop_top_offset: " << std::to_string(frame_crop_offset[2]) << std::endl;
		g_traceFile << "frame_crop_bottum_offset: " << std::to_string(frame_crop_offset[3]) << std::endl;
	}
	g_traceFile << "vui_parameters_present_flag: " << std::to_string(vui_parameters_present_flag) << std::endl;
	g_traceFile << "==========================================" << std::endl;
	g_traceFile.flush();
#endif

#endif
}

void CSeqParamSet::Set_profile_level_idc(UINT8 profile, UINT8 level)
{
	profile_idc = profile;
	level_idc = level;
}

void CSeqParamSet::Set_sps_id(UINT8 sps_id)
{
	sps_id = sps_id;
}

void CSeqParamSet::Set_chroma_format_idc(UINT8 chromaFormatIdc)
{
	chroma_format_idc = chromaFormatIdc;
}

void CSeqParamSet::Set_bit_depth(UINT8 bit_depth_luma, UINT8 bit_depth_chroma)
{
	bit_depth_luma = bit_depth_luma;
	bit_depth_chroma = bit_depth_chroma;
}

void CSeqParamSet::Set_log2_max_frame_num(UINT32 log2maxFrameNum)
{
	log2_max_frame_num = log2maxFrameNum;
}

void CSeqParamSet::Set_poc_type(UINT8 pocType)
{
	poc_type = pocType;
}

void CSeqParamSet::Set_log2_max_poc_cnt(UINT32 log2maxPocCnt)
{
	log2_max_poc_cnt = log2maxPocCnt;
}

void CSeqParamSet::Set_max_num_ref_frames(UINT32 maxRefFrames)
{
	log2_max_num_ref_frames = maxRefFrames;
}

void CSeqParamSet::Set_sps_multiple_flags(UINT32 flags)
{
	separate_colour_plane_flag = flags & (1 << 21);
	qpprime_y_zero_transform_bypass_flag = flags & (1 << 20);
	seq_scaling_matrix_present_flag = flags & (1 << 19);

	gaps_in_frame_num_value_allowed_flag = flags & (1 << 5);
	frame_mbs_only_flag = flags & (1 << 4);
	mb_adaptive_frame_field_flag = flags & (1 << 3);
	direct_8x8_inference_flag = flags & (1 << 2);
	frame_cropping_flag = flags & (1 << 1);
	vui_parameters_present_flag = flags & 1;
}

void CSeqParamSet::Set_pic_reslution_in_mbs(UINT16 widthInMBs, UINT16 heightInMapUnits)
{
	pic_width_in_mbs = widthInMBs;
	pic_height_in_map_units = heightInMapUnits;
	pic_height_in_mbs = frame_mbs_only_flag ? pic_height_in_map_units : 2 * pic_height_in_map_units;
}

void CSeqParamSet::Set_frame_crop_offset(UINT32 offsets[4])
{
	for (int idx = 0; idx < 4; idx++)
	{
		frame_crop_offset[idx] = offsets[idx];
	}
}

bool CSeqParamSet::Get_separate_colour_plane_flag()
{
	return separate_colour_plane_flag;
}

UINT32 CSeqParamSet::Get_log2_max_frame_num()
{
	return log2_max_frame_num;
}

UINT32 CSeqParamSet::Get_log2_max_poc_cnt()
{
	return log2_max_poc_cnt;
}

bool CSeqParamSet::Get_frame_mbs_only_flag()
{
	return frame_mbs_only_flag;
}

UINT8 CSeqParamSet::Get_poc_type()
{
	return poc_type;
}

UINT16 CSeqParamSet::Get_pic_width_in_mbs()
{
	return pic_width_in_mbs;
}

UINT16 CSeqParamSet::Get_pic_height_in_mbs()
{
	return pic_height_in_mbs;
}