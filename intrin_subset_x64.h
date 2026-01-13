#ifndef INTRIN_SUBSET_X64_H
#define INTRIN_SUBSET_X64_H

#include "stdint.h"

#if defined(__AES__)
# define CPU_SUPPORTS_AES128
#endif

#if defined(__AVX512VL__) && defined(__VAES__)
# define CPU_SUPPORTS_AES256
#endif

#if defined(__AVX512F__) && defined(__VAES__)
# define CPU_SUPPORTS_AES512
#endif


#if !defined(CPU_SUPPORTS_AES128) && !defined(CPU_SUPPORTS_AES256) && !defined(CPU_SUPPORTS_AES512)
# include "intrin_subset_c_emu.h"
#else


#if defined(CPU_SUPPORTS_AES128)
# include <wmmintrin.h>
#else

typedef struct __M128I_ST {
    uint8_t val[16];
} __M128I;

static inline __M128I _MM_loadu_si128(const __M128I* mem_addr) {
    __M128I a;
    memcpy(a.val, mem_addr, sizeof(a.val));
    return a;
}

static inline void _MM_storeu_si128(__M128I* mem_addr, __M128I a) {
    memcpy(mem_addr, a.val, sizeof(a.val));
}

static inline __M128I _MM_xor_si128(__M128I a, __M128I b) {
    __M128I c;
    for (size_t i = 0; i < sizeof(c.val); ++i) {
        c.val[i] = a.val[i] ^ b.val[i];
    }
    return c;
}

static inline __M128I _MM_and_si128(__M128I a, __M128I b) {
    __M128I c;
    for (size_t i = 0; i < sizeof(c.val); ++i) {
        c.val[i] = a.val[i] & b.val[i];
    }
    return c;
}

static inline __M128I _MM_setzero_si128() {
    __M128I c;
    for (size_t i = 0; i < sizeof(c.val); ++i) {
        c.val[i] = 0;
    }
    return c;
}

static inline __M128I _MM_set_epi64x(int64_t e1, int64_t e0) {
    int64_t arrVal64[] = {e0, e1};
    __M128I c;
    size_t i = 0;
    for(size_t h = 0; h < sizeof(arrVal64)/sizeof(arrVal64[0]); ++h) {
	    for(size_t j = 0; j < sizeof(arrVal64[0]); ++j) {
	        c.val[i] = arrVal64[h] >> 8*j;
	        ++i;
	    }
	}
    return c;
}

static inline __M128I _MM_aesenc_si128(__M128I a, __M128I RoundKey) {
    __M128I c;
    AESROUND(c.val, a.val, RoundKey.val);
    return c;
}

#define __m128i                __M128I
#define _mm_loadu_si128(m)     _MM_loadu_si128((const __M128I *)(m))
#define _mm_storeu_si128(m,a)  _MM_storeu_si128((__M128I *)(m),a)
#define _mm_xor_si128(a,b)     _MM_xor_si128(a,b)
#define _mm_and_si128(a,b)     _MM_and_si128(a,b)
#define _mm_aesenc_si128(a,k)  _MM_aesenc_si128(a,k)
#define _mm_setzero_si128()    _MM_setzero_si128()
#define _mm_set_epi64x(e1,e0)  _MM_set_epi64x(e1,e0)

#endif


#if defined(CPU_SUPPORTS_AES256)
# include <immintrin.h>
#else

#undef  NUM_M128_IN_Mbit_STRUCT
#define NUM_M128_IN_Mbit_STRUCT 2

typedef struct __M256I_ST {
    __m128i m128[NUM_M128_IN_Mbit_STRUCT];
} __M256I;

static inline __M256I _MM256_loadu_si256(const __M256I* mem_addr) {
    __M256I a;
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        a.m128[i] = _mm_loadu_si128((const __m128i*)((const uint8_t*)mem_addr + 16 * i));
    }
    return a;
}

static inline void _MM256_storeu_si256(__M256I* mem_addr, __M256I a) {
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        _mm_storeu_si128((__m128i*)((uint8_t*)mem_addr + 16 * i), a.m128[i]);
    }
}

static inline __M256I _MM256_xor_si256(__M256I a, __M256I b) {
    __M256I c;
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        c.m128[i] = _mm_xor_si128(a.m128[i], b.m128[i]);
    }
    return c;
}

static inline __M256I _MM256_and_si256(__M256I a, __M256I b) {
    __M256I c;
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        c.m128[i] = _mm_and_si128(a.m128[i], b.m128[i]);
    }
    return c;
}

static inline __M256I _MM256_setzero_si256() {
    __M256I c;
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        c.m128[i] = _mm_setzero_si128();
    }
    return c;
}

static inline __M256I _MM256_set_epi64x(int64_t e3, int64_t e2, int64_t e1, int64_t e0) {
    int64_t arrVal64[] = {e0, e1, e2, e3};
    __M256I c;
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        c.m128[i] = _mm_set_epi64x(arrVal64[2*i + 1], arrVal64[2*i + 0]);
    }
    return c;
}

static inline __M256I _MM256_aesenc_epi128(__M256I a, __M256I RoundKey) {
    __M256I c;
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        c.m128[i] = _mm_aesenc_si128(a.m128[i], RoundKey.m128[i]);
    }
    return c;
}

static __m128i _MM256_extracti128_si256(__M256I a, const int imm8) {
    if ((0 <= imm8) && (imm8 < NUM_M128_IN_Mbit_STRUCT)) {
        return a.m128[imm8];
    }
    else {
        return _mm_setzero_si128();
    }
}

#define __m256i                        __M256I
#define _mm256_loadu_si256(m)          _MM256_loadu_si256((const __M256I *)(m))
#define _mm256_storeu_si256(m,a)       _MM256_storeu_si256((__M256I *)(m),a)
#define _mm256_xor_si256(a,b)          _MM256_xor_si256(a,b)
#define _mm256_and_si256(a,b)          _MM256_and_si256(a,b)
#define _mm256_aesenc_epi128(a,k)      _MM256_aesenc_epi128(a,k)
#define _mm256_setzero_si256()         _MM256_setzero_si256()
#define _mm256_set_epi64x(e3,e2,e1,e0) _MM256_set_epi64x(e3,e2,e1,e0)
#define _mm256_extracti128_si256(a,i)  _MM256_extracti128_si256(a,i)

#endif


#if defined(CPU_SUPPORTS_AES512)
# include <immintrin.h>
#else

#undef  NUM_M128_IN_Mbit_STRUCT
#define NUM_M128_IN_Mbit_STRUCT 4

typedef struct __M512I_ST {
    __m128i m128[NUM_M128_IN_Mbit_STRUCT];
} __M512I;

static inline __M512I _MM512_loadu_si512(const __M512I* mem_addr) {
    __M512I a;
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        a.m128[i] = _mm_loadu_si128((const __m128i*)((const uint8_t*)mem_addr + 16 * i));
    }
    return a;
}

static inline void _MM512_storeu_si512(__M512I* mem_addr, __M512I a) {
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        _mm_storeu_si128((__m128i*)((uint8_t*)mem_addr + 16 * i), a.m128[i]);
    }
}

static inline __M512I _MM512_xor_si512(__M512I a, __M512I b) {
    __M512I c;
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        c.m128[i] = _mm_xor_si128(a.m128[i], b.m128[i]);
    }
    return c;
}

static inline __M512I _MM512_and_si512(__M512I a, __M512I b) {
    __M512I c;
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        c.m128[i] = _mm_and_si128(a.m128[i], b.m128[i]);
    }
    return c;
}

static inline __M512I _MM512_setzero_si512() {
    __M512I c;
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        c.m128[i] = _mm_setzero_si128();
    }
    return c;
}

static inline __M512I _MM512_set_epi64(int64_t e7, int64_t e6, int64_t e5, int64_t e4, int64_t e3, int64_t e2, int64_t e1, int64_t e0) {
    int64_t arrVal64[] = {e0, e1, e2, e3, e4, e5, e6, e7};
    __M512I c;
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        c.m128[i] = _mm_set_epi64x(arrVal64[2*i + 1], arrVal64[2*i + 0]);
    }
    return c;
}

static inline __M512I _MM512_aesenc_epi128(__M512I a, __M512I RoundKey) {
    __M512I c;
    for(size_t i = 0; i < NUM_M128_IN_Mbit_STRUCT; ++i) {
        c.m128[i] = _mm_aesenc_si128(a.m128[i], RoundKey.m128[i]);
    }
    return c;
}

static __m128i _MM512_extracti32x4_epi32(__M512I a, int imm8) {
    if ((0 <= imm8) && (imm8 < NUM_M128_IN_Mbit_STRUCT)) {
        return a.m128[imm8];
    }
    else {
        return _mm_setzero_si128();
    }
}

#define __m512i                    __M512I
#define _mm512_loadu_si512(m)      _MM512_loadu_si512((const __M512I *)(m))
#define _mm512_storeu_si512(m,a)   _MM512_storeu_si512((__M512I *)(m),a)
#define _mm512_xor_si512(a,b)      _MM512_xor_si512(a,b)
#define _mm512_and_si512(a,b)      _MM512_and_si512(a,b)
#define _mm512_aesenc_epi128(a,k)  _MM512_aesenc_epi128(a,k)
#define _mm512_setzero_si512()     _MM512_setzero_si512()
#define _mm512_set_epi64(e7,e6,e5,e4,e3,e2,e1,e0)  _MM512_set_epi64(e7,e6,e5,e4,e3,e2,e1,e0)
#define _mm512_extracti32x4_epi32(a,i)  _MM512_extracti32x4_epi32(a,i)

#endif

#endif

#endif
