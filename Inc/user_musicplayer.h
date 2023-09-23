#ifndef __USER_MUSICPLAYER_H
#define __USER_MUSICPLAYER_H

/* Touch */
#define TOUCH_NEXT_XMIN         650//570//500+70
#define TOUCH_NEXT_XMAX         710//630//560+70
#define TOUCH_NEXT_YMIN         100//350
#define TOUCH_NEXT_YMAX         160//410

#define TOUCH_PREVIOUS_XMIN     90//450//380+70
#define TOUCH_PREVIOUS_XMAX     150//510//440+70
#define TOUCH_PREVIOUS_YMIN     100//350
#define TOUCH_PREVIOUS_YMAX     160//410

#define TOUCH_STOP_XMIN         650//330//260+70
#define TOUCH_STOP_XMAX         710//390//320+70
#define TOUCH_STOP_YMIN         200//350
#define TOUCH_STOP_YMAX         260//410

#define TOUCH_PAUSE_XMIN        90//210//140+70
#define TOUCH_PAUSE_XMAX        150//270//200+70
#define TOUCH_PAUSE_YMIN        200//350
#define TOUCH_PAUSE_YMAX        260//410

#define TOUCH_VOL_MINUS_XMIN    90//20+70
#define TOUCH_VOL_MINUS_XMAX    150//80+70
#define TOUCH_VOL_MINUS_YMIN    300//350
#define TOUCH_VOL_MINUS_YMAX    360//410

#define TOUCH_VOL_PLUS_XMIN     650//690//620+70
#define TOUCH_VOL_PLUS_XMAX     710//750//680+70
#define TOUCH_VOL_PLUS_YMIN     300//350
#define TOUCH_VOL_PLUS_YMAX     360//410

#define STR_VOL_MINUS_X     TOUCH_VOL_MINUS_XMIN    
#define STR_VOL_MINUS_Y     TOUCH_VOL_MINUS_YMIN
#define STR_VOL_PLUS_X      TOUCH_VOL_PLUS_XMIN
#define STR_VOL_PLUS_Y      TOUCH_VOL_PLUS_YMIN

typedef struct {
  uint32_t ChunkID;       /* 0 */ 
  uint32_t FileSize;      /* 4 */
  uint32_t FileFormat;    /* 8 */
  uint32_t SubChunk1ID;   /* 12 */
  uint32_t SubChunk1Size; /* 16*/  
  uint16_t AudioFormat;   /* 20 */ 
  uint16_t NbrChannels;   /* 22 */   
  uint32_t SampleRate;    /* 24 */
  
  uint32_t ByteRate;      /* 28 */
  uint16_t BlockAlign;    /* 32 */  
  uint16_t BitPerSample;  /* 34 */  
  uint32_t SubChunk2ID;   /* 36 */   
  uint32_t SubChunk2Size; /* 40 */    
}WAVE_FormatTypeDef;

#if 1
typedef enum {
  AUDIO_ERROR_NONE = 0,  
  AUDIO_ERROR_IO,
  AUDIO_ERROR_EOF,
  AUDIO_ERROR_INVALID_VALUE,     
}AUDIO_ErrorTypeDef;
#endif

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

extern FIL MusicFile;

extern int16_t Get_MusicFilePos();

extern void LCD_DisplayButtonsDebug(void);

AUDIO_ErrorTypeDef AUDIO_PLAYER_Init(void);
AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(uint8_t idx);
AUDIO_ErrorTypeDef AUDIO_PLAYER_Process(void);
AUDIO_ErrorTypeDef AUDIO_PLAYER_Stop(void);

extern void LCD_DisplayButtons(void);
extern void LCD_DisplayPlay(void);

#endif /* __WAVEPLAYER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
