#ifndef MLISA_LIBDEVICE_H
#define MLISA_LIBDEVICE_H

#include <spirv/spirv.h>

#ifdef cl_khr_fp64
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#endif

#ifdef cl_khr_fp16
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#endif


float __cn_scalar_add_f32_rn(float,float);
float __cn_scalar_sub_f32_rn(float,float);
float __cn_scalar_mul_f32_rn(float,float);
float __cn_scalar_div_f32_rn(float,float);
float __cn_scalar_sin_f32(float);
float __cn_scalar_cos_f32(float);
float __cn_scalar_tan_f32(float);
float __cn_scalar_sinh_f32(float);
float __cn_scalar_cosh_f32(float);
float __cn_scalar_tanh_f32(float);
float __cn_scalar_asin_f32(float);
float __cn_scalar_acos_f32(float);
float __cn_scalar_atan_f32(float);

float __cn_scalar_ceil_f32(float);
float __cn_scalar_round_f32(float);
float __cn_scalar_trunc_f32(float);
float __cn_scalar_abs_f32(float);
float __cn_scalar_sign_f32(float);
float __cn_scalar_floor_f32(float);
float __cn_scalar_log_f32(float);
float __cn_scalar_log1p_f32(float);
float __cn_scalar_log2_f32(float);
float __cn_scalar_log10_f32(float);
float __cn_scalar_exp_f32(float);
float __cn_scalar_erf_f32(float);
float __cn_scalar_sqrt_f32(float);
float __cn_scalar_rsqrt_f32(float);

float __cn_scalar_mod_f32(float,float);
float __cn_scalar_pow_f32(float,float);
float __cn_scalar_max_f32(float,float);
float __cn_scalar_min_f32(float,float);

int __cn_scalar_isinf_f32(float);
int __cn_scalar_isnan_f32(float);

#endif // MLISA_LIBDEVICE_H
