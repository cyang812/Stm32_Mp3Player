#include "main.h"

RTC_HandleTypeDef hrtc;
USBD_HandleTypeDef USBD_Device;

void LED_All_Init(void)
{
	BSP_LED_Init(LED1);
	BSP_LED_Init(LED2);
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
}

void LCD_AppInit()
{
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);   
  BSP_LCD_SelectLayer(0);
  BSP_LCD_DisplayOn();
  BSP_LCD_Clear(LCD_COLOR_BLACK);
  //LCD_SetHeader("music player");
  //LCD_SetFooter("this is a footer");
  LCD_DisplayPlayButton(ON);  
}

/* RTC init function */
void RTC_Init(void)
{
    /**Initialize RTC Only 
    */
  uint8_t ret;
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if ((ret = HAL_RTC_Init(&hrtc)) != HAL_OK)
  { 
    printf("ret %d\n",ret);
    Error_Handler();
  }
}


void USB_Init()
{
		/* Init Device Library */
	USBD_Init(&USBD_Device, &MSC_Desc, 0);
	USBD_DbgLog("usb step1");

	/* Add Supported Class */
	USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);
	USBD_DbgLog("usb step2");
	
	/* Add Storage callbacks for MSC Class */
	USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);
	USBD_DbgLog("usb step3");
	
	/* Start Device Process */
	USBD_Start(&USBD_Device);
	USBD_DbgLog("usb step4");
}

void System_Init()
{
	LED_All_Init();	// LED
	LCD_AppInit();  // LCD
	BSP_QSPI_Init();// NOR FLASH
	BSP_TS_Init(800,480); // Touch Screen
	FATFS_Init();         // FATFS
	MountFat32FileSystem(); 
	BSP_SDRAM_Init(); // SDRAM
	USB_Init();
	RTC_Init();
}


