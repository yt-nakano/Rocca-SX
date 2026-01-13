#ifndef INTRIN_SUBSET_ARM_H
#define INTRIN_SUBSET_ARM_H

#if !defined(__ARM_FEATURE_AES)
//# error __ARM_FEATURE_AES is not defined
# include "intrin_subset_c_emu.h"
#else

#include <arm_neon.h>

#ifndef  __m128i
# define __m128i uint8x16_t
#endif

#ifndef  _mm_load_si128
# define _mm_load_si128(p)      vld1q_u8((unsigned char*)(p))
#endif

#ifndef  _mm_loadu_si128
# define _mm_loadu_si128(p)     vld1q_u8((unsigned char*)(p))
#endif

#ifndef  _mm_lddq_si128
# define _mm_lddq_si128(p)      vld1q_u8((unsigned char*)(p))
#endif

#ifndef  _mm_lddqu_si128
# define _mm_lddqu_si128(p)     vld1q_u8((unsigned char*)(p))
#endif

#ifndef  _mm_store_si128
# define _mm_store_si128(p,v)   vst1q_u8((unsigned char*)(p),v)
#endif

#ifndef  _mm_storeu_si128
# define _mm_storeu_si128(p,v)  vst1q_u8((unsigned char*)(p),v)
#endif

#ifndef  _mm_and_si128
# define _mm_and_si128(a,b)     vandq_u8(a,b)
#endif

#ifndef  _mm_xor_si128
# define _mm_xor_si128(a,b)     veorq_u8(a,b)
#endif

#ifndef  _mm_setzero_si128
# define _mm_setzero_si128()    vdupq_n_u8(0)
#endif

#ifndef  _mm_set_epi64x
# define _mm_set_epi64x(e1,e0)  vcombine_u8(vcreate_u8(e0),vcreate_u8(e1))
#endif

#ifndef  _mm_aesenc_si128
# define _mm_aesenc_si128(d,k)  veorq_u8(vaesmcq_u8(vaeseq_u8(d,vdupq_n_u8(0))),k)
#endif

#ifndef  _mm_aessbsrmc_si128
# define _mm_aessbsrmc_si128(d)  vaesmcq_u8(vaeseq_u8(d,vdupq_n_u8(0)))
#endif



#ifndef  __m256i
# define __M256I uint8x16x2_t
# define __m256i __M256I
#endif

#ifndef  _mm256_loadu_si256
# define _mm256_loadu_si256 _MM256_loadu_si256
static inline __M256I _MM256_loadu_si256(__M256I const* mem_addr) {
    __M256I r;
    uint8_t * p = (uint8_t*)mem_addr;
    for(size_t i = 0; i < 2; ++i) { r.val[i] = _mm_loadu_si128((__m128i*)(p+16*i)); }
    return r;
}
#endif

#ifndef  _mm256_storeu_si256
# define _mm256_storeu_si256 _MM256_storeu_si256
static inline void _MM256_storeu_si256(__M256I const* mem_addr, __M256I a) {
    uint8_t * p = (uint8_t*)mem_addr;
    for(size_t i = 0; i < 2; ++i) { _mm_storeu_si128((__m128i*)(p+16*i), a.val[i]); }
}
#endif

#ifndef  _mm256_and_si256
# define _mm256_and_si256 _MM256_and_si256
static inline __M256I _MM256_and_si256(__M256I a, __M256I b) {
    __M256I r;
    for(size_t i = 0; i < 2; ++i) { r.val[i] = _mm_and_si128(a.val[i], b.val[i]); }
    return r;
}
#endif

#ifndef  _mm256_xor_si256
# define _mm256_xor_si256 _MM256_xor_si256
static inline __M256I _MM256_xor_si256(__M256I a, __M256I b) {
    __M256I r;
    for(size_t i = 0; i < 2; ++i) { r.val[i] = _mm_xor_si128(a.val[i], b.val[i]); }
    return r;
}
#endif

#ifndef  _mm256_castsi256_si128
# define _mm256_castsi256_si128 _MM256_castsi256_si128
static inline __m128i _MM256_castsi256_si128(__M256I a) {
    return a.val[0];
}
#endif

#ifndef  _mm256_extractf128_si256
# define _mm256_extractf128_si256 _MM256_extractf128_si256
static inline __m128i _MM256_extractf128_si256(__M256I a, const int imm8) {
    if ((imm8 < 0) || (2 <= imm8)) { return _mm_setzero_si128(); }
    return a.val[imm8];
}
#endif

#ifndef  _mm256_extracti128_si256
# define _mm256_extracti128_si256 _MM256_extracti128_si256
static inline __m128i _MM256_extracti128_si256(__M256I a, const int imm8) {
    if ((imm8 < 0) || (2 <= imm8)) { return _mm_setzero_si128(); }
    return a.val[imm8];
}
#endif

#ifndef  _mm256_set_m128i
# define _mm256_set_m128i _MM256_set_m128i
static inline __M256I _MM256_set_m128i(__m128i hi, __m128i lo) {
    __M256I r;
    r.val[0] = lo;
    r.val[1] = hi;
    return r;
}
#endif

#ifndef  _mm256_setzero_si256
# define _mm256_setzero_si256 _MM256_setzero_si256
static inline __M256I _MM256_setzero_si256() {
    __M256I r;
    for(size_t i = 0; i < 2; ++i) { r.val[i] = _mm_setzero_si128(); }
    return r;
}
#endif

#ifndef  _mm256_set_epi64x
# define _mm256_set_epi64x _MM256_set_epi64x
static inline __M256I _MM256_set_epi64x(int64_t e3, int64_t e2, int64_t e1, int64_t e0) {
    int64_t arrVal64[] = {e0, e1, e2, e3};
    __M256I r;
    for(size_t i = 0; i < 2; ++i) { r.val[i] = _mm_set_epi64x(arrVal64[2*i + 1], arrVal64[2*i + 0]); }
    return r;
}
#endif

#ifndef  _mm256_aesenc_epi128
# define _mm256_aesenc_epi128 _MM256_aesenc_epi128
#if 0
static inline __M256I _MM256_aesenc_epi128(__M256I a, __M256I RoundKey) {
    __M256I c;
    c.val[0] = _mm_aesenc_si128(a.val[0], RoundKey.val[0]);
    c.val[1] = _mm_aesenc_si128(a.val[1], RoundKey.val[1]);
    return c;
}
#else
static inline __M256I _MM256_aesenc_epi128(__M256I a, __M256I RoundKey) {
    __M256I c;
    __m128i zero = vdupq_n_u8(0);
    c.val[0] = vaeseq_u8(a.val[0], zero);
    c.val[1] = vaeseq_u8(a.val[1], zero);
    c.val[0] = vaesmcq_u8(c.val[0]);
    c.val[1] = vaesmcq_u8(c.val[1]);
    c.val[0] = veorq_u8(c.val[0], RoundKey.val[0]);
    c.val[1] = veorq_u8(c.val[1], RoundKey.val[1]);
    return c;
}
#endif
#endif


#ifndef  __m512i
# define __M512I uint8x16x4_t
# define __m512i __M512I
#endif

#ifndef  _mm512_loadu_si512
# define _mm512_loadu_si512 _MM512_loadu_si512
static inline __M512I _MM512_loadu_si512(__M512I const* mem_addr) {
    __M512I r;
    uint8_t * p = (uint8_t*)mem_addr;
    for(size_t i = 0; i < 4; ++i) { r.val[i] = _mm_loadu_si128((__m128i*)(p+16*i)); }
    return r;
}
#endif

#ifndef  _mm512_storeu_si512
# define _mm512_storeu_si512 _MM512_storeu_si512
static inline void _MM512_storeu_si512(__M512I const* mem_addr, __M512I a) {
    uint8_t * p = (uint8_t*)mem_addr;
    for(size_t i = 0; i < 4; ++i) { _mm_storeu_si128((__m128i*)(p+16*i), a.val[i]); }
}
#endif

#ifndef  _mm512_and_si512
# define _mm512_and_si512 _MM512_and_si512
static inline __M512I _MM512_and_si512(__M512I a, __M512I b) {
    __M512I r;
    for(size_t i = 0; i < 4; ++i) { r.val[i] = _mm_and_si128(a.val[i], b.val[i]); }
    return r;
}
#endif

#ifndef  _mm512_xor_si512
# define _mm512_xor_si512 _MM512_xor_si512
static inline __M512I _MM512_xor_si512(__M512I a, __M512I b) {
    __M512I r;
    for(size_t i = 0; i < 4; ++i) { r.val[i] = _mm_xor_si128(a.val[i], b.val[i]); }
    return r;
}
#endif

#ifndef  _mm512_castsi512_si128
# define _mm512_castsi512_si128 _MM512_castsi512_si128
static inline __m128i _MM512_castsi512_si128(__M512I a) {
    return a.val[0];
}
#endif

#ifndef  _mm512_castsi128_si512
# define _mm512_castsi128_si512 _MM512_castsi128_si512
static inline __M512I _MM512_castsi128_si512(__m128i a) {
    __M512I r;
    r.val[0] = a;
    return r;
}
#endif

#ifndef  _mm512_extracti32x4_epi32
# define _mm512_extracti32x4_epi32 _MM512_extracti32x4_epi32
static inline __m128i _MM512_extracti32x4_epi32(__M512I a, const int imm8) {
    if ((imm8 < 0) || (4 <= imm8)) { return _mm_setzero_si128(); }
    return a.val[imm8];
}
#endif

#ifndef  _mm512_inserti32x4
# define _mm512_inserti32x4 _MM512_inserti32x4
static inline __M512I _MM512_inserti32x4(__M512I a, __m128i b, int imm8) {
    __M512I r;
    for(size_t i = 0; i < 4; ++i) { r.val[i] = a.val[i]; }
    if ((0 <= imm8) && (imm8 < 4)) { r.val[imm8] = b; }
    return r;
}
#endif

#ifndef  _mm512_setzero_si512
# define _mm512_setzero_si512 _MM512_setzero_si512
static inline __M512I _MM512_setzero_si512() {
    __M512I r;
    for(size_t i = 0; i < 4; ++i) { r.val[i] = _mm_setzero_si128(); }
    return r;
}
#endif

#ifndef  _mm512_set_epi64
# define _mm512_set_epi64 _MM512_set_epi64
static inline __M512I _MM512_set_epi64(int64_t e7, int64_t e6, int64_t e5, int64_t e4, int64_t e3, int64_t e2, int64_t e1, int64_t e0) {
    int64_t arrVal64[] = {e0, e1, e2, e3, e4, e5, e6, e7};
    __M512I r;
    for(size_t i = 0; i < 4; ++i) { r.val[i] = _mm_set_epi64x(arrVal64[2*i + 1], arrVal64[2*i + 0]); }
    return r;
}
#endif

#ifndef  _mm512_aesenc_epi128
# define _mm512_aesenc_epi128 _MM512_aesenc_epi128
#if 0
static inline __M512I _MM512_aesenc_epi128(__M512I a, __M512I RoundKey) {
    __M512I c;
    c.val[0] = _mm_aesenc_si128(a.val[0], RoundKey.val[0]);
    c.val[1] = _mm_aesenc_si128(a.val[1], RoundKey.val[1]);
    c.val[2] = _mm_aesenc_si128(a.val[2], RoundKey.val[2]);
    c.val[3] = _mm_aesenc_si128(a.val[3], RoundKey.val[3]);
    return c;
}
#else
static inline __M512I _MM512_aesenc_epi128(__M512I a, __M512I RoundKey) {
    __M512I c;
    __m128i zero = vdupq_n_u8(0);
    c.val[0] = vaeseq_u8(a.val[0], zero);
    c.val[1] = vaeseq_u8(a.val[1], zero);
    c.val[0] = vaesmcq_u8(c.val[0]);
    c.val[1] = vaesmcq_u8(c.val[1]);
    c.val[0] = veorq_u8(c.val[0], RoundKey.val[0]);
    c.val[1] = veorq_u8(c.val[1], RoundKey.val[1]);

    c.val[2] = vaeseq_u8(a.val[2], zero);
    c.val[3] = vaeseq_u8(a.val[3], zero);
    c.val[2] = vaesmcq_u8(c.val[2]);
    c.val[3] = vaesmcq_u8(c.val[3]);
    c.val[2] = veorq_u8(c.val[2], RoundKey.val[2]);
    c.val[3] = veorq_u8(c.val[3], RoundKey.val[3]);
    return c;
}
#endif
#endif

#endif

#endif
