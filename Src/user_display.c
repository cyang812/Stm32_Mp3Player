#include "main.h"

extern FILELIST_FileTypeDef FileList;
extern AUDIO_DEMO_StateMachine     AudioDemo;
extern RTC_DateTypeDef RTC_Get_Date;
extern RTC_TimeTypeDef RTC_Get_Time;
extern AUDIO_PLAYBACK_StateTypeDef AudioState;

void LCD_DisplayUSB(void)
{
	printf("LCD_DisplayUSB\n");
	BSP_LCD_Clear(LCD_COLOR_BLACK);

  /* Set the LCD Font */
  BSP_LCD_SetFont (&Font20);

  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

	BSP_LCD_DisplayStringAt(0,240, "USB connected", CENTER_MODE);
}


void LCD_SetHeader (uint8_t *header)
{
  /* Set the LCD Font */
  BSP_LCD_SetFont (&LCD_HEADER_FONT);

  BSP_LCD_SetTextColor(LCD_SOLID_BACKGROUND_COLOR);
  BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), LCD_HEADER_FONT.Height * 3);// H:60
  
  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_SOLID_TEXT_COLOR);
  BSP_LCD_SetBackColor(LCD_SOLID_BACKGROUND_COLOR);

  BSP_LCD_DisplayStringAt(0, LCD_HEADER_FONT.Height, header, CENTER_MODE);

  BSP_LCD_SetBackColor(LCD_BACKGROUND_COLOR);
  BSP_LCD_SetTextColor(LCD_TEXT_COLOR);
  BSP_LCD_SetFont (&LCD_TEXT_FONT);
}

void LCD_SetFooter(uint8_t *footer)
{
  /* Set the LCD Font */
  BSP_LCD_SetFont (&LCD_FOOTER_FONT);

  BSP_LCD_SetTextColor(LCD_SOLID_BACKGROUND_COLOR);
  BSP_LCD_FillRect(0, BSP_LCD_GetYSize() - LCD_FOOTER_FONT.Height - 4, BSP_LCD_GetXSize(), LCD_FOOTER_FONT.Height + 4);//H:16
  
  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_SOLID_TEXT_COLOR);
  BSP_LCD_SetBackColor(LCD_SOLID_BACKGROUND_COLOR);

  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - LCD_FOOTER_FONT.Height, footer, CENTER_MODE);

  BSP_LCD_SetBackColor(LCD_BACKGROUND_COLOR);
  BSP_LCD_SetTextColor(LCD_TEXT_COLOR);
  BSP_LCD_SetFont (&LCD_TEXT_FONT);
}

void LCD_DisplayRTCTime() // update when min change
{	
#ifndef SHOW_LRC 
	static uint8_t sec_count = 0;
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);

	if(bRTCTimeUpdate)
	{
		sec_count++;
		if(AudioState != AUDIO_STATE_STOP)
		{
			if(sec_count == 60 || bFirstDisplayRTCTime)
			{
				update_time();
				sprintf((char *)rtcTime,"%02d:%02d",RTC_Get_Time.Hours,RTC_Get_Time.Minutes);//,RTC_Get_Time.Seconds);
				printf("DisplayRTCTime:");printf(rtcTime);printf("\n");
				sprintf((char *)rtcDate,"20%02d/%02d/%02d",RTC_Get_Date.Year,RTC_Get_Date.Month,RTC_Get_Date.Date);
				printf("DisplayRTCTime:");printf(rtcDate);printf("\n");
				BSP_LCD_DisplayStringAt(600, 420, rtcTime, LEFT_MODE);	
				BSP_LCD_DisplayStringAt(600, 440, rtcDate, LEFT_MODE);	
				printf("LCD_DisplayRTCTime-> 1 min update time\n");
				bFirstDisplayRTCTime = false;
				sec_count = 0;
			}
		}
	}
	bRTCTimeUpdate = false;
#endif	
}

void LCD_DisplayMusicInfo() // 总时长+file Pos
{
	sprintf((char *)musicFilePos,"%02d/%02d",(int)(Get_MusicFilePos()+1), (uint16_t)(NumObs));
	printf("music file pos: %s\n",musicFilePos);
	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(674, 400, musicFullTime, LEFT_MODE);
	BSP_LCD_DisplayStringAt(684, 420, musicFilePos, LEFT_MODE);	
}

void LCD_DisplayMusicTimeRate()
{
	static uint32_t time_second = 0;
	static uint32_t lrc_time_idx = 0;
	if(bMusicRateUpdate)
	{
		time_second++; // update time count
		if(bFirstDisplayMusicTime)
		{
			printf("LCD_DisplayMusicRate -> first play music,clear time count\n");
			time_second = 0;
			lrc_time_idx = 0;
			bFirstDisplayMusicTime = false;
		}
		BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
		BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		sprintf((char *)musicRateTime,"%02d:%02d",Get_Min(time_second),Get_Sec(time_second));
		BSP_LCD_DisplayStringAt(600, 400, musicRateTime, LEFT_MODE);
		//printf(musicRateTime);printf("\n");
		//LCD_DisplayRTCTime();

		if(time_second == lrc.str_time[lrc_time_idx])  // lrc display
		{
			LCD_ClearZone(0,440,800,40,LCD_COLOR_BLACK); // lcd clear
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			LCD_DisplayStringAt(0,440,lrc.lrc_sub_str[lrc_time_idx].str_unicode, CENTER_MODE);
			lrc_time_idx++;
			printf("lrc_time_idx=%d\n",lrc_time_idx);
			if(time_second == lrc.str_time[lrc_time_idx]) //解决相同秒出现两句歌词
			{
				lrc_time_idx++;
				if(time_second == lrc.str_time[lrc_time_idx])  //解决相同秒出现三句歌词
				{
					lrc_time_idx++;                        
				}
			}
		}
	}
	bMusicRateUpdate = false;
}

void LCD_DisplayPlayerStatus(Player_Status_Display status,uint8_t vol)
{
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	if(status == PlayOrPause)
	{
		if(vol == 1)
			BSP_LCD_DisplayStringAt(90, 420, (uint8_t *)"  [PLAY ]", LEFT_MODE);
		else if(vol == 2)
			BSP_LCD_DisplayStringAt(90, 420, (uint8_t *)"  [PAUSE]", LEFT_MODE);
	} 
	else if(status == VolUpOrVolDown)
	{
		//BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
		//BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
		sprintf((char *)str,	"Volume : %lu ", vol);
		BSP_LCD_DisplayStringAt(90,400,str,LEFT_MODE);
	}
}

void LCD_DisplayMusicName(uint8_t idx,uint8_t unicode_num)
{
	static uint8_t unicode_idx = 0;
	//unicode_num =	GBK2Unicode(idx);

	if(bMusicNameFlash)
	{
		if(unicode_num >= 20) // music name display flash
		{
			if(bFirstDisplayMusicName)
			{
				unicode_idx = 0; // 切换歌曲时清除滚动位置，重新滚动
				bFirstDisplayMusicName = false;
			}
			if(unicode_idx == 20)unicode_idx = 0;
			LCD_ClearZone(0,0,800,60,LCD_COLOR_BLACK); // lcd clear
			LCD_DisplayStringRollAt(0,10,FileList.file[idx].unicode+unicode_idx,unicode_num,20);//H:80 
			unicode_idx++;
			//printf("LCD_DisplayMusicName roll \n");
		}		
		else if(bFirstDisplayMusicName)
		{
			LCD_ClearZone(0,0,800,60,LCD_COLOR_BLACK); // lcd clear
			LCD_DisplayStringAt(0,10,FileList.file[idx].unicode,CENTER_MODE);//music name
			bFirstDisplayMusicName = false;
		}
	}	
	bMusicNameFlash = false;
//		else
//		{
//			//BSP_LCD_SetFont(&Font20); 
//			//LCD_DisplayStringAt(0,10,FileList.file[idx].unicode,CENTER_MODE);//H:80 
//			printf("lcd_DisplayMusicName do not need flash");
//		}
	
//	for(uint8_t i = 0; i<40;i++)
//	{
//		printf("\n====");
//		printfhex(FileList.file[idx].unicode[i]);	
//	}
}

void LCD_DisplayStringRollAt(uint16_t Xpos, uint16_t Ypos, uint16_t * Text, uint8_t textsize, uint8_t LCD_char_num)
{
		uint16_t i = 0;
		uint32_t size = 0, xsize = 0;
		uint16_t	*ptr = Text;
		uint16_t ret_xs;
		uint16_t refcolumn = 160;

		/* Get the text size */
		while (*ptr++) size ++ ;
		//printf("filename char size: %d \n",size);

		/* Send the string character by character on LCD */
		while((*Text != 0) && i < LCD_char_num)
		{
			/* Display one character on LCD */
			ret_xs = LCD_DisplayChar(refcolumn, Ypos, *Text);
			/* Decrement the column position by 16 */
			//refcolumn += LCD_FONT_WIDTH;
			refcolumn += ret_xs;
			/* Point on the next character */
			Text++;
			i++;
		}
	
}

void LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint16_t * Text, Text_AlignModeTypdef Mode)
{

  uint16_t refcolumn = 1, i = 0;
  uint32_t size = 0, xsize = 0;
  uint16_t  *ptr = Text;
  uint16_t ret_xs;

  /* Get the text size */
  while (*ptr++) size ++ ;
	printf("filename char size: %d \n",size);
#if 1
  /* Characters number per line */
  xsize = (BSP_LCD_GetXSize()/LCD_FONT_WIDTH);

  switch (Mode)
  {
  case CENTER_MODE:
    {
      refcolumn = Xpos + ((xsize - size)* LCD_FONT_WIDTH) / 2;
      break;
    }
  case LEFT_MODE:
    {
      refcolumn = Xpos;
      break;
    }
  case RIGHT_MODE:
    {
      refcolumn = - Xpos + ((xsize - size)*LCD_FONT_WIDTH);
      break;
    }
  default:
    {
      refcolumn = Xpos;
      break;
    }
  }

  /* Check that the Start column is located in the screen */
  if ((refcolumn < 1) || (refcolumn >= 0x8000))
  {
    refcolumn = 1;
  }
#endif
  /* Send the string character by character on LCD */
  while ((*Text != 0) & (((BSP_LCD_GetXSize() - (i*LCD_FONT_WIDTH)) & 0xFFFF) >= LCD_FONT_WIDTH))
  //while(size)
  {
    /* Display one character on LCD */
		ret_xs = LCD_DisplayChar(refcolumn, Ypos, *Text);
    /* Decrement the column position by 16 */
    //refcolumn += LCD_FONT_WIDTH;
		refcolumn += ret_xs;
    /* Point on the next character */
    Text++;
    i++;
  }
}

uint16_t LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint16_t unicode)
{
	uint16_t ret_xs;
	uint8_t fontUnicodeBuff[72];
	uint8_t fontAsciiBuff[72];   //for font v1 32
	uint32_t add;
	uint8_t ret;
	add = GetFontAddress(unicode);
//	if(add == 0xffffffff) // v1.6 使用完整Unicode字符集 // 非常见Unicode编码，例如中文符号,从内置flash读取
//	{
//		ret = Get_Special_font(fontUnicodeBuff,unicode);
//		if(ret == OK)
//		{
//			LCD_DrawChar(Xpos, Ypos, fontUnicodeBuff,CHAR_TYPE_CH);
//			ret_xs = 24; 
//			return ret_xs;
//		}
//		else  // 未在内置的 flash 中找到字符点阵，显示一个A
//		{
//			add = (uint32_t)(0x21 * 3 *24); // 字符'A'的地址
//			BSP_QSPI_Read(fontUnicodeBuff,add,72);
//			//for(int i=0; i<72; i++)fontUnicodeBuff[i] = 0xff; // 显示黑块
//			LCD_DrawChar(Xpos, Ypos, fontUnicodeBuff,CHAR_TYPE_CH);
//			ret_xs = 24;
//			return ret_xs;
//		}
//	} 
//	else  // read font point data from external nor flash
	{
		//printf("unicode : 0x%04x\n",unicode);
		//printf("add : 0x%08x\n",add);
		if(add < CH_UNICODE_QSPI_OFFSET) //ascii
		{
			BSP_QSPI_Read(fontAsciiBuff,add,72);		
			LCD_DrawChar(Xpos, Ypos, fontAsciiBuff,CHAR_TYPE_EN);
			ret_xs = 18;  //for font v1 16
			return ret_xs;
		}
		else  // unicode
		{
			BSP_QSPI_Read(fontUnicodeBuff,add,72);
			LCD_DrawChar(Xpos, Ypos, fontUnicodeBuff,CHAR_TYPE_CH);
			ret_xs = 24;
			return ret_xs;
		}
	}
}

void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c, uint8_t type)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t  offset;
  uint8_t  *pchar;
  uint32_t line;

	if(type == CHAR_TYPE_EN)  //English 
	{
		height = 24;    //for font v1 16
	  width  = 24;		//for font v1 16
	} 
	else
	{
		height = 24;
	  width  = 24;

	}
  
  offset =  8 *((width + 7)/8) -  width ;

  for(i = 0; i < height; i++)
  {
    pchar = ((uint8_t *)c + (width + 7)/8 * i);

    switch(((width + 7)/8))
    {

    case 1:
      line =  pchar[0];
      break;

    case 2:
      line =  (pchar[0]<< 8) | pchar[1];
      break;

    case 3:
    default:
      line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];
      break;
    }

    for (j = 0; j < width; j++)
    {
      if(line & (1 << (width- j + offset- 1)))
      {
        BSP_LCD_DrawPixel((Xpos + j), Ypos, LCD_COLOR_CYAN);//DrawProp[ActiveLayer].TextColor);
      }
      else
      {
        BSP_LCD_DrawPixel((Xpos + j), Ypos, LCD_COLOR_BLACK);//DrawProp[ActiveLayer].BackColor);
      }
    }
    Ypos++;
  }
}

uint32_t LCD_DisplayUnicodeCharTest(uint16_t unicode)
{
	uint32_t add;
	uint8_t fontBuff[72];
	uint32_t offset = 0x00000c00;
	add = (uint32_t)(offset + (unicode - 0x4e00) * 3 * 24);
	BSP_QSPI_Read(fontBuff, add,72);
	LCD_DrawChar(350,140, fontBuff,CHAR_TYPE_EN);
	return add;
}
#if 0
extern const uint8_t st_logo[];
void LCD_DisplayBmp()
{
	BSP_LCD_DrawBitmap(200,200, (uint8_t *)st_logo);
}
#endif
void LCD_DisplayPlayButton(uint8_t ENABLE)
{	
	if(ENABLE)
	{
		BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
		Point PlaybackLogoPoints[] = {{TOUCH_PLAYBACK_XMIN, TOUCH_PLAYBACK_YMIN},
																	{TOUCH_PLAYBACK_XMAX, (TOUCH_PLAYBACK_YMIN+TOUCH_PLAYBACK_YMAX)/2},
																	{TOUCH_PLAYBACK_XMIN, TOUCH_PLAYBACK_YMAX}};
		BSP_LCD_FillPolygon(PlaybackLogoPoints, 3);
	}
	else
	{
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		Point PlaybackLogoPoints[] = {{TOUCH_PLAYBACK_XMIN, TOUCH_PLAYBACK_YMIN},
																	{TOUCH_PLAYBACK_XMAX, (TOUCH_PLAYBACK_YMIN+TOUCH_PLAYBACK_YMAX)/2},
																	{TOUCH_PLAYBACK_XMIN, TOUCH_PLAYBACK_YMAX}};
		BSP_LCD_FillPolygon(PlaybackLogoPoints, 3);
	}
}

void LCD_ClearZone(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint32_t color)
{ 
	BSP_LCD_SetTextColor(color);
	/* Draw rectangle with background color */
  BSP_LCD_FillRect(Xpos,Ypos,Width,Height); 
}


