#ifndef ROCCA_SX2_H
#define ROCCA_SX2_H

#include <stdlib.h>
#include <stdint.h>

#define ROCCA_SX2_KEY_SIZE       (32)
#define ROCCA_SX2_IV_SIZE        (16)
#define ROCCA_SX2_MSG_BLOCK_SIZE (32*2)
#define ROCCA_SX2_TAG_SIZE       (32)
#define ROCCA_SX2_STATE_NUM      ( 7)

typedef struct ROCCA_SX2_CTX {
	uint8_t state[ROCCA_SX2_STATE_NUM][16*2];
	size_t size_ad;
	size_t size_m;
} rocca_sx2_context;

void rocca_sx2_init(rocca_sx2_context * ctx, const uint8_t * key, const uint8_t * iv);
void rocca_sx2_add_ad(rocca_sx2_context * ctx, const uint8_t * in, size_t size);
void rocca_sx2_encrypt(rocca_sx2_context * ctx, uint8_t * out, const uint8_t * in, size_t size);
void rocca_sx2_decrypt(rocca_sx2_context * ctx, uint8_t * out, const uint8_t * in, size_t size);
void rocca_sx2_tag(rocca_sx2_context * ctx, uint8_t *tag);

#endif
