/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32469i_discovery.h"
#include "stm32469i_discovery_sd.h"
#include "stm32469i_discovery_lcd.h"
#include "stm32469i_discovery_qspi.h"
#include "stm32469i_discovery_ts.h"
#include "stm32469i_discovery_audio.h"

#include <string.h>

/* FatFs includes component */
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "jpeglib.h"
#include "decode.h"

/* USB MSC */
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h" 

/* user includes ---------------------------------------------*/
#include "user_display.h"
#include "user_task.h"
#include "user_ram.h"
#include "user_menu.h"
#include "user_mp3player.h"
#include "user_musicPlayer.h"
#include "user_jpeg.h"
#include "user_SpecialFont.h"
#include "user_sdram.h"
#include "user_init.h"
#include "user_lrc.h" 

/* Definition for USARTx clock resources */
#define USARTx                           USART3
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_10
#define USARTx_TX_GPIO_PORT              GPIOB
#define USARTx_TX_AF                     GPIO_AF7_USART3
#define USARTx_RX_PIN                    GPIO_PIN_11
#define USARTx_RX_GPIO_PORT              GPIOB
#define USARTx_RX_AF                     GPIO_AF7_USART3

/* Definition RTC Clok Source*/
#define RTC_CLOCK_SOURCE_LSE

/* Flag */
#define ON        1
#define OFF 			0
#define true 			1
#define false 		0
#define OK 				1
#define ERROR 		0
#define LOW 			0
#define HIGH 			1

/* Defines -------------------------------------------------------------------*/
#define AUDIO_OUT_BUFFER_SIZE                      8192   //0x2000
#if 0
/* File */
#define FILEMGR_LIST_DEPDTH                        40     //default 24
#define FILEMGR_FILE_NAME_SIZE                     40
#define FILEMGR_FULL_PATH_SIZE                     256
#define FILEMGR_MAX_LEVEL                          4    
#define FILETYPE_DIR                               0
#define FILETYPE_FILE                              1
#endif
/* Exported types ------------------------------------------------------------*/
typedef enum {
  APPLICATION_IDLE = 0,  
  APPLICATION_START,   
  APPLICATION_READY,
  APPLICATION_DISCONNECT,
  APPLICATION_USBCONNECT,
}AUDIO_ApplicationTypeDef;
    
/* Audio Demo State Structure */    
typedef enum {
  AUDIO_DEMO_IDLE = 0,
  AUDIO_DEMO_WAIT,  
  AUDIO_DEMO_EXPLORE,
  AUDIO_DEMO_PLAYBACK,
  AUDIO_DEMO_IN,  
}AUDIO_Demo_State;

/* Audio Demo State Machine Structure */
typedef struct _DemoStateMachine {
  __IO AUDIO_Demo_State state;
  __IO uint8_t select;  
}AUDIO_DEMO_StateMachine;

typedef enum {
  AUDIO_STATE_IDLE = 0,
  AUDIO_STATE_WAIT,    
  AUDIO_STATE_INIT,    
  AUDIO_STATE_PLAY,
  AUDIO_STATE_PRERECORD,
  AUDIO_STATE_RECORD,  
  AUDIO_STATE_NEXT,  
  AUDIO_STATE_PREVIOUS,
  AUDIO_STATE_FORWARD,   
  AUDIO_STATE_BACKWARD,
  AUDIO_STATE_STOP,   
  AUDIO_STATE_PAUSE,
  AUDIO_STATE_RESUME,
  AUDIO_STATE_VOLUME_UP,
  AUDIO_STATE_VOLUME_DOWN,
  AUDIO_STATE_ERROR,  
}AUDIO_PLAYBACK_StateTypeDef;

#if 0
typedef enum {
  AUDIO_SELECT_MENU = 0,
  AUDIO_PLAYBACK_CONTROL,  
}AUDIO_DEMO_SelectMode;
#endif

typedef enum {
  BUFFER_OFFSET_NONE = 0,  
  BUFFER_OFFSET_HALF,  
  BUFFER_OFFSET_FULL,     
}BUFFER_StateTypeDef;

/* Audio buffer control struct */
typedef struct {
  uint8_t buff[AUDIO_OUT_BUFFER_SIZE];
  BUFFER_StateTypeDef state;
  uint32_t fptr;
}AUDIO_OUT_BufferTypeDef;

typedef enum {
  BUFFER_EMPTY = 0,  
  BUFFER_FULL,     
}WR_BUFFER_StateTypeDef;

#if 0
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
#endif
#if 0
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
#endif
#if 0
typedef enum {
  AUDIO_ERROR_NONE = 0,  
  AUDIO_ERROR_IO,
  AUDIO_ERROR_EOF,
  AUDIO_ERROR_INVALID_VALUE,     
}AUDIO_ErrorTypeDef;
#endif

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

extern void Toggle_Leds(void);
extern void USART3_UART_Init(void);
extern void Error_Handler(void);
extern void System_Timercnt(void);
extern void update_time(void);


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
