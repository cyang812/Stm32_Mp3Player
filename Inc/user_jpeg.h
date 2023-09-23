#ifndef _USER_JPEG_H
#define _USER_JPEG_H

typedef struct RGB
{
  uint8_t B;
  uint8_t G;
  uint8_t R;
}RGB_typedef;

typedef enum _Jpeg_Tag_Type
{
	IMAGE_JPEG = 0,
	IMAGE_JPG,
}Jpeg_Tag_Type;

typedef struct _Cover_Jpeg
{
	uint32_t Tag_Size;
	uint32_t Jpeg_Size;
	Jpeg_Tag_Type Tag_Type;
}Cover_Jpeg;

typedef enum _Jpeg_Error
{
	JPEG_NO_ERROR = 0,
	NOT_FIND_APIC, 
	JPEG_TOO_BIG,
	JPEG_PNG_TYPE,
	JPEG_UNKNOW_TYPE,
}Jpeg_Error;

typedef enum _Jpeg_Data_From
{
	JPEG_From_Buff = 0,
	JPEG_From_SD,
	JPEG_From_FLASH,
}Jpeg_Data_From;


extern FIL PicFile;     /* Pic File object */

void show_jepg(FIL *file,uint32_t width, uint8_t * buff, uint8_t (*callback)(uint8_t*, uint32_t),uint8_t datafrom);
static uint8_t Jpeg_CallbackFunction(uint8_t* Row, uint32_t DataLength);

extern void ReadStrUnit(uint8_t *src,uint8_t *desc, uint16_t idx, uint8_t NbrOfBytes);
extern Jpeg_Error Get_MusicCoverJpeg(uint8_t idx);
extern void Show_MusicJPEG(uint8_t idx);

extern void JPEG_Debug(void);

//#define MAKE_DWORD(b3, b2, b1, b0) (((uint32_t)((uint8_t)(b3) << 24)) | ((uint8_t)(b2) << 16) | ((uint8_t)(b1) << 8) | ((uint8_t)(b0)))
//#define MAKE_WORD(h, l)  (((uint32_t)(h) << 16) | ((l) & 0xffff))

#endif
