/*************************************************************************
    > File Name: h264.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月24日 星期二 15时37分22秒
 ************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2015
 */

#ifndef BV_H264_H
#define BV_H264_H

#ifdef __cplusplus
extern "C"{
#endif

#define H264_MAX_PICTURE_COUNT 36
#define H264_MAX_THREADS       32

#define MAX_SPS_COUNT          32
#define MAX_PPS_COUNT         256

#define MAX_MMCO_COUNT         66

#define MAX_DELAYED_PIC_COUNT  16

#define MAX_MBPAIR_SIZE (256*1024) // a tighter bound could be calculated if someone cares about a few bytes

/* Compiling in interlaced support reduces the speed
 * of progressive decoding by about 2%. */
#define ALLOW_INTERLACE

#define FMO 0

/**
 * The maximum number of slices supported by the decoder.
 * must be a power of 2
 */
#define MAX_SLICES 32

#ifdef ALLOW_INTERLACE
#define MB_MBAFF(h)    (h)->mb_mbaff
#define MB_FIELD(h)    (h)->mb_field_decoding_flag
#define FRAME_MBAFF(h) (h)->mb_aff_frame
#define FIELD_PICTURE(h) ((h)->picture_structure != PICT_FRAME)
#define LEFT_MBS 2
#define LTOP     0
#define LBOT     1
#define LEFT(i)  (i)
#else
#define MB_MBAFF(h)      0
#define MB_FIELD(h)      0
#define FRAME_MBAFF(h)   0
#define FIELD_PICTURE(h) 0
#undef  IS_INTERLACED
#define IS_INTERLACED(mb_type) 0
#define LEFT_MBS 1
#define LTOP     0
#define LBOT     0
#define LEFT(i)  0
#endif
#define FIELD_OR_MBAFF_PICTURE(h) (FRAME_MBAFF(h) || FIELD_PICTURE(h))

#ifndef CABAC
#define CABAC(h) (h)->pps.cabac
#endif

#define CHROMA(h)    ((h)->sps.chroma_format_idc)
#define CHROMA422(h) ((h)->sps.chroma_format_idc == 2)
#define CHROMA444(h) ((h)->sps.chroma_format_idc == 3)

#define EXTENDED_SAR       255

#define MB_TYPE_REF0       MB_TYPE_ACPRED // dirty but it fits in 16 bit
#define MB_TYPE_8x8DCT     0x01000000
#define IS_REF0(a)         ((a) & MB_TYPE_REF0)
#define IS_8x8DCT(a)       ((a) & MB_TYPE_8x8DCT)

#define QP_MAX_NUM (51 + 6*6)           // The maximum supported qp

/* NAL unit types */
enum {
    NAL_SLICE           = 1,
    NAL_DPA             = 2,
    NAL_DPB             = 3,
    NAL_DPC             = 4,
    NAL_IDR_SLICE       = 5,
    NAL_SEI             = 6,
    NAL_SPS             = 7,
    NAL_PPS             = 8,
    NAL_AUD             = 9,
    NAL_END_SEQUENCE    = 10,
    NAL_END_STREAM      = 11,
    NAL_FILLER_DATA     = 12,
    NAL_SPS_EXT         = 13,
    NAL_AUXILIARY_SLICE = 19,
    NAL_FF_IGNORE       = 0xff0f001,
};

/**
 * SEI message types
 */
typedef enum {
    SEI_TYPE_BUFFERING_PERIOD       = 0,   ///< buffering period (H.264, D.1.1)
    SEI_TYPE_PIC_TIMING             = 1,   ///< picture timing
    SEI_TYPE_USER_DATA_ITU_T_T35    = 4,   ///< user data registered by ITU-T Recommendation T.35
    SEI_TYPE_USER_DATA_UNREGISTERED = 5,   ///< unregistered user data
    SEI_TYPE_RECOVERY_POINT         = 6,   ///< recovery point (frame # to decoder sync)
    SEI_TYPE_FRAME_PACKING          = 45,  ///< frame packing arrangement
    SEI_TYPE_DISPLAY_ORIENTATION    = 47,  ///< display orientation
} SEI_Type;

/**
 * pic_struct in picture timing SEI message
 */
typedef enum {
    SEI_PIC_STRUCT_FRAME             = 0, ///<  0: %frame
    SEI_PIC_STRUCT_TOP_FIELD         = 1, ///<  1: top field
    SEI_PIC_STRUCT_BOTTOM_FIELD      = 2, ///<  2: bottom field
    SEI_PIC_STRUCT_TOP_BOTTOM        = 3, ///<  3: top field, bottom field, in that order
    SEI_PIC_STRUCT_BOTTOM_TOP        = 4, ///<  4: bottom field, top field, in that order
    SEI_PIC_STRUCT_TOP_BOTTOM_TOP    = 5, ///<  5: top field, bottom field, top field repeated, in that order
    SEI_PIC_STRUCT_BOTTOM_TOP_BOTTOM = 6, ///<  6: bottom field, top field, bottom field repeated, in that order
    SEI_PIC_STRUCT_FRAME_DOUBLING    = 7, ///<  7: %frame doubling
    SEI_PIC_STRUCT_FRAME_TRIPLING    = 8  ///<  8: %frame tripling
} SEI_PicStructType;

/**
 * frame_packing_arrangement types
 */
typedef enum {
    SEI_FPA_TYPE_CHECKERBOARD        = 0,
    SEI_FPA_TYPE_INTERLEAVE_COLUMN   = 1,
    SEI_FPA_TYPE_INTERLEAVE_ROW      = 2,
    SEI_FPA_TYPE_SIDE_BY_SIDE        = 3,
    SEI_FPA_TYPE_TOP_BOTTOM          = 4,
    SEI_FPA_TYPE_INTERLEAVE_TEMPORAL = 5,
    SEI_FPA_TYPE_2D                  = 6,
} SEI_FpaType;

/**
 * Sequence parameter set
 */
typedef struct SPS {
    unsigned int sps_id;
    int profile_idc;
    int level_idc;
    int chroma_format_idc;
    int transform_bypass;              ///< qpprime_y_zero_transform_bypass_flag
    int log2_max_frame_num;            ///< log2_max_frame_num_minus4 + 4
    int poc_type;                      ///< pic_order_cnt_type
    int log2_max_poc_lsb;              ///< log2_max_pic_order_cnt_lsb_minus4
    int delta_pic_order_always_zero_flag;
    int offset_for_non_ref_pic;
    int offset_for_top_to_bottom_field;
    int poc_cycle_length;              ///< num_ref_frames_in_pic_order_cnt_cycle
    int ref_frame_count;               ///< num_ref_frames
    int gaps_in_frame_num_allowed_flag;
    int mb_width;                      ///< pic_width_in_mbs_minus1 + 1
    int mb_height;                     ///< pic_height_in_map_units_minus1 + 1
    int frame_mbs_only_flag;
    int mb_aff;                        ///< mb_adaptive_frame_field_flag
    int direct_8x8_inference_flag;
    int crop;                          ///< frame_cropping_flag

    /* those 4 are already in luma samples */
    unsigned int crop_left;            ///< frame_cropping_rect_left_offset
    unsigned int crop_right;           ///< frame_cropping_rect_right_offset
    unsigned int crop_top;             ///< frame_cropping_rect_top_offset
    unsigned int crop_bottom;          ///< frame_cropping_rect_bottom_offset
    int vui_parameters_present_flag;
    BVRational sar;
    int video_signal_type_present_flag;
    int full_range;
    int colour_description_present_flag;
    enum BVColorPrimaries color_primaries;
    enum BVColorTransferCharacteristic color_trc;
    enum BVColorSpace colorspace;
    int timing_info_present_flag;
    uint32_t num_units_in_tick;
    uint32_t time_scale;
    int fixed_frame_rate_flag;
    short offset_for_ref_frame[256]; // FIXME dyn aloc?
    int bitstream_restriction_flag;
    int num_reorder_frames;
    int scaling_matrix_present;
    uint8_t scaling_matrix4[6][16];
    uint8_t scaling_matrix8[6][64];
    int nal_hrd_parameters_present_flag;
    int vcl_hrd_parameters_present_flag;
    int pic_struct_present_flag;
    int time_offset_length;
    int cpb_cnt;                          ///< See H.264 E.1.2
    int initial_cpb_removal_delay_length; ///< initial_cpb_removal_delay_length_minus1 + 1
    int cpb_removal_delay_length;         ///< cpb_removal_delay_length_minus1 + 1
    int dpb_output_delay_length;          ///< dpb_output_delay_length_minus1 + 1
    int bit_depth_luma;                   ///< bit_depth_luma_minus8 + 8
    int bit_depth_chroma;                 ///< bit_depth_chroma_minus8 + 8
    int residual_color_transform_flag;    ///< residual_colour_transform_flag
    int constraint_set_flags;             ///< constraint_set[0-3]_flag
    int new;                              ///< flag to keep track if the decoder context needs re-init due to changed SPS
} SPS;

/**
 * Picture parameter set
 */
typedef struct PPS {
    unsigned int sps_id;
    int cabac;                  ///< entropy_coding_mode_flag
    int pic_order_present;      ///< pic_order_present_flag
    int slice_group_count;      ///< num_slice_groups_minus1 + 1
    int mb_slice_group_map_type;
    unsigned int ref_count[2];  ///< num_ref_idx_l0/1_active_minus1 + 1
    int weighted_pred;          ///< weighted_pred_flag
    int weighted_bipred_idc;
    int init_qp;                ///< pic_init_qp_minus26 + 26
    int init_qs;                ///< pic_init_qs_minus26 + 26
    int chroma_qp_index_offset[2];
    int deblocking_filter_parameters_present; ///< deblocking_filter_parameters_present_flag
    int constrained_intra_pred;     ///< constrained_intra_pred_flag
    int redundant_pic_cnt_present;  ///< redundant_pic_cnt_present_flag
    int transform_8x8_mode;         ///< transform_8x8_mode_flag
    uint8_t scaling_matrix4[6][16];
    uint8_t scaling_matrix8[6][64];
    uint8_t chroma_qp_table[2][QP_MAX_NUM+1];  ///< pre-scaled (with chroma_qp_index_offset) version of qp_table
    int chroma_qp_diff;
} PPS;

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_H264_H */
