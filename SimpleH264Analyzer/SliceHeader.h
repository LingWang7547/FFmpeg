#pragma once
class CSeqParamSet;
class CPicParamSet;

typedef enum
{
	SLICE_TYPE_P = 0,
	SLICE_TYPE_B,
	SLICE_TYPE_I,
	SLICE_TYPE_SP,
	SLICE_TYPE_SI
} SliceType;

typedef struct DecRefPicMarking
{
	bool no_output_of_prior_pics_flag;
	bool long_term_reference_flag;
} DecRefPicMarking;

class CSliceHeader
{
public:
	CSliceHeader(UINT8	*pSODB, CSeqParamSet *sps, CPicParamSet *pps, UINT8	nalType);
	~CSliceHeader();

	UINT32 Parse_slice_header();
	void  Dump_slice_header_info();

	UINT8 Get_slice_type();
	int  Get_slice_qp_delta();

	int	   disable_deblocking_filter_idc;
	int    slice_alpha_c0_offset;
	int    slice_beta_offset;
private:
	CSeqParamSet *sps_active;
	CPicParamSet *pps_active;
	UINT8	*pSODB;
	UINT8   nalType;

	UINT16 first_mb_in_slice;
	UINT8  slice_type;
	UINT8  pps_id;
	UINT8  colour_plane_id;
	UINT32 frame_num;
	bool   field_pic_flag;
	bool   bottom_field_flag;
	UINT16 idr_pic_id;
	UINT32 poc;
	int	   delta_poc_bottom;
	DecRefPicMarking dec_ref_pic_marking;
	int	   slice_qp_delta;
};
