#pragma once

class CPicParamSet
{
public:
	CPicParamSet();
	~CPicParamSet();

	void Set_pps_id(UINT8 ppsID);
	void Set_sps_id(UINT8 spsID);
	void Set_num_slice_groups(UINT8 num_slice_grops);
	void Set_num_ref_idx(UINT8 l0, UINT8 l1);
	void Set_weighted_bipred_idc(UINT8 weighted_bipred_idc);
	void Set_pic_init_qp(int pic_init_qp);
	void Set_pic_init_qs(int pic_init_qs);
	void Set_chroma_qp_index_offset(int chroma_qp_index_offset);
	void Set_multiple_flags(UINT16 flags);

	bool Get_bottom_field_pic_order_in_frame_present_flag();
	bool Get_transform_8x8_mode_flag();
	bool Get_entropy_coding_flag();
	int  Get_pic_init_qp();
	bool Get_deblocking_filter_control_present_flag();

	void Dump_pps_info();

private:
	UINT8  pps_id;
	UINT8  sps_id;
	bool   entropy_coding_flag;
	bool   bottom_field_pic_order_in_frame_present_flag;
	UINT8  num_slice_groups;
	UINT8  num_ref_idx_l0_default_active;
	UINT8  num_ref_idx_l1_default_active;
	bool   weighted_pred_flag;
	UINT8  weighted_bipred_idc;
	int    pic_init_qp;
	int    pic_init_qs;
	int    chroma_qp_index_offset;
	bool   deblocking_filter_control_present_flag;
	bool   constrained_intra_pred_flag;
	bool   redundant_pic_cnt_present_flag;
	bool   transform_8x8_mode_flag;
};

