#ifndef MLISA_LIBDEVICE_H
#define MLISA_LIBDEVICE_H

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

void __cn_vector_abs_f32(int n, float *y, const float *x);
void __cn_vector_add_f32_rn(int n, float *z, const float *x, const float *y);
void __cn_vector_add_scalar_f32_rn(int n, float *z, const float *x, const float y);
void __cn_vector_acos_f32(int n, float *y, const float *x);
void __cn_vector_asin_f32(int n, float *y, const float *x);
void __cn_vector_atan_f32(int n, float *y, const float *x);
void __cn_vector_ceil_f32(int n, float *y, const float *x);
void __cn_vector_cos_f32(int n, float *y, const float *x);
void __cn_vector_cosh_f32(int n, float *y, const float *x);
void __cn_scalar_div_vector_f32_rn(int n, float *z, const float x, const float *y);
void __cn_vector_div_f32_rn(int n, float *z, const float *x, const float *y);
void __cn_vector_div_scalar_f32_rn(int n, float *z, const float *x, const float y);
void __cn_vector_erf_f32(int n, float *y, const float *x);
void __cn_vector_exp_f32(int n, float *y, const float *x);
void __cn_vector_floor_f32(int n, float *y, const float *x);
void __cn_vector_log_f32(int n, float *y, const float *x);
void __cn_vector_max_f32(int n, float *z, const float *x, const float *y);
void __cn_vector_min_f32(int n, float *z, const float *x, const float *y);
void __cn_vector_mod_f32(int n, float *z, const float *x, const float *y);
void __cn_vector_mul_f32_rn(int n, float *z, const float *x, const float *y);
void __cn_vector_mul_scalar_f32_rn(int n, float *z, const float *x, const float y);
void __cn_vector_negate_f32(int n, float *y, const float *x);
void __cn_vector_negate_s32(int n, int *y, const int *x);
void __cn_vector_pow_f32(int n, float *z, const float *x, const float *y);
void __cn_vector_rcp_f32_rn(int n, float *y, const float *x);
void __cn_vector_round_f32(int n, float *y, const float *x);
void __cn_vector_rsqrt_f32(int n, float *y, const float *x);
void __cn_vector_sign_f32(int n, float *y, const float *x);
void __cn_vector_sin_f32(int n, float *y, const float *x);
void __cn_vector_sinh_f32(int n, float *y, const float *x);
void __cn_vector_sqrt_f32(int n, float *y, const float *x);
void __cn_scalar_sub_vector_f32_rn(int n, float *z, const float x, const float *y);
void __cn_vector_sub_f32_rn(int n, float *z, const float *x, const float *y);
void __cn_vector_sub_scalar_f32_rn(int n, float *z, const float *x, const float y);
void __cn_vector_tan_f32(int n, float *y, const float *x);
void __cn_vector_tanh_f32(int n, float *y, const float *x);
void __cn_vector_trunc_f32(int n, float *y, const float *x);

void __cn_vector_and_u32(int n, unsigned int *z, const unsigned int *x, const unsigned int *y);
void __cn_vector_not_u32(int n, unsigned int *y, const unsigned int *x);
void __cn_vector_or_u32(int n, unsigned int *z, const unsigned int *x, const unsigned int *y);
void __cn_vector_shift_left_u32(int n, unsigned int *z, const unsigned int *x, const unsigned int *y);
void __cn_vector_shift_right_arithmetic_scalar_s32(int n, int *z, const int *x, const int y);
void __cn_vector_shift_right_arithmetic_u32(int n, unsigned int *z, const unsigned int *x, const unsigned int *y);
void __cn_vector_shift_right_logical_u32(int n, unsigned int *z, const unsigned int *x, const unsigned int *y);
void __cn_vector_xor_u32(int n, unsigned int *z, const unsigned int *x, const unsigned int *y);

#endif // MLISA_LIBDEVICE_H
