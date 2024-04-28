#ifndef MLISA_BANG_DEVICE_FUNCTIONS_H
#define MLISA_BANG_DEVICE_FUNCTIONS_H

#include <spirv/spirv.h>

void __sync_all() {
  if (__mlvm_read_mlu_sreg_clusterdim() == 0) {
    __asm__ __volatile__("sync;\n\t");
  } else {
    __asm__ __volatile__("barrier.sync.local 32772, %[cnt];\n\t"
                           ::[cnt]"r"(__mlvm_read_mlu_sreg_coredim() + 1));
  }
}

void __sync_all_arrive(unsigned int id, unsigned int count);

void __sync_all_ipu();

void __sycl_all_ipu_within_cluster();

void __sync_all_mpu();

void __sync_all_wait(unsigned int id, unsigned int count);

void __sync_cluster();

void __sync_cluster_arrive(unsigned int id, unsigned int count);

void __sync_cluster_wait(unsigned int id, unsigned int count);

void __sync();

void __sync_compute();

void __sync_io();

void __sync_move();

#endif // MLISA_BANG_DEVICE_FUNCTIONS_H