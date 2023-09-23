#ifndef _USER_DISPLAY_H
#define _USER_DISPLAY_H

#define     LCD_DEFAULT_COLOR    LCD_COLOR_WHITE

/* Comment the line below to disable the scroll back and forward features */
#define     LCD_SCROLL_ENABLED      1 

#define     LCD_HEADER_FONT                   Font20
#define     LCD_FOOTER_FONT                   Font12
#define     LCD_TEXT_FONT                     Font16

#define 		LCD_FONT_HEIGHT      16
#define 		LCD_FONT_WIDTH			 16

#define     FONT_V3_HEIGHT       24
#define     FONT_V3_WIDTH        24

/* Define the LCD LOG Color  */
#define     LCD_BACKGROUND_COLOR              LCD_COLOR_BLACK
#define     LCD_TEXT_COLOR                    LCD_COLOR_WHITE

#define     LCD_SOLID_BACKGROUND_COLOR        LCD_COLOR_BLUE
#define     LCD_SOLID_TEXT_COLOR              LCD_COLOR_WHITE


#define CH_UNICODE_QSPI_OFFSET 0x00002000  //chinese font flash address offset

#define CHAR_TYPE_EN   0
#define CHAR_TYPE_CH   1

#define MAKE_WORD(b1, b0)          (((uint16_t)(b1) << 8) | (uint8_t)(b0))
#define MAKE_DWORD(b3, b2, b1, b0) (((uint32_t)((uint8_t)(b3) << 24)) | ((uint8_t)(b2) << 16) | ((uint8_t)(b1) << 8) | ((uint8_t)(b0)))


#define Get_Min(second)     ((uint8_t) (second/60))
#define Get_Sec(second)     ((uint8_t) (second%60))


typedef enum _Player_Status_Display
{
	PlayOrPause =0,
	VolUpOrVolDown,
}Player_Status_Display;

extern void LCD_DisplayUSB(void);

extern void LCD_SetHeader(uint8_t * header);
extern void LCD_SetFooter(uint8_t * footer);
extern void LCD_DisplayRTCTime(void);
extern void LCD_DisplayMusicInfo(void);
extern void LCD_DisplayMusicName(uint8_t idx,uint8_t unicode_num);
extern void LCD_DisplayMusicTimeRate(void);



extern void LCD_DisplayStringRollAt(uint16_t Xpos, uint16_t Ypos, uint16_t * Text, uint8_t textsize, uint8_t LCD_char_num);
extern void LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint16_t * Text, Text_AlignModeTypdef Mode);
extern uint16_t LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint16_t unicode);
extern void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c,uint8_t type);

extern uint32_t LCD_DisplayUnicodeCharTest(uint16_t unicode);

extern void LCD_DisplayBmp(void);
extern void LCD_DisplayPlayButton(uint8_t ENABLE);

extern void LCD_ClearZone(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint32_t color);


#endif
