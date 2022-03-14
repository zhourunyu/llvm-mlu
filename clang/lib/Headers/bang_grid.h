/*===--- bang_grid.h - Device-side MLU grid programming model ---------------===
 *
 * Copyright (C) [2020] by Cambricon, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *===-----------------------------------------------------------------------===
 */
#ifndef __BANG_GRID_H__
#define __BANG_GRID_H__

#ifndef __BANG__
#error "Ths file is for BANG compilation only."
#endif  // __BANG__

#if __BANG_ARCH__ > 306
#include <bang_device_types.h>

// Launch kernel.
#define __grid_launch_kernel(grid_dim, kernel, args...)                                                             \
  __asm__ volatile("grid.begin;\n\t");                                                                              \
  __mlvm_write_mlu_grid_sreg_griddim(grid_dim.x * grid_dim.y * grid_dim.z);                                         \
  __mlvm_write_mlu_grid_sreg_griddimx(grid_dim.x);                                                                  \
  __mlvm_write_mlu_grid_sreg_griddimy(grid_dim.y);                                                                  \
  __mlvm_write_mlu_grid_sreg_griddimz(grid_dim.z);                                                                  \
  for (unsigned blockidz = 0; blockidz < grid_dim.z; ++blockidz) {                                                  \
    __mlvm_write_mlu_grid_sreg_blockidz(blockidz);                                                                  \
    for (unsigned blockidy = 0; blockidy < grid_dim.y; ++blockidy) {                                                \
      __mlvm_write_mlu_grid_sreg_blockidy(blockidy);                                                                \
      for (unsigned blockidx = 0; blockidx < grid_dim.x; ++blockidx) {                                              \
        __mlvm_write_mlu_grid_sreg_blockidx(blockidx);                                                              \
        __mlvm_write_mlu_grid_sreg_blockid(blockidx + blockidy * grid_dim.x + blockidz * grid_dim.x * grid_dim.y);  \
        kernel(args);                                                                                               \
      }                                                                                                             \
    }                                                                                                               \
  }                                                                                                                 \
  __asm__ volatile("grid.end;\n\t");

// -----------------------------------------------------------------------------
// Grid move
// -----------------------------------------------------------------------------
#define GRID_MOVE(DTYPE, STYPE)                                                           \
  static __mlu_func__ __mlu_builtin__ void __grid_move(__grid_##DTYPE &dst,               \
                                                       const __grid_##STYPE &src) {       \
    dst.__elem = __mlvm_grid_move_##DTYPE##_##STYPE(src.__elem);                          \
  }

#define GRID_MOVE_SCALAR(VTYPE, STYPE)                                                    \
  static __mlu_func__ __mlu_builtin__ void __grid_move(__grid_##VTYPE &dst, STYPE src) {  \
    dst.__elem = __mlvm_grid_move_##VTYPE##_scalar(src);                                  \
  }

GRID_MOVE(f32x1, f32x1);
GRID_MOVE(f16x2, f16x2);
GRID_MOVE(s32x1, s32x1);
GRID_MOVE(s16x2, s16x2);
GRID_MOVE(s8x4, s8x4);
GRID_MOVE(u32x1, u32x1);
GRID_MOVE(u16x2, u16x2);
GRID_MOVE(u8x4, u8x4);
GRID_MOVE(f16x1, f16x1);
GRID_MOVE(s16x1, s16x1);
GRID_MOVE(s8x1, s8x1);
GRID_MOVE(u16x1, u16x1);
GRID_MOVE(u8x1, u8x1);

GRID_MOVE_SCALAR(f32x1, float);
GRID_MOVE_SCALAR(f16x2, half);
GRID_MOVE_SCALAR(s32x1, int32_t);
GRID_MOVE_SCALAR(s16x2, int16_t);
GRID_MOVE_SCALAR(s8x4, char);
GRID_MOVE_SCALAR(u32x1, uint32_t);
GRID_MOVE_SCALAR(u16x2, uint16_t);
GRID_MOVE_SCALAR(u8x4, unsigned char);
GRID_MOVE_SCALAR(f16x1, half);
GRID_MOVE_SCALAR(s16x1, int16_t);
GRID_MOVE_SCALAR(s8x1, char);
GRID_MOVE_SCALAR(u16x1, uint16_t);
GRID_MOVE_SCALAR(u8x1, unsigned char);

#undef GRID_MOVE
#undef GRID_MOVE_SCALAR

// -----------------------------------------------------------------------------
// Grid load
// -----------------------------------------------------------------------------
#define GRID_LOAD(VTYPE)                                                      \
  static __mlu_func__ __mlu_builtin__ void __grid_load(__grid_##VTYPE &dst,   \
                                                       void* addr,            \
                                                       unsigned size,         \
                                                       unsigned stride,       \
                                                       unsigned iter) {       \
    dst.__elem = __mlvm_grid_load_##VTYPE(addr, size, stride, iter);          \
  }

GRID_LOAD(f32x1);
GRID_LOAD(f16x2);
GRID_LOAD(s32x1);
GRID_LOAD(s16x2);
GRID_LOAD(s8x4);
GRID_LOAD(u32x1);
GRID_LOAD(u16x2);
GRID_LOAD(u8x4);
GRID_LOAD(f16x1);
GRID_LOAD(s16x1);
GRID_LOAD(s8x1);
GRID_LOAD(u16x1);
GRID_LOAD(u8x1);

#undef GRID_LOAD

// -----------------------------------------------------------------------------
// Grid store
// -----------------------------------------------------------------------------
#define GRID_STORE(VTYPE)                                                           \
  static __mlu_func__ __mlu_builtin__ void __grid_store(void* addr,                 \
                                                        const __grid_##VTYPE &src,  \
                                                        unsigned size,              \
                                                        unsigned stride,            \
                                                        unsigned iter) {            \
    __mlvm_grid_store_##VTYPE(addr, src.__elem, size, stride, iter);                \
  }

GRID_STORE(f32x1);
GRID_STORE(f16x2);
GRID_STORE(s32x1);
GRID_STORE(s16x2);
GRID_STORE(s8x4);
GRID_STORE(u32x1);
GRID_STORE(u16x2);
GRID_STORE(u8x4);
GRID_STORE(f16x1);
GRID_STORE(s16x1);
GRID_STORE(s8x1);
GRID_STORE(u16x1);
GRID_STORE(u8x1);

#undef GRID_STORE

// -----------------------------------------------------------------------------
// Grid binary computation
// -----------------------------------------------------------------------------
#define GRID_BINARY(OP, VTYPE)                                                        \
  static __mlu_func__ __mlu_builtin__ void __grid_##OP(__grid_##VTYPE &dst,           \
                                                       const __grid_##VTYPE &src0,    \
                                                       const __grid_##VTYPE &src1) {  \
    dst.__elem = __mlvm_grid_##OP##_##VTYPE(src0.__elem, src1.__elem);                \
  }

#define GRID_BINARY_SCALAR(OP, VTYPE, STYPE)                                          \
  static __mlu_func__ __mlu_builtin__ void __grid_##OP(__grid_##VTYPE &dst,           \
                                                       const __grid_##VTYPE &src0,    \
                                                       STYPE src1) {                  \
    dst.__elem = __mlvm_grid_##OP##_##VTYPE##_scalar(src0.__elem, src1);              \
  }

// add
GRID_BINARY(add, f32x1);
GRID_BINARY(add, f16x2);
GRID_BINARY(add, s32x1);
GRID_BINARY(add, s16x2);
GRID_BINARY(add, s8x4);
GRID_BINARY(add, u32x1);
GRID_BINARY(add, u16x2);
GRID_BINARY(add, u8x4);
GRID_BINARY(add, f16x1);
GRID_BINARY(add, s16x1);
GRID_BINARY(add, s8x1);
GRID_BINARY(add, u16x1);
GRID_BINARY(add, u8x1);

// sub
GRID_BINARY(sub, f32x1);
GRID_BINARY(sub, f16x2);
GRID_BINARY(sub, s32x1);
GRID_BINARY(sub, s16x2);
GRID_BINARY(sub, s8x4);
GRID_BINARY(sub, u32x1);
GRID_BINARY(sub, u16x2);
GRID_BINARY(sub, u8x4);
GRID_BINARY(sub, f16x1);
GRID_BINARY(sub, s16x1);
GRID_BINARY(sub, s8x1);
GRID_BINARY(sub, u16x1);
GRID_BINARY(sub, u8x1);

// mul
GRID_BINARY(mul, f32x1);
GRID_BINARY(mul, f16x2);
GRID_BINARY(mul, s32x1);
GRID_BINARY(mul, s16x2);
GRID_BINARY(mul, s8x4);
GRID_BINARY(mul, u32x1);
GRID_BINARY(mul, u16x2);
GRID_BINARY(mul, u8x4);
GRID_BINARY(mul, f16x1);
GRID_BINARY(mul, s16x1);
GRID_BINARY(mul, s8x1);
GRID_BINARY(mul, u16x1);
GRID_BINARY(mul, u8x1);

// mulh
GRID_BINARY(mulh, s32x1);
GRID_BINARY(mulh, s16x2);
GRID_BINARY(mulh, s8x4);
GRID_BINARY(mulh, u32x1);
GRID_BINARY(mulh, u16x2);
GRID_BINARY(mulh, u8x4);
GRID_BINARY(mulh, s16x1);
GRID_BINARY(mulh, s8x1);
GRID_BINARY(mulh, u16x1);
GRID_BINARY(mulh, u8x1);

// eq
GRID_BINARY(eq, f32x1);
GRID_BINARY(eq, f16x2);
GRID_BINARY(eq, s32x1);
GRID_BINARY(eq, s16x2);
GRID_BINARY(eq, s8x4);
GRID_BINARY(eq, u32x1);
GRID_BINARY(eq, u16x2);
GRID_BINARY(eq, u8x4);
GRID_BINARY(eq, f16x1);
GRID_BINARY(eq, s16x1);
GRID_BINARY(eq, s8x1);
GRID_BINARY(eq, u16x1);
GRID_BINARY(eq, u8x1);

// ne
GRID_BINARY(ne, f32x1);
GRID_BINARY(ne, f16x2);
GRID_BINARY(ne, s32x1);
GRID_BINARY(ne, s16x2);
GRID_BINARY(ne, s8x4);
GRID_BINARY(ne, u32x1);
GRID_BINARY(ne, u16x2);
GRID_BINARY(ne, u8x4);
GRID_BINARY(ne, f16x1);
GRID_BINARY(ne, s16x1);
GRID_BINARY(ne, s8x1);
GRID_BINARY(ne, u16x1);
GRID_BINARY(ne, u8x1);

// lt
GRID_BINARY(lt, f32x1);
GRID_BINARY(lt, f16x2);
GRID_BINARY(lt, s32x1);
GRID_BINARY(lt, s16x2);
GRID_BINARY(lt, s8x4);
GRID_BINARY(lt, u32x1);
GRID_BINARY(lt, u16x2);
GRID_BINARY(lt, u8x4);
GRID_BINARY(lt, f16x1);
GRID_BINARY(lt, s16x1);
GRID_BINARY(lt, s8x1);
GRID_BINARY(lt, u16x1);
GRID_BINARY(lt, u8x1);

// ge
GRID_BINARY(ge, f32x1);
GRID_BINARY(ge, f16x2);
GRID_BINARY(ge, s32x1);
GRID_BINARY(ge, s16x2);
GRID_BINARY(ge, s8x4);
GRID_BINARY(ge, u32x1);
GRID_BINARY(ge, u16x2);
GRID_BINARY(ge, u8x4);
GRID_BINARY(ge, f16x1);
GRID_BINARY(ge, s16x1);
GRID_BINARY(ge, s8x1);
GRID_BINARY(ge, u16x1);
GRID_BINARY(ge, u8x1);

// and
GRID_BINARY(and, s32x1);
GRID_BINARY(and, s16x2);
GRID_BINARY(and, s8x4);
GRID_BINARY(and, u32x1);
GRID_BINARY(and, u16x2);
GRID_BINARY(and, u8x4);
GRID_BINARY(and, s16x1);
GRID_BINARY(and, s8x1);
GRID_BINARY(and, u16x1);
GRID_BINARY(and, u8x1);

// or
GRID_BINARY(or, s32x1);
GRID_BINARY(or, s16x2);
GRID_BINARY(or, s8x4);
GRID_BINARY(or, u32x1);
GRID_BINARY(or, u16x2);
GRID_BINARY(or, u8x4);
GRID_BINARY(or, s16x1);
GRID_BINARY(or, s8x1);
GRID_BINARY(or, u16x1);
GRID_BINARY(or, u8x1);

// xor
GRID_BINARY(xor, s32x1);
GRID_BINARY(xor, s16x2);
GRID_BINARY(xor, s8x4);
GRID_BINARY(xor, u32x1);
GRID_BINARY(xor, u16x2);
GRID_BINARY(xor, u8x4);
GRID_BINARY(xor, s16x1);
GRID_BINARY(xor, s8x1);
GRID_BINARY(xor, u16x1);
GRID_BINARY(xor, u8x1);

// max
GRID_BINARY(max, f32x1);
GRID_BINARY(max, f16x2);
GRID_BINARY(max, s32x1);
GRID_BINARY(max, s16x2);
GRID_BINARY(max, s8x4);
GRID_BINARY(max, u32x1);
GRID_BINARY(max, u16x2);
GRID_BINARY(max, u8x4);
GRID_BINARY(max, f16x1);
GRID_BINARY(max, s16x1);
GRID_BINARY(max, s8x1);
GRID_BINARY(max, u16x1);
GRID_BINARY(max, u8x1);

// min
GRID_BINARY(min, f32x1);
GRID_BINARY(min, f16x2);
GRID_BINARY(min, s32x1);
GRID_BINARY(min, s16x2);
GRID_BINARY(min, s8x4);
GRID_BINARY(min, u32x1);
GRID_BINARY(min, u16x2);
GRID_BINARY(min, u8x4);
GRID_BINARY(min, f16x1);
GRID_BINARY(min, s16x1);
GRID_BINARY(min, s8x1);
GRID_BINARY(min, u16x1);
GRID_BINARY(min, u8x1);

// land
GRID_BINARY(land, f32x1);
GRID_BINARY(land, f16x2);
GRID_BINARY(land, s32x1);
GRID_BINARY(land, s16x2);
GRID_BINARY(land, s8x4);
GRID_BINARY(land, u32x1);
GRID_BINARY(land, u16x2);
GRID_BINARY(land, u8x4);
GRID_BINARY(land, f16x1);
GRID_BINARY(land, s16x1);
GRID_BINARY(land, s8x1);
GRID_BINARY(land, u16x1);
GRID_BINARY(land, u8x1);

// lor
GRID_BINARY(lor, f32x1);
GRID_BINARY(lor, f16x2);
GRID_BINARY(lor, s32x1);
GRID_BINARY(lor, s16x2);
GRID_BINARY(lor, s8x4);
GRID_BINARY(lor, u32x1);
GRID_BINARY(lor, u16x2);
GRID_BINARY(lor, u8x4);
GRID_BINARY(lor, f16x1);
GRID_BINARY(lor, s16x1);
GRID_BINARY(lor, s8x1);
GRID_BINARY(lor, u16x1);
GRID_BINARY(lor, u8x1);

// lxor
GRID_BINARY(lxor, f32x1);
GRID_BINARY(lxor, f16x2);
GRID_BINARY(lxor, s32x1);
GRID_BINARY(lxor, s16x2);
GRID_BINARY(lxor, s8x4);
GRID_BINARY(lxor, u32x1);
GRID_BINARY(lxor, u16x2);
GRID_BINARY(lxor, u8x4);
GRID_BINARY(lxor, f16x1);
GRID_BINARY(lxor, s16x1);
GRID_BINARY(lxor, s8x1);
GRID_BINARY(lxor, u16x1);
GRID_BINARY(lxor, u8x1);

// sll
GRID_BINARY_SCALAR(sll, s32x1, unsigned);
GRID_BINARY_SCALAR(sll, s16x2, unsigned);
GRID_BINARY_SCALAR(sll, s8x4, unsigned);
GRID_BINARY_SCALAR(sll, u32x1, unsigned);
GRID_BINARY_SCALAR(sll, u16x2, unsigned);
GRID_BINARY_SCALAR(sll, u8x4, unsigned);
GRID_BINARY_SCALAR(sll, s16x1, unsigned);
GRID_BINARY_SCALAR(sll, s8x1, unsigned);
GRID_BINARY_SCALAR(sll, u16x1, unsigned);
GRID_BINARY_SCALAR(sll, u8x1, unsigned);

// srl
GRID_BINARY_SCALAR(srl, u32x1, unsigned);
GRID_BINARY_SCALAR(srl, u16x2, unsigned);
GRID_BINARY_SCALAR(srl, u8x4, unsigned);
GRID_BINARY_SCALAR(srl, u16x1, unsigned);
GRID_BINARY_SCALAR(srl, u8x1, unsigned);

// sra
GRID_BINARY_SCALAR(sra, s32x1, unsigned);
GRID_BINARY_SCALAR(sra, s16x2, unsigned);
GRID_BINARY_SCALAR(sra, s8x4, unsigned);
GRID_BINARY_SCALAR(sra, s16x1, unsigned);
GRID_BINARY_SCALAR(sra, s8x1, unsigned);

// relun
GRID_BINARY_SCALAR(relun, f32x1, float);
GRID_BINARY_SCALAR(relun, f16x2, half);
GRID_BINARY_SCALAR(relun, s32x1, int32_t);
GRID_BINARY_SCALAR(relun, s16x2, int16_t);
GRID_BINARY_SCALAR(relun, s8x4, char);
GRID_BINARY_SCALAR(relun, f16x1, half);
GRID_BINARY_SCALAR(relun, s16x1, int16_t);
GRID_BINARY_SCALAR(relun, s8x1, char);

#undef GRID_BINARY
#undef GRID_BINARY_SCALAR

// -----------------------------------------------------------------------------
// Grid unary computation
// -----------------------------------------------------------------------------
#define GRID_UNARY(OP, VTYPE)                                                             \
  static __mlu_func__ __mlu_builtin__ void __grid_##OP(__grid_##VTYPE &dst,               \
                                                       const __grid_##VTYPE &src) {       \
    dst.__elem = __mlvm_grid_##OP##_##VTYPE(src.__elem);                                  \
  }

// not
GRID_UNARY(not, s32x1);
GRID_UNARY(not, s16x2);
GRID_UNARY(not, s8x4);
GRID_UNARY(not, u32x1);
GRID_UNARY(not, u16x2);
GRID_UNARY(not, u8x4);
GRID_UNARY(not, s16x1);
GRID_UNARY(not, s8x1);
GRID_UNARY(not, u16x1);
GRID_UNARY(not, u8x1);

// lnot
GRID_UNARY(lnot, f32x1);
GRID_UNARY(lnot, f16x2);
GRID_UNARY(lnot, s32x1);
GRID_UNARY(lnot, s16x2);
GRID_UNARY(lnot, s8x4);
GRID_UNARY(lnot, u32x1);
GRID_UNARY(lnot, u16x2);
GRID_UNARY(lnot, u8x4);
GRID_UNARY(lnot, f16x1);
GRID_UNARY(lnot, s16x1);
GRID_UNARY(lnot, s8x1);
GRID_UNARY(lnot, u16x1);
GRID_UNARY(lnot, u8x1);

// abs
GRID_UNARY(abs, f32x1);
GRID_UNARY(abs, f16x2);
GRID_UNARY(abs, s32x1);
GRID_UNARY(abs, s16x2);
GRID_UNARY(abs, s8x4);
GRID_UNARY(abs, f16x1);
GRID_UNARY(abs, s16x1);
GRID_UNARY(abs, s8x1);

// relu
GRID_UNARY(relu, f32x1);
GRID_UNARY(relu, f16x2);
GRID_UNARY(relu, s32x1);
GRID_UNARY(relu, s16x2);
GRID_UNARY(relu, s8x4);
GRID_UNARY(relu, f16x1);
GRID_UNARY(relu, s16x1);
GRID_UNARY(relu, s8x1);

#undef GRID_UNARY

// -----------------------------------------------------------------------------
// Grid shift.
// -----------------------------------------------------------------------------
#define GRID_SHIFT(DIR, VTYPE)                                                              \
  static __mlu_func__ __mlu_builtin__ void __grid_shift_##DIR(__grid_##VTYPE &dst,          \
                                                              const __grid_##VTYPE &src) {  \
    dst.__elem = __mlvm_grid_shift_##DIR##_##VTYPE(src.__elem);                             \
  }

GRID_SHIFT(up, f32x1);
GRID_SHIFT(up, f16x2);
GRID_SHIFT(up, s32x1);
GRID_SHIFT(up, s16x2);
GRID_SHIFT(up, s8x4);
GRID_SHIFT(up, u32x1);
GRID_SHIFT(up, u16x2);
GRID_SHIFT(up, u8x4);
GRID_SHIFT(up, f16x1);
GRID_SHIFT(up, s16x1);
GRID_SHIFT(up, s8x1);
GRID_SHIFT(up, u16x1);
GRID_SHIFT(up, u8x1);

GRID_SHIFT(down, f32x1);
GRID_SHIFT(down, f16x2);
GRID_SHIFT(down, s32x1);
GRID_SHIFT(down, s16x2);
GRID_SHIFT(down, s8x4);
GRID_SHIFT(down, u32x1);
GRID_SHIFT(down, u16x2);
GRID_SHIFT(down, u8x4);
GRID_SHIFT(down, f16x1);
GRID_SHIFT(down, s16x1);
GRID_SHIFT(down, s8x1);
GRID_SHIFT(down, u16x1);
GRID_SHIFT(down, u8x1);

GRID_SHIFT(left, f32x1);
GRID_SHIFT(left, f16x2);
GRID_SHIFT(left, s32x1);
GRID_SHIFT(left, s16x2);
GRID_SHIFT(left, s8x4);
GRID_SHIFT(left, u32x1);
GRID_SHIFT(left, u16x2);
GRID_SHIFT(left, u8x4);
GRID_SHIFT(left, f16x1);
GRID_SHIFT(left, s16x1);
GRID_SHIFT(left, s8x1);
GRID_SHIFT(left, u16x1);
GRID_SHIFT(left, u8x1);

GRID_SHIFT(right, f32x1);
GRID_SHIFT(right, f16x2);
GRID_SHIFT(right, s32x1);
GRID_SHIFT(right, s16x2);
GRID_SHIFT(right, s8x4);
GRID_SHIFT(right, u32x1);
GRID_SHIFT(right, u16x2);
GRID_SHIFT(right, u8x4);
GRID_SHIFT(right, f16x1);
GRID_SHIFT(right, s16x1);
GRID_SHIFT(right, s8x1);
GRID_SHIFT(right, u16x1);
GRID_SHIFT(right, u8x1);

#endif  // __BANG_ARCH__ > 306

#endif  // __BANG_GRID_H__
