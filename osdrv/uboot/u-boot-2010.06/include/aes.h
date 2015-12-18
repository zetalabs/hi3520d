#ifndef _AES_H
#define _AES_H
#define AES_MIN_KEY_SIZE	16
#define AES_MAX_KEY_SIZE	32

#define AES_BLOCK_SIZE	16

#define E_KEY	(&ctx->buf[0])
#define D_KEY	(&ctx->buf[60])

struct aes_ctx
{
	int key_length;
	u32 buf[120];
};

void gen_tabs (void);
int aes_set_key(struct aes_ctx * ctx, const u8 *in_key, unsigned int key_len);
void aes_encrypt(struct aes_ctx * ctx, u8 *out, const u8 *in);
void aes_decrypt(struct aes_ctx * ctx, u8 *out, const u8 *in);
#endif
