#include "main.h"

extern AUDIO_ApplicationTypeDef AppliState;

AUDIO_DEMO_StateMachine     AudioDemo;
AUDIO_PLAYBACK_StateTypeDef AudioState;

void Menu_prase()
{
	AUDIO_ErrorTypeDef	status;
	TS_StateTypeDef  TS_State;
	//printf("Menu Prase\n");
	
  //for test
	//AudioDemo.state = AUDIO_DEMO_PLAYBACK;

	if(PA9_Status == HIGH)
		AppliState = APPLICATION_USBCONNECT;
	else if(PA9_Status == LOW)
	{
		AppliState = APPLICATION_READY;
		if (bUSB_DisConnect_Enable) // 断开 USB 后更新数据
		{
			printf("usb disconnect , update data\n");
			BSP_LCD_Clear(LCD_COLOR_BLACK);
			Scan_MusicFile();
			LCD_DisplayPlayButton(ON);
			AudioDemo.state = AUDIO_DEMO_WAIT;
			bUSB_DisConnect_Enable = false;
		}
	}
	
  if(AppliState == APPLICATION_READY)
  { 
	  //printf("app ready \n");
    switch(AudioDemo.state)
    {
    case AUDIO_DEMO_IDLE:
			printf("audio demo idle\n");
      AudioDemo.state = AUDIO_DEMO_WAIT;
      break;    
      
    case AUDIO_DEMO_WAIT:
			//printf("audio demo wait\n");

      BSP_TS_GetState(&TS_State);
      if(TS_State.touchDetected == 1)
      {
        if ((TS_State.touchX[0] > TOUCH_PLAYBACK_XMIN-20) && (TS_State.touchX[0] < TOUCH_PLAYBACK_XMAX+20) &&
                 (TS_State.touchY[0] > TOUCH_PLAYBACK_YMIN-20) && (TS_State.touchY[0] < TOUCH_PLAYBACK_YMAX+20))
        {
          AudioDemo.state = AUDIO_DEMO_PLAYBACK;
          //LCD_DisplayPlayButton(OFF);
        }
        else
        {
          AudioDemo.state = AUDIO_DEMO_EXPLORE;
        }
        /* Wait for touch released */
        do
        {
        	//printf("do while error");
          BSP_TS_GetState(&TS_State);
        }while(TS_State.touchDetected > 0);
      }
      break;
      
    case AUDIO_DEMO_EXPLORE: 
			printf("audio demo explore\n");
			
      if(AppliState == APPLICATION_READY)
      {
        if(AUDIO_ShowWavFiles() > 0)
        {
          printf("There is no WAV file on the USB Key.\n");         
          AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU); 
          AudioDemo.state = AUDIO_DEMO_IDLE;
          printf("11111");
        }
        else
        {
        	printf("22222");
          AudioDemo.state = AUDIO_DEMO_WAIT;
        }
      }
      else
      {
        AudioDemo.state = AUDIO_DEMO_WAIT;
      }
      break;
      
    case AUDIO_DEMO_PLAYBACK:
			//printf("audio demo playback\n");
			LCD_DisplayRTCTime();
      if(AppliState == APPLICATION_READY)
      {
        if(AudioState == AUDIO_STATE_IDLE)
        {
          if(AUDIO_ShowWavFiles() > 0)
          {
            printf("There is no WAV file on the USB Key.\n");         
            AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU); 
            AudioDemo.state = AUDIO_DEMO_IDLE;
          }
          else
          {
            /* Start Playing */
            AudioState = AUDIO_STATE_INIT;
          }

          /* Clear the LCD */
          //LCD_ClearTextZone();
          //LCD_DisplayButtons();
			
          if(AUDIO_PLAYER_Start(0) == AUDIO_ERROR_IO)
          {
          	printf("play start error\n");
            AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU); 
            AudioDemo.state = AUDIO_DEMO_IDLE;
          }
        }
        else /* Not idle */
        {
        	
          if(AUDIO_PLAYER_Process() == AUDIO_ERROR_IO)
          {
            /* Clear the LCD */
            LCD_ClearTextZone();

            AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU);  
            AudioDemo.state = AUDIO_DEMO_IDLE;
          }
          else
          {
						//printf("not error\n");
          }
        }
      }
      else
      {
        AudioDemo.state = AUDIO_DEMO_WAIT;
      }
      break; 
      
    default:
      break;
    }
  }
  
  //if(AppliState == APPLICATION_DISCONNECT)
  if(AppliState == APPLICATION_USBCONNECT)
  {
  	//printf("app usb connect \n");
  	MonitorUSBOperation();
    //AppliState = APPLICATION_IDLE;     
    //AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU); 
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);    
  }
}

static void AUDIO_ChangeSelectMode(AUDIO_DEMO_SelectMode select_mode)
{
  if(select_mode == AUDIO_SELECT_MENU)
  {
    //LCD_Update_Display(); 
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    LCD_DisplayPlayButton(ON);
    AudioDemo.state = AUDIO_DEMO_IDLE; 
    printf("change mode 1\n");
  }
  else if(select_mode == AUDIO_PLAYBACK_CONTROL)
  {
    LCD_ClearTextZone();  
    printf("change mode 2\n");
  }
}

static void LCD_Update_Display()
{
	uint8_t i = 0;
  
  for(i= 15; i < 22; i++)
  {
    BSP_LCD_ClearStringLine(i);
  }

	LCD_DisplayPlayButton(ON);
}

static void LCD_Pic_UpdateDisplay(void)
{
	uint8_t i = 0;
  
  for(i= 0; i < 5; i++)
  {
    BSP_LCD_ClearStringLine(i + 3);
  }
}


static void LCD_ClearTextZone(void)
{
  uint8_t i = 0;
  
  for(i= 0; i < 13; i++)
  {
    BSP_LCD_ClearStringLine(i + 3);
  }
}


