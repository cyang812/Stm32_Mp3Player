#include "main.h"

FILELIST_FileTypeDef FileList;
uint16_t NumObs = 0;

uint8_t File_path[FILEMGR_FULL_PATH_SIZE];
uint8_t filename[FILEMGR_LIST_DEPDTH][FILEMGR_FULL_PATH_SIZE];

uint8_t str[32];
uint8_t Unicodebuf[170];
uint8_t musicRateTime[20];
uint8_t musicFullTime[20];
uint8_t musicFilePos[20];
uint8_t rtcTime[20];
uint8_t rtcDate[20];

uint8_t LRC_Ram[4096];

// 外部sram
#define SDRAM_BANK_ADDR   ((uint32_t)0xC0000000)
#define SDRAM_JPEG_SIZE   NumByte2ShowJpeg //1024000 // 1024KB //204800 // 200KB
#define SDRAM_JPEG_WRITE_ADDR  SDRAM_DEVICE_SIZE/2 //((uint32_t)0x400000)
unsigned char Jpeg_Cover_Ram[SDRAM_JPEG_SIZE] __attribute__((at(SDRAM_BANK_ADDR + SDRAM_JPEG_WRITE_ADDR)))={0};

//unsigned char Jpeg_Cover_Ram[NumByte2ShowJpeg]={0};  //内部sram


// System
uint8_t *GlobalPtr;
uint32_t *GlobalPtr32;

// USB
__IO uint8_t PA9_Status; // detect usb connect
__IO uint8_t bUSB_Connect;
__IO uint8_t bFirstDisplay_USB_Connect;
__IO uint8_t bUSB_DisConnect_Enable = false;


// Display
__IO uint8_t bSystemsecond;
__IO uint8_t bMusicNameFlash;
__IO uint8_t bRTCTimeUpdate;
__IO uint8_t bPlayerStatus;
__IO uint8_t bMusicRateUpdate;
__IO uint8_t bFirstDisplayMusicName;
__IO uint8_t bFirstDisplayMusicTime;
__IO uint8_t bFirstDisplayMusicInfo;
__IO uint8_t bFirstDisplayRTCTime;


// Music Unicode Size
__IO uint8_t UnicodeCharSize;

void Flash2Ram(unsigned char *src,unsigned char *dsrc)
{
	memcpy(dsrc,src,5764);
	printf("Jpeg_Cover_Ram[0]:%d\n",Jpeg_Cover_Ram[0]);
	printf("Jpeg_Cover_Ram[1]:%d\n",Jpeg_Cover_Ram[1]);
	printf("Jpeg_Cover_Ram[5762]:%d\n",Jpeg_Cover_Ram[5762]);
	printf("Jpeg_Cover_Ram[5763]:%d\n",Jpeg_Cover_Ram[5763]);
}
