#ifndef ROCCA_SX4_H
#define ROCCA_SX4_H

#include <stdlib.h>
#include <stdint.h>

#define ROCCA_SX4_KEY_SIZE       (32)
#define ROCCA_SX4_IV_SIZE        (16)
#define ROCCA_SX4_MSG_BLOCK_SIZE (32*4)
#define ROCCA_SX4_TAG_SIZE       (32)
#define ROCCA_SX4_STATE_NUM      ( 7)

typedef struct ROCCA_SX4_CTX {
	uint8_t state[ROCCA_SX4_STATE_NUM][16*4];
	size_t size_ad;
	size_t size_m;
} rocca_sx4_context;

void rocca_sx4_init(rocca_sx4_context * ctx, const uint8_t * key, const uint8_t * iv);
void rocca_sx4_add_ad(rocca_sx4_context * ctx, const uint8_t * in, size_t size);
void rocca_sx4_encrypt(rocca_sx4_context * ctx, uint8_t * out, const uint8_t * in, size_t size);
void rocca_sx4_decrypt(rocca_sx4_context * ctx, uint8_t * out, const uint8_t * in, size_t size);
void rocca_sx4_tag(rocca_sx4_context * ctx, uint8_t *tag);

#endif
