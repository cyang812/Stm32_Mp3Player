#ifndef _USER_MP3PLAYER
#define _USER_MP3PLAYER

#include <stdint.h>
#include "mp3dec.h"
#include "mp3common.h"

#define READBUF_SIZE 4000 

#define		MAX_ID3V1_TAG_LEN	32		//32 bytes,the lastest two bytes are 0x00 always
#define		MAX_ID3V2_TAG_LEN	32		//32 bytes,the lastest two bytes are 0x00 always

typedef struct _ID3V1									// ID3v1 structure, refer to ID3v1 spec
{
	uint8_t		Tag[3];
	uint8_t		Genre;
	uint8_t		Title[MAX_ID3V1_TAG_LEN];
	uint8_t 	Artist[MAX_ID3V1_TAG_LEN];
	uint8_t		Album[MAX_ID3V1_TAG_LEN];
	uint8_t		Comment[MAX_ID3V1_TAG_LEN];
	uint32_t	Year;	
} ID3V1;


typedef struct _ID3V2_HEADER			// ID3v2 structure, refer to ID3v2 spec 标签头
{
	uint8_t		 Header[3];   // ID3
	uint8_t    Ver;         // for ID3V2.3 is 0x03
	uint8_t    Resivion;    
	uint8_t    Flag;
	uint8_t    Size[4];     //
} ID3V2_HEADER;

typedef struct _ID3V2_FRAME   // 标签帧
{
	uint8_t   			FrameID[4];  // 类似于 APIC
	unsigned long  	Size;
	uint8_t   			Flags[2];
} ID3V2_FRAME;

typedef struct _ID3V2									// ID3v2 structure for system
{
	uint8_t	Title[MAX_ID3V2_TAG_LEN];
	uint8_t	Artist[MAX_ID3V2_TAG_LEN];
	uint8_t	Album[MAX_ID3V2_TAG_LEN];
} ID3V2;

#if 0
typedef struct _MP3_FormatTypeDef   
{
	

}MP3_FormatTypeDef;
#endif


#define BUFF_SIZE 2304

typedef struct _MP3_BUFF
{
	uint16_t buffer1[BUFF_SIZE];
	uint16_t buffer2[BUFF_SIZE];
	//uint16_t twobuff[BUFF_SIZE*2];
}MP3_BUFF;

typedef enum _MP3_PLAY_STATUS
{
	MP3_PLAY_PLAYING =0,
	MP3_PLAY_END,
}mp3_play_status;

void Mp3_PlayInit(void);
mp3_play_status Mp3_PlayStart(void);
uint8_t Get_Mp3file_Info(void);
uint8_t Demo_Mp3Parsing(void);

extern void MP3_Debug(void);

#endif
