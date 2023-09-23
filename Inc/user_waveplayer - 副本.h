/**
  ******************************************************************************
  * @file    Audio/Audio_playback_and_record/Inc/waveplayer.h
  * @author  MCD Application Team
  * @version V1.0.6
  * @date    04-November-2016
  * @brief   Header for waveplayer.c module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright ?2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_WAVEPLAYER_H
#define __USER_WAVEPLAYER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
#define TOUCH_NEXT_XMIN         570//500+70
#define TOUCH_NEXT_XMAX         630//560+70
#define TOUCH_NEXT_YMIN         350
#define TOUCH_NEXT_YMAX         410

#define TOUCH_PREVIOUS_XMIN     450//380+70
#define TOUCH_PREVIOUS_XMAX     510//440+70
#define TOUCH_PREVIOUS_YMIN     350
#define TOUCH_PREVIOUS_YMAX     410

#define TOUCH_STOP_XMIN         330//260+70
#define TOUCH_STOP_XMAX         390//320+70
#define TOUCH_STOP_YMIN         350
#define TOUCH_STOP_YMAX         410

#define TOUCH_PAUSE_XMIN        210//140+70
#define TOUCH_PAUSE_XMAX        270//200+70
#define TOUCH_PAUSE_YMIN        350
#define TOUCH_PAUSE_YMAX        410

#define TOUCH_VOL_MINUS_XMIN    90//20+70
#define TOUCH_VOL_MINUS_XMAX    150//80+70
#define TOUCH_VOL_MINUS_YMIN    350
#define TOUCH_VOL_MINUS_YMAX    410

#define TOUCH_VOL_PLUS_XMIN     690//620+70
#define TOUCH_VOL_PLUS_XMAX     750//680+70
#define TOUCH_VOL_PLUS_YMIN     350
#define TOUCH_VOL_PLUS_YMAX     410

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

AUDIO_ErrorTypeDef AUDIO_PLAYER_Init(void);
AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(uint8_t idx);
AUDIO_ErrorTypeDef AUDIO_PLAYER_Process(void);
AUDIO_ErrorTypeDef AUDIO_PLAYER_Stop(void);

extern void LCD_DisplayButtons(void);
extern void LCD_DisplayPlay(void);

#endif /* __WAVEPLAYER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
