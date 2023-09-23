#ifndef _USER_RAM_H
#define _USER_RAM_H

#define SHOW_LRC

#define ROOT_DIR_PATH 			"0:/"
#define MUSIC_DIR_PATH			"0:/music"//{0x30,0x3a,0x2f,0xc7,0xe5,0xd0,0xc4,0x00}
#define TEST_DIR_PATH       "0:/test"
#define CH_MUSIC_DIR_PATH   "0:/cmusic"
#define EN_MUSIC_DIR_PATH   "0:/emusic"
#define TE_MUSIC_DIR_PATH   "0:/tmusic"
#define MP3_MUSIC_DIR_PATH  "0:/mp3"
#define TEST_COVER_PATH     "0:/testcover"

#define CHOOSE_DIR_PATH MUSIC_DIR_PATH

#define DEFAULT_COVER_JPEG_PATH "0:/SYSTEM/Cover.jpg"

/* File */
#define FILEMGR_LIST_DEPDTH                        100     //default 24
#define FILEMGR_FILE_NAME_SIZE                     100
#define FILEMGR_FULL_PATH_SIZE                     100     //256
#define FILEMGR_MAX_LEVEL                          4    
#define FILETYPE_DIR                               0
#define FILETYPE_FILE                              1


/* Jpeg */
#define NumByte2FindAPIC  2048
#define NumByte2ShowJpeg   358400 // 350KB  // 512000 // 512KB  //1024000 //204800   // 200kB 
//#define NumByte2ShowJpeg  204800 //1024000 //204800   // 200kB 

typedef enum {
	MP3_TYPE = 0,
	WAV_TYPE
}MUSIC_TYPE_TypeDef;

typedef struct _FILELIST_LineTypeDef {
  MUSIC_TYPE_TypeDef type;
  uint8_t name[FILEMGR_FILE_NAME_SIZE];
  uint16_t unicode[FILEMGR_FILE_NAME_SIZE];
}FILELIST_LineTypeDef;

typedef struct _FILELIST_FileTypeDef {
  FILELIST_LineTypeDef  file[FILEMGR_LIST_DEPDTH] ;
  uint8_t              ptr; 
}FILELIST_FileTypeDef;

extern uint16_t NumObs;

extern FILELIST_FileTypeDef FileList;
//extern uint16_t NumObs = 0;

//extern char SDPath[4];
extern uint8_t File_path[FILEMGR_FULL_PATH_SIZE];
extern uint8_t filename[FILEMGR_LIST_DEPDTH][FILEMGR_FULL_PATH_SIZE];

extern uint8_t str[32];
extern uint8_t Unicodebuf[170];
extern uint8_t musicRateTime[20];
extern uint8_t musicFullTime[20];
extern uint8_t musicFilePos[20];
extern uint8_t rtcTime[20];
extern uint8_t rtcDate[20];

extern uint8_t LRC_Ram[4096];

// USB
extern __IO uint8_t PA9_Status; // detect usb connect
extern __IO uint8_t bUSB_Connect;
extern __IO uint8_t bFirstDisplay_USB_Connect;
extern __IO uint8_t bUSB_DisConnect_Enable;



// Display
extern __IO uint8_t bSystemsecond; 
extern __IO uint8_t bMusicNameFlash;
extern __IO uint8_t bPlayerStatus;
extern __IO uint8_t bRTCTimeUpdate;
extern __IO uint8_t bMusicRateUpdate;
extern __IO uint8_t bFirstDisplayMusicName;
extern __IO uint8_t bFirstDisplayMusicTime;
extern __IO uint8_t bFirstDisplayMusicInfo;
extern __IO uint8_t bFirstDisplayRTCTime;

//System
extern uint8_t *GlobalPtr;
extern uint32_t *GlobalPtr32;

// Music Unicode Size
extern __IO uint8_t UnicodeCharSize;


extern void Flash2Ram(unsigned char *src,unsigned char *dsrc);


#endif
