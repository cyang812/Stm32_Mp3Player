#ifndef  _USER_LRC_H
#define _USER_LRC_H 

#define MAXUTF8 60
#define MAXUNICODE 60
//#define MAXLINE 20  // 允许有20字
#define MAXBUF 100 //  允许100行

#define DWBYTE(b3, b2, b1, b0) (((uint32_t)((uint8_t)(b3) << 24)) | ((uint8_t)(b2) << 16) | ((uint8_t)(b1) << 8) | ((uint8_t)(b0)))
//#define MAKE_WORD(h, l)  (((uint32_t)(h) << 8) | ((l) & 0xffff))

typedef struct
{
	uint8_t str_utf8[MAXUTF8];
	uint16_t str_unicode[MAXUNICODE];
}Lrc_sub_struct;

typedef struct
{
	uint16_t str_time[MAXBUF];
	Lrc_sub_struct lrc_sub_str[MAXBUF];
}LRC_struct;

int enc_utf8_to_unicode_one(const uint8_t* pInput, uint16_t *Unic);

void Lrc_Parse(uint8_t idx);
extern void clear_Lrc_buff();

extern LRC_struct lrc;
#endif
