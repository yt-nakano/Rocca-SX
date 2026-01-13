
#include <memory.h>
#include "intrin_subset.h"
#include "rocca-sx2.h"

#define load(m)      _mm256_loadu_si256((const __m256i *)(m))
#define store(m,a)   _mm256_storeu_si256((__m256i *)(m),a)
#define xor(a,b)     _mm256_xor_si256(a,b)
#define and(a,b)     _mm256_and_si256(a,b)
#define enc(a,k)     _mm256_aesenc_epi128(a,k)
#define setzero()    _mm256_setzero_si256()
#define extract(a,i) _mm256_extracti128_si256(a,i)

#define load128(m)    _mm_loadu_si128((const __m128i *)(m))
#define store128(m,a) _mm_storeu_si128((__m128i *)(m),a)
#define xor128(a,b)   _mm_xor_si128(a,b)
#define setzero128()  _mm_setzero_si128()

#define ENCODE_IN_LITTLE_ENDIAN(bytes, v) \
  bytes[ 0] = ((uint64_t)(v) << (    3)); \
  bytes[ 1] = ((uint64_t)(v) >> (1*8-3)); \
  bytes[ 2] = ((uint64_t)(v) >> (2*8-3)); \
  bytes[ 3] = ((uint64_t)(v) >> (3*8-3)); \
  bytes[ 4] = ((uint64_t)(v) >> (4*8-3)); \
  bytes[ 5] = ((uint64_t)(v) >> (5*8-3)); \
  bytes[ 6] = ((uint64_t)(v) >> (6*8-3)); \
  bytes[ 7] = ((uint64_t)(v) >> (7*8-3)); \
  bytes[ 8] = ((uint64_t)(v) >> (8*8-3)); \
  bytes[ 9] = 0; \
  bytes[10] = 0; \
  bytes[11] = 0; \
  bytes[12] = 0; \
  bytes[13] = 0; \
  bytes[14] = 0; \
  bytes[15] = 0;

#define FLOORTO(a,b) ((a) / (b) * (b))

#define P_NUM      ( 2)
#define S_NUM      ROCCA_SX2_STATE_NUM
#define M_NUM      ( 2)
#define INIT_LOOP  (16)
#define TAG_LOOP   (16)
#define MSGBLKSIZE ROCCA_SX2_MSG_BLOCK_SIZE

#define DECL_VARS_4_UPDATE \
  __m256i state[S_NUM], stateNew[S_NUM], M[M_NUM];

#define DECL_VARS_4_INIT \
  DECL_VARS_4_UPDATE; __m256i k[2];
  
#define DECL_VARS_4_ENCRYPT \
  DECL_VARS_4_UPDATE; __m256i Z[M_NUM], C[M_NUM];

#define COPY_TO_LOCAL(ctx) \
  for(size_t i = 0; i < S_NUM; ++i) { state[i] = load(&((ctx)->state[i][0])); }
  
#define COPY_FROM_LOCAL(ctx) \
  for(size_t i = 0; i < S_NUM; ++i) { store(&((ctx)->state[i][0]), state[i]); }

static __m256i broadcast(const uint8_t * data16bytes) {
	uint8_t tmp[16 * P_NUM];
	for (size_t i = 0; i < P_NUM; ++i) {
		memcpy(tmp + 16 * i, data16bytes, 16);
	}
	return load(tmp);
}

static __m256i broadcast_with_index(const uint8_t* data16bytes) {
	uint8_t tmp[16 * P_NUM];
	for (size_t i = 0; i < P_NUM; ++i) {
		memcpy(tmp + 16 * i, data16bytes, 16);
		*(tmp + 16 * i) ^= i;
	}
	return load(tmp);
}

#define UPDATE_STATE(X) \
  stateNew[0] = xor(state[6], state[1]); \
  stateNew[1] = enc(state[0],     X[0]); \
  stateNew[2] = enc(state[1], state[0]); \
  stateNew[3] = enc(state[2], state[6]); \
  stateNew[4] = enc(state[3],     X[1]); \
  stateNew[5] = enc(state[4], state[3]); \
  stateNew[6] = enc(state[5], state[4]); \
  for(size_t i = 0; i < S_NUM; ++i) {state[i] = stateNew[i];}


  
#define INIT_STATE(key, iv) \
  k[0] = broadcast((key) + 16*0); \
  k[1] = broadcast((key) + 16*1); \
  state[0] = k[1]; \
  state[1] = broadcast_with_index(iv); \
  state[2] = broadcast(Z0); \
  state[3] = k[0]; \
  state[4] = broadcast(Z1); \
  state[5] = xor(state[1], state[0]); \
  state[6] = setzero(); \
  M[0] = state[2]; \
  M[1] = state[4]; \
  for(size_t i = 0; i < INIT_LOOP; ++i) { \
    UPDATE_STATE(M); \
  } \
  state[0] = xor(state[0], k[0]); \
  state[1] = xor(state[1], k[0]); \
  state[2] = xor(state[2], k[1]); \
  state[3] = xor(state[3], k[0]); \
  state[4] = xor(state[4], k[0]); \
  state[5] = xor(state[5], k[1]); \
  state[6] = xor(state[6], k[1]);


#define MAKE_STRM(z) \
  z[0] = enc(xor(state[3], state[5]), state[0]); \
  z[1] = enc(xor(state[4], state[6]), state[2]);

#define MSG_LOAD(mem, reg) \
  reg[0] = load((mem) + (MSGBLKSIZE/2)*0); \
  reg[1] = load((mem) + (MSGBLKSIZE/2)*1);

#define MSG_STORE(mem, reg) \
  store((mem) + (MSGBLKSIZE/2)*0, reg[0]); \
  store((mem) + (MSGBLKSIZE/2)*1, reg[1]);

#define XOR_BLOCK(dst, src1, src2) \
  dst[0] = xor(src1[0], src2[0]); \
  dst[1] = xor(src1[1], src2[1]); 

#define MASKXOR_BLOCK(dst, src1, src2, mask) \
  dst[0] = and(xor(src1[0], src2[0]), mask[0]); \
  dst[1] = and(xor(src1[1], src2[1]), mask[1]);
  
#define ADD_AD(input) \
  MSG_LOAD(input, M); \
  UPDATE_STATE(M);
  
#define ADD_AD_LAST_BLOCK(input, size) \
  uint8_t tmpblk[MSGBLKSIZE] = {0}; \
  memcpy(tmpblk, input, size); \
  MSG_LOAD(tmpblk, M); \
  UPDATE_STATE(M);
  
#define ENCRYPT(output, input) \
  MSG_LOAD(input, M); \
  MAKE_STRM(Z); \
  XOR_BLOCK(C, M, Z); \
  MSG_STORE(output, C); \
  UPDATE_STATE(M);

#define ENCRYPT_LAST_BLOCK(output, input, size) \
  uint8_t tmpblk[MSGBLKSIZE] = {0}; \
  memcpy(tmpblk, input, size); \
  MSG_LOAD(tmpblk, M); \
  MAKE_STRM(Z); \
  XOR_BLOCK(C, M, Z); \
  MSG_STORE(tmpblk, C); \
  memcpy(output, tmpblk, size); \
  UPDATE_STATE(M);

#define DECRYPT(output, input) \
  MSG_LOAD(input, C); \
  MAKE_STRM(Z); \
  XOR_BLOCK(M, C, Z); \
  MSG_STORE(output, M); \
  UPDATE_STATE(M);

#define DECRYPT_LAST_BLOCK(output, input, size) \
  uint8_t tmpblk[MSGBLKSIZE] = {0}; \
  uint8_t tmpmsk[MSGBLKSIZE] = {0}; \
  __m256i mask[M_NUM]; \
  memcpy(tmpblk, input, size); \
  memset(tmpmsk, 0xFF , size); \
  MSG_LOAD(tmpblk, C   ); \
  MSG_LOAD(tmpmsk, mask); \
  MAKE_STRM(Z); \
  MASKXOR_BLOCK(M, C, Z, mask); \
  MSG_STORE(tmpblk, M); \
  memcpy(output, tmpblk, size); \
  UPDATE_STATE(M);

#define SET_AD_BITLEN_MSG_BITLEN(sizeAD, sizeM) \
  uint8_t bitlenAD[16]; \
  uint8_t bitlenM [16]; \
  ENCODE_IN_LITTLE_ENDIAN(bitlenAD, sizeAD); \
  ENCODE_IN_LITTLE_ENDIAN(bitlenM , sizeM ); \
  M[0] = broadcast(bitlenAD); \
  M[1] = broadcast(bitlenM );

#define MAKE_TAG(sizeAD, sizeM, tag) \
  SET_AD_BITLEN_MSG_BITLEN(sizeAD, sizeM); \
  for(size_t i = 0; i < TAG_LOOP; ++i) { \
    UPDATE_STATE(M); \
  } \
  __m256i tag256a = setzero(); \
  for(size_t i = 0; i <= 3; ++i) { \
    tag256a = xor(tag256a, state[i]); \
  } \
  __m256i tag256b = setzero(); \
  for(size_t i = 4; i <= 6; ++i) { \
    tag256b = xor(tag256b, state[i]); \
  } \
  __m128i tag128[2] = {setzero128(), setzero128()}; \
  for(size_t i = 0; i < P_NUM; ++i) { \
    tag128[0] = xor128(tag128[0], extract(tag256a, i)); \
    tag128[1] = xor128(tag128[1], extract(tag256b, i)); \
  } \
  store128((tag)+16*0, tag128[0]); \
  store128((tag)+16*1, tag128[1]);

static const uint8_t Z0[] = {0xcd,0x65,0xef,0x23,0x91,0x44,0x37,0x71,0x22,0xae,0x28,0xd7,0x98,0x2f,0x8a,0x42};
static const uint8_t Z1[] = {0xbc,0xdb,0x89,0x81,0xa5,0xdb,0xb5,0xe9,0x2f,0x3b,0x4d,0xec,0xcf,0xfb,0xc0,0xb5};

void rocca_sx2_init(rocca_sx2_context * ctx, const uint8_t * key, const uint8_t * iv) {
	DECL_VARS_4_INIT;
	INIT_STATE(key, iv);
	COPY_FROM_LOCAL(ctx);
	ctx->size_ad = 0;
	ctx->size_m  = 0;
}

void rocca_sx2_add_ad(rocca_sx2_context * ctx, const uint8_t * in, size_t size) {
	DECL_VARS_4_UPDATE;
	COPY_TO_LOCAL(ctx);	
	size_t i = 0;
	for(size_t size2 = FLOORTO(size, MSGBLKSIZE); i < size2; i += MSGBLKSIZE) {
		ADD_AD(in + i);
	}
	if(i < size) {
		ADD_AD_LAST_BLOCK(in + i, size - i);
	}
	COPY_FROM_LOCAL(ctx);
	ctx->size_ad += size;
}

void rocca_sx2_encrypt(rocca_sx2_context * ctx, uint8_t * out, const uint8_t * in, size_t size) {
	DECL_VARS_4_ENCRYPT;
	COPY_TO_LOCAL(ctx);
	size_t i = 0;
	for(size_t size2 = FLOORTO(size, MSGBLKSIZE); i < size2; i += MSGBLKSIZE) {
		ENCRYPT(out + i, in + i);
	}
	if(i < size) {
		ENCRYPT_LAST_BLOCK(out + i, in + i, size - i);		
	}
	COPY_FROM_LOCAL(ctx);
	ctx->size_m += size;
}

void rocca_sx2_decrypt(rocca_sx2_context * ctx, uint8_t * out, const uint8_t * in, size_t size) {
	DECL_VARS_4_ENCRYPT;
	COPY_TO_LOCAL(ctx);
	size_t i = 0;
	for(size_t size2 = FLOORTO(size, MSGBLKSIZE); i < size2; i += MSGBLKSIZE) {
		DECRYPT(out + i, in + i);
	}
	if(i < size) {
		DECRYPT_LAST_BLOCK(out + i, in + i, size - i);
	}
	COPY_FROM_LOCAL(ctx);
	ctx->size_m += size;
}

void rocca_sx2_tag(rocca_sx2_context * ctx, uint8_t *tag) {
	DECL_VARS_4_UPDATE;
	COPY_TO_LOCAL(ctx);
	MAKE_TAG(ctx->size_ad, ctx->size_m, tag);
}

