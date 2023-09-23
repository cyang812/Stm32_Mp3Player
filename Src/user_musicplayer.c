#include "main.h"

AUDIO_OUT_BufferTypeDef  BufferCtl;  //static
static int16_t FilePos = 0;
__IO uint32_t uwVolume = 50;        //static
static Point NextPoints[] = {{TOUCH_NEXT_XMIN, TOUCH_NEXT_YMIN},
                             {TOUCH_NEXT_XMAX, (TOUCH_NEXT_YMIN+TOUCH_NEXT_YMAX)/2},
                             {TOUCH_NEXT_XMIN, TOUCH_NEXT_YMAX}};
static Point PreviousPoints[] = {{TOUCH_PREVIOUS_XMIN, (TOUCH_PREVIOUS_YMIN+TOUCH_PREVIOUS_YMAX)/2},
                                 {TOUCH_PREVIOUS_XMAX, TOUCH_PREVIOUS_YMIN},
                                 {TOUCH_PREVIOUS_XMAX, TOUCH_PREVIOUS_YMAX}};

WAVE_FormatTypeDef WaveFormat;
MP3FrameInfo Mp3Format; 
//MP3_FormatTypeDef Mp3Format;  


FIL MusicFile;
extern FILELIST_FileTypeDef FileList;

extern AUDIO_PLAYBACK_StateTypeDef AudioState;
extern AUDIO_ApplicationTypeDef AppliState;

/* Private function prototypes -----------------------------------------------*/
static AUDIO_ErrorTypeDef GetFileInfo(uint8_t file_idx);//, WAVE_FormatTypeDef *info);
static uint8_t PlayerInit(uint32_t AudioFreq);
static void AUDIO_PlaybackDisplayButtons(void);
static void AUDIO_AcquireTouchButtons(void);

/* Private functions ---------------------------------------------------------*/

int16_t Get_MusicFilePos()
{
	return FilePos;
}

/**
  * @brief  Initializes Audio Interface.
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_PLAYER_Init(void)
{
  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, uwVolume, I2S_AUDIOFREQ_48K) == 0)
  {
    return AUDIO_ERROR_NONE;
  }
  else
  {
    return AUDIO_ERROR_IO;
  }

  AppliState = APPLICATION_READY;
}

/**
  * @brief  Starts Audio streaming.    
  * @param  idx: File index
  * @retval Audio error
  */ 
AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(uint8_t idx)
{
  uint32_t bytesread;
	uint8_t ret;
  
  f_close(&MusicFile);
  clear_Lrc_buff();
  if(AUDIO_GetWavObjectNumber() > idx)
  { 
    GetFileInfo(idx);

		// 左下角显示播放器状态
		LCD_DisplayPlayerStatus(PlayOrPause,1);
		LCD_DisplayPlayerStatus(VolUpOrVolDown,uwVolume);
		LCD_DisplayRTCTime();
		//LCD_DisplayMusicInfo(); // 歌曲总时长

    /*Adjust the Audio frequency */
    //PlayerInit(WaveFormat.SampleRate); // comment for test mp3 music file
    
    //BufferCtl.state = BUFFER_OFFSET_NONE; // comment for test mp3 music file
    
    /* Get Data from USB Flash Disk */
    f_lseek(&MusicFile, 0);
    
    /* Fill whole buffer at first time */
		if(FileList.file[idx].type == MP3_TYPE)
		{
			printf("first play a mp3 file \n");
			InitMp3();   // clear two buffer
			Demo_Mp3Parsing(); // update data to buffer
			AudioState = AUDIO_STATE_PLAY;
		  AUDIO_PlaybackDisplayButtons();
		  //BSP_LCD_DisplayStringAt(250, LINE(9), (uint8_t *)"  [PLAY ]", LEFT_MODE); 
		  printf("change mp3 file , next or previous \n");
			//JPEG_Debug();  // show music cover jpeg
			Show_MusicJPEG(idx);
			Lrc_Parse(idx);
			LCD_DisplayMusicInfo(); // 歌曲总时长
		  return AUDIO_ERROR_NONE;
		}
    else if(FileList.file[idx].type == WAV_TYPE)
    {
    	printf("first play a wav file \n");
  		if(f_read(&MusicFile, 
              &BufferCtl.buff[0], 
              AUDIO_OUT_BUFFER_SIZE, 
              (void *)&bytesread) == FR_OK)
	 		{
      	AudioState = AUDIO_STATE_PLAY;
      	AUDIO_PlaybackDisplayButtons();
      	//BSP_LCD_DisplayStringAt(250, LINE(9), (uint8_t *)"  [PLAY ]", LEFT_MODE); 
      	if(bytesread != 0)
      	{
        	printf("bsp_audio_out, bytesread=%#x\n",bytesread);
      		//for(int xx=0;xx<8192;xx++) printf(" buff: 0x%02x ",BufferCtl.buff[xx]); 

			    /*Adjust the Audio frequency */
    			PlayerInit(WaveFormat.SampleRate); // adjust wav type samplerate
					ret = BSP_AUDIO_OUT_Play((uint16_t*)&BufferCtl.buff[0], AUDIO_OUT_BUFFER_SIZE);
        	printf("bsp_out ret: %d\n",ret);
        	BufferCtl.fptr = bytesread;
        	Show_MusicJPEG(0xff); //  music file is wav type file load cover from flash
        	LCD_DisplayMusicInfo(); // 歌曲总时长
        	return AUDIO_ERROR_NONE;
      	}
      }
		  else printf("play wav file read error \n");
    }
  }
  return AUDIO_ERROR_IO;
}

/**
  * @brief  Manages Audio process. 
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_PLAYER_Process(void)
{
  uint32_t bytesread, elapsed_time;
  AUDIO_ErrorTypeDef audio_error = AUDIO_ERROR_NONE;
  static uint32_t prev_elapsed_time = 0xFFFFFFFF;
  uint8_t str[10];  
	mp3_play_status status;
	 
  switch(AudioState)
  {
  case AUDIO_STATE_PLAY:
	  //printf("AudioState:AUDIO_STATE_PLAY\n");
		if (FileList.file[FilePos].type == MP3_TYPE)
		{
			//printf("mp3 file is playing \n");
			status = Mp3_PlayStart();
			LCD_DisplayMusicName(FilePos,UnicodeCharSize); // music name display flash
			LCD_DisplayMusicTimeRate();
			
			if(status == MP3_PLAY_END)    // mp3 播放结束处理
				AudioState = AUDIO_STATE_NEXT;
		}	
		else  // wav type
		{
			LCD_DisplayMusicName(FilePos,UnicodeCharSize); // music name display flash
			LCD_DisplayMusicTimeRate();

	    if(BufferCtl.fptr >= WaveFormat.FileSize)
	    {
	      BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
	      AudioState = AUDIO_STATE_NEXT;
	    }
	    
	    if(BufferCtl.state == BUFFER_OFFSET_HALF)
	    {
	      if(f_read(&MusicFile, 
	                &BufferCtl.buff[0], 
	                AUDIO_OUT_BUFFER_SIZE/2, 
	                (void *)&bytesread) != FR_OK)
	      { 
	        BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW); 
	        return AUDIO_ERROR_IO;       
	      } 
	      BufferCtl.state = BUFFER_OFFSET_NONE;
	      BufferCtl.fptr += bytesread; 
	    }
	    
	    if(BufferCtl.state == BUFFER_OFFSET_FULL)
	    {
	      if(f_read(&MusicFile, 
	                &BufferCtl.buff[AUDIO_OUT_BUFFER_SIZE /2], 
	                AUDIO_OUT_BUFFER_SIZE/2, 
	                (void *)&bytesread) != FR_OK)
	      { 
	        BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW); 
	        return AUDIO_ERROR_IO;       
	      } 
	 
	      BufferCtl.state = BUFFER_OFFSET_NONE;
	      BufferCtl.fptr += bytesread; 
	    }
			#if 0
			/* Display elapsed time */
			elapsed_time = BufferCtl.fptr / WaveFormat.ByteRate; 
			if(prev_elapsed_time != elapsed_time)
			{
				prev_elapsed_time = elapsed_time;
				sprintf((char *)str, "  [%02d:%02d]", (int)(elapsed_time /60), (int)(elapsed_time%60));
				BSP_LCD_SetTextColor(LCD_COLOR_CYAN); 
				BSP_LCD_DisplayStringAt(90, 440, str, LEFT_MODE);
			}
			#endif
	  }

	  //LCD_DisplayRTCTime();
    /* Update audio state machine according to touch acquisition */
    AUDIO_AcquireTouchButtons();
    break;
    
  case AUDIO_STATE_STOP:
  	printf("AudioState:AUDIO_STATE_STOP\n");
  	LCD_ClearZone(0,0,800,60,LCD_COLOR_BLACK);   /* clear music name display */ 
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_FillRect(TOUCH_STOP_XMIN, TOUCH_STOP_YMIN , /* Stop rectangle */
                     TOUCH_STOP_XMAX - TOUCH_STOP_XMIN,
                     TOUCH_STOP_YMAX - TOUCH_STOP_YMIN);
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    AudioState = AUDIO_STATE_IDLE; 
    audio_error = AUDIO_ERROR_IO;
    show_filename(); // printf music file name;
    Scan_MusicFile(); // 停止播放以后再次扫描文件，解决文件名 buff 出错
    show_filename(); // printf music file name;
    break;
    
  case AUDIO_STATE_NEXT:
	  printf("AudioState:AUDIO_STATE_NEXT\n");
    if(++FilePos >= AUDIO_GetWavObjectNumber())
    {
      FilePos = 0; 
    }
    if(FilePos ==1)FilePos=2;  //解决第二首跳歌
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    AUDIO_PLAYER_Start(FilePos);
    break;    
    
  case AUDIO_STATE_PREVIOUS:
	  printf("AudioState:AUDIO_STATE_PREVIOUS\n");
    if(--FilePos < 0)
    {
      FilePos = AUDIO_GetWavObjectNumber() - 1; 
    }
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    AUDIO_PLAYER_Start(FilePos);
    break;   
    
  case AUDIO_STATE_PAUSE:
	  printf("AudioState:AUDIO_STATE_PAUSE\n");
//    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
//    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
//    BSP_LCD_DisplayStringAt(90, 420, (uint8_t *)"  [PAUSE]", LEFT_MODE);
		LCD_DisplayPlayerStatus(PlayOrPause,2);  // 2 代表pause
    
    BSP_LCD_SetTextColor(LCD_COLOR_RED);    /* Display red pause rectangles */
    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN , 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 40, TOUCH_PAUSE_YMIN, 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
    BSP_AUDIO_OUT_Pause();
    AudioState = AUDIO_STATE_WAIT;
    break;
    
  case AUDIO_STATE_RESUME:
	  printf("AudioState:AUDIO_STATE_RESUME\n");
//    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
//    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
//    BSP_LCD_DisplayStringAt(90, 420, (uint8_t *)"  [PLAY ]", LEFT_MODE);
    LCD_DisplayPlayerStatus(PlayOrPause,1);  // 1 代表play

    /* Display blue cyan pause rectangles */
    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN , 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 40, TOUCH_PAUSE_YMIN, 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
    BSP_AUDIO_OUT_Resume();
    AudioState = AUDIO_STATE_PLAY;
    break;
    
  case AUDIO_STATE_VOLUME_UP: 
    if( uwVolume <= 90)
    {
      uwVolume += 10;
    }
    BSP_AUDIO_OUT_SetVolume(uwVolume);

//    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
//    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
//    sprintf((char *)str,  "Volume : %lu ", uwVolume);
//    BSP_LCD_DisplayStringAt(90,400,str,LEFT_MODE);

		LCD_DisplayPlayerStatus(VolUpOrVolDown,uwVolume);

    AudioState = AUDIO_STATE_PLAY;
    break;
    
  case AUDIO_STATE_VOLUME_DOWN:    
    if( uwVolume >= 10)
    {
      uwVolume -= 10;
    }
    BSP_AUDIO_OUT_SetVolume(uwVolume);
//    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
//    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
//    sprintf((char *)str,  "Volume : %lu ", uwVolume);
//    BSP_LCD_DisplayStringAt(90,400,str,LEFT_MODE);

		LCD_DisplayPlayerStatus(VolUpOrVolDown,uwVolume);

    AudioState = AUDIO_STATE_PLAY;
    break;
    
  case AUDIO_STATE_WAIT:
  case AUDIO_STATE_IDLE:
  case AUDIO_STATE_INIT:    
  default:
    /* Update audio state machine according to touch acquisition */
    AUDIO_AcquireTouchButtons();
    break;
  }
  return audio_error;
}

/**
  * @brief  Stops Audio streaming.
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_PLAYER_Stop(void)
{
  AudioState = AUDIO_STATE_STOP;
  FilePos = 0;
  
  BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
  f_close(&MusicFile);
  return AUDIO_ERROR_NONE;
}

extern uint8_t waitflag;
extern uint8_t buffer_switch;
/**
  * @brief  Calculates the remaining file size and new position of the pointer.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	//printf("change buff 1\n");
  if(AudioState == AUDIO_STATE_PLAY)
  {
    BufferCtl.state = BUFFER_OFFSET_FULL;
    waitflag = 1;
    buffer_switch = 1;
  }
	//Decbuf=(short *)buffer2;
}

/**
  * @brief  Manages the DMA Half Transfer complete interrupt.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{ 
	//printf("change buff 2\n");
  if(AudioState == AUDIO_STATE_PLAY)
  {
    BufferCtl.state = BUFFER_OFFSET_HALF;

    waitflag = 1;
    buffer_switch = 2;
  }
	//Decbuf=(short *)buffer1;
}


void LCD_DisplayButtonsDebug()
{
	AUDIO_PlaybackDisplayButtons();
}

/*******************************************************************************
                            Static Functions
*******************************************************************************/
/**
  * @brief  Gets the file info.
  * @param  file_idx: File index
  * @param  info: Pointer to WAV file info
  * @retval Audio error
  */
static AUDIO_ErrorTypeDef GetFileInfo(uint8_t file_idx)//, WAVE_FormatTypeDef *info)
{
  uint32_t bytesread;
  uint32_t duration;
  uint8_t str[FILEMGR_FULL_PATH_SIZE];  
  uint8_t ret;
  
  printf("get file ifno\n");

	strcpy((char *)File_path,(char *)"0:/music/");  //change file path
	//strcat((char *)File_path,(char *)'/');
	strcat((char *)File_path,(const TCHAR *)FileList.file[file_idx].name);

	printf("File_path :%s \n",File_path);
  
  ret = f_open(&MusicFile, (const char *)File_path, FA_OPEN_EXISTING | FA_READ);
  printf("open file ret :%d\n",ret);
  
  if(ret == FR_OK)
  //if(f_open(&WavFile, (char *)FileList.file[file_idx].name, FA_OPEN_EXISTING | FA_READ) == FR_OK) 
  {
  	printf("file type is %d\n",FileList.file[file_idx].type);
    if(FileList.file[file_idx].type == MP3_TYPE)
    {
    	printf("file is mp3 type \n");
			ret = f_read(&MusicFile, &Mp3Format, sizeof(Mp3Format), (void *)&bytesread);
    }
    else if(FileList.file[file_idx].type == WAV_TYPE)
    {
    	printf("file is wav type \n");
			ret = f_read(&MusicFile, &WaveFormat, sizeof(WaveFormat), (void *)&bytesread);
			duration = WaveFormat.FileSize / WaveFormat.ByteRate; // wav 歌曲总时间
			//sprintf((char *)musicFullTime, "File Size : %d KB [%02d:%02d]", (int)(WaveFormat.FileSize/1024), (int)(duration/60), (int)(duration%60));
			sprintf((char *)musicFullTime,"/%02d:%02d",(int)(duration/60), (int)(duration%60));
    }
    /* Fill the buffer to Send */
    //if(f_read(&WavFile, info, sizeof(WaveFormat), (void *)&bytesread) == FR_OK)
    if(ret == FR_OK)
    {
    //	BSP_LCD_SetFont(&Font20);
    // BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
      //GBK2Unicode(file_idx);//for chinese filename display
//      sprintf((char *)str, "Playing file (%d/%d): %s", 
//              file_idx + 1, FileList.ptr,
//              (char *)FileList.file[file_idx].unicode);
			UnicodeCharSize = GBK2Unicode(file_idx);   // get name unicode
			bFirstDisplayMusicName = true;
			bFirstDisplayMusicTime = true;
			bFirstDisplayRTCTime = true;			
			
			//BSP_LCD_SetFont(&Font20); 
			//LCD_DisplayStringAt(0,10,FileList.file[file_idx].unicode,CENTER_MODE);// for display file name	
			//LCD_DisplayMusicName(file_idx,UnicodeCharSize);//for chinese filename display
//      sprintf((char *)str, "Playing file (%d/%d): %s", 
//              file_idx + 1, FileList.ptr,
//              (char *)FileList.file[file_idx].name);
      //BSP_LCD_ClearStringLine(4);
      //BSP_LCD_DisplayStringAtLine(4, str);
      
			#if 0
      BSP_LCD_SetTextColor(LCD_COLOR_CYAN); 
      sprintf((char *)str,  "Sample rate : %d Hz", (int)(info->SampleRate));
      BSP_LCD_ClearStringLine(6);
      BSP_LCD_DisplayStringAtLine(6, str);
      
      sprintf((char *)str,  "Channels number : %d", info->NbrChannels);
      BSP_LCD_ClearStringLine(7);      
      BSP_LCD_DisplayStringAtLine(7, str);
      
      duration = info->FileSize / info->ByteRate; 
      sprintf((char *)str, "File Size : %d KB [%02d:%02d]", (int)(info->FileSize/1024), (int)(duration/60), (int)(duration%60));
      BSP_LCD_ClearStringLine(8);
      BSP_LCD_DisplayStringAtLine(8, str);
      BSP_LCD_DisplayStringAt(263, LINE(8), (uint8_t *)"[00:00]", LEFT_MODE);
 
      BSP_LCD_SetTextColor(LCD_COLOR_WHITE); 
      sprintf((char *)str,  "Volume : %lu", uwVolume);
      BSP_LCD_ClearStringLine(9);      
      BSP_LCD_DisplayStringAtLine(9, str);
			#endif
      return AUDIO_ERROR_NONE;
    }
    f_close(&MusicFile);
  }
  return AUDIO_ERROR_IO;
}
#if 0
static void printf_musicInfo(MUSIC_TYPE_TypeDef music_type)
{
	if(music_type == MP3_TYPE)
	{
		printf("mp3====> %#x",Mp3Format.bitrate);
	}
	else if( music_type == WAV_TYPE)
	{
		printf("wav====> %#x",WaveFormat.FileFormat);		
	} 
}
#endif
/**
  * @brief  Initializes the Wave player.
  * @param  AudioFreq: Audio sampling frequency
  * @retval None
  */
static uint8_t PlayerInit(uint32_t AudioFreq)
{ 
  /* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */ 
  printf("audio freq : %d \n",AudioFreq);
  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, uwVolume, AudioFreq) != 0)
  {
    return 1;
  }
  else
  {
    BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
    return 0;
  } 
}

/**
  * @brief  Display interface touch screen buttons
  * @param  None
  * @retval None
  */
static void AUDIO_PlaybackDisplayButtons(void)
{
  BSP_LCD_SetFont(&Font20);
//  BSP_LCD_ClearStringLine(14);            /* Clear dedicated zone */
//  BSP_LCD_ClearStringLine(15);           
//  BSP_LCD_ClearStringLine(16);
//  BSP_LCD_ClearStringLine(17);
//  BSP_LCD_ClearStringLine(18);
//  BSP_LCD_ClearStringLine(19);
//  BSP_LCD_ClearStringLine(20);

  BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
  
  BSP_LCD_FillPolygon(PreviousPoints, 3);   /* Previous track icon */
  BSP_LCD_FillRect(TOUCH_PREVIOUS_XMIN, TOUCH_PREVIOUS_YMIN , 10, TOUCH_PREVIOUS_YMAX - TOUCH_PREVIOUS_YMIN);

  BSP_LCD_FillPolygon(NextPoints, 3);       /* Next track icon */
  BSP_LCD_FillRect(TOUCH_NEXT_XMAX-9, TOUCH_NEXT_YMIN , 10, TOUCH_NEXT_YMAX - TOUCH_NEXT_YMIN);

  BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN , 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);    /* Pause rectangles */
  BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 40, TOUCH_PAUSE_YMIN, 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);

  BSP_LCD_FillRect(TOUCH_STOP_XMIN, TOUCH_STOP_YMIN , /* Stop rectangle */
                   TOUCH_STOP_XMAX - TOUCH_STOP_XMIN,
                   TOUCH_STOP_YMAX - TOUCH_STOP_YMIN);

  BSP_LCD_DrawRect(TOUCH_VOL_MINUS_XMIN, TOUCH_VOL_MINUS_YMIN , /* VOl- rectangle */
                   TOUCH_VOL_MINUS_XMAX - TOUCH_VOL_MINUS_XMIN,
                   TOUCH_VOL_MINUS_YMAX - TOUCH_VOL_MINUS_YMIN);
  //BSP_LCD_DisplayStringAt(STR_VOL_MINUS_X,STR_VOL_MINUS_Y, (uint8_t *)"VOL-", LEFT_MODE);
  LCD_DisplayStringAt(STR_VOL_MINUS_X,STR_VOL_MINUS_Y, (uint16_t *)"-", LEFT_MODE);

  BSP_LCD_DrawRect(TOUCH_VOL_PLUS_XMIN, TOUCH_VOL_PLUS_YMIN , /* VOl+ rectangle */
                   TOUCH_VOL_PLUS_XMAX - TOUCH_VOL_PLUS_XMIN,
                   TOUCH_VOL_PLUS_YMAX - TOUCH_VOL_PLUS_YMIN);
  //BSP_LCD_DisplayStringAt(STR_VOL_PLUS_X, STR_VOL_PLUS_Y, (uint8_t *)"VOL+", LEFT_MODE);
  LCD_DisplayStringAt(STR_VOL_PLUS_X, STR_VOL_PLUS_Y, (uint16_t *)"+", LEFT_MODE);  

  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
  //BSP_LCD_ClearStringLine(21);//cyang modify *add
  //BSP_LCD_ClearStringLine(22);
  //BSP_LCD_DisplayStringAtLine(22, (uint8_t *)"Use stop button to exit");
  //BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
  BSP_LCD_SetFont(&Font20);
}

/**
  * @brief  Test touch screen state and modify audio state machine according to that
  * @param  None
  * @retval None
  */
static void AUDIO_AcquireTouchButtons(void)
{
  static TS_StateTypeDef  TS_State={0};

  if(TS_State.touchDetected == 1)   /* If previous touch has not been released, we don't proceed any touch command */
  {
    BSP_TS_GetState(&TS_State);
  }
  else
  {
    BSP_TS_GetState(&TS_State);
    if(TS_State.touchDetected == 1)
    {
      if ((TS_State.touchX[0] > TOUCH_PAUSE_XMIN) && (TS_State.touchX[0] < TOUCH_PAUSE_XMAX) &&
          (TS_State.touchY[0] > TOUCH_PAUSE_YMIN) && (TS_State.touchY[0] < TOUCH_PAUSE_YMAX))
      {
        if (AudioState == AUDIO_STATE_PLAY)
        {
          AudioState = AUDIO_STATE_PAUSE;
        }
        else
        {
          AudioState = AUDIO_STATE_RESUME;
        }
      }
      else if ((TS_State.touchX[0] > TOUCH_NEXT_XMIN) && (TS_State.touchX[0] < TOUCH_NEXT_XMAX) &&
               (TS_State.touchY[0] > TOUCH_NEXT_YMIN) && (TS_State.touchY[0] < TOUCH_NEXT_YMAX))
      {
        AudioState = AUDIO_STATE_NEXT;
      }
      else if ((TS_State.touchX[0] > TOUCH_PREVIOUS_XMIN) && (TS_State.touchX[0] < TOUCH_PREVIOUS_XMAX) &&
               (TS_State.touchY[0] > TOUCH_PREVIOUS_YMIN) && (TS_State.touchY[0] < TOUCH_PREVIOUS_YMAX))
      {
        AudioState = AUDIO_STATE_PREVIOUS;
      }
      else if ((TS_State.touchX[0] > TOUCH_STOP_XMIN) && (TS_State.touchX[0] < TOUCH_STOP_XMAX) &&
               (TS_State.touchY[0] > TOUCH_STOP_YMIN) && (TS_State.touchY[0] < TOUCH_STOP_YMAX))
      {
        AudioState = AUDIO_STATE_STOP;
      }
      else if((TS_State.touchX[0] > TOUCH_VOL_MINUS_XMIN) && (TS_State.touchX[0] < TOUCH_VOL_MINUS_XMAX) &&
              (TS_State.touchY[0] > TOUCH_VOL_MINUS_YMIN) && (TS_State.touchY[0] < TOUCH_VOL_MINUS_YMAX))
      {
        AudioState = AUDIO_STATE_VOLUME_DOWN;
      }
      else if((TS_State.touchX[0] > TOUCH_VOL_PLUS_XMIN) && (TS_State.touchX[0] < TOUCH_VOL_PLUS_XMAX) &&
              (TS_State.touchY[0] > TOUCH_VOL_PLUS_YMIN) && (TS_State.touchY[0] < TOUCH_VOL_PLUS_YMAX))
      {
        AudioState = AUDIO_STATE_VOLUME_UP;
      }
    }
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
