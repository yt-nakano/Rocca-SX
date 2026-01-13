#ifndef INTRIN_SUBSET_H
#define INTRIN_SUBSET_H

#if defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(__amd64__)
# include "intrin_subset_x64.h"
#elif defined(__aarch64__)
# include "intrin_subset_arm.h"
#else
# include "intrin_subset_c_emu.h"
#endif

#endif
