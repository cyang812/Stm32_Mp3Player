#ifndef _USER_MENU_H
#define _USER_MENU_H

//modify to change playbutton place
#define TOUCH_PLAYBACK_XMIN     370//200
#define TOUCH_PLAYBACK_XMAX     430//260
#define TOUCH_PLAYBACK_YMIN     210
#define TOUCH_PLAYBACK_YMAX     270

typedef enum {
  AUDIO_SELECT_MENU = 0,
  AUDIO_PLAYBACK_CONTROL,  
}AUDIO_DEMO_SelectMode;

extern void Menu_prase(void);
static void AUDIO_ChangeSelectMode(AUDIO_DEMO_SelectMode select_mode);
static void LCD_ClearTextZone(void);
static void LCD_Update_Display(void);
static void LCD_Pic_UpdateDisplay(void);


#endif
