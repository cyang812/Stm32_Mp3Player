#include "main.h"

#define TEST_MUSIC_NAME1 "0:/1.WAV"
#define TEST_MUSIC_NAME2 "0:/test/1.WAV"
#define TEST_MUSIC_NAME3 "0:/test/1.WAV.WAV"

FATFS SDFatFs;
//FIL MyFile;
FIL fil;

RTC_DateTypeDef RTC_Set_Date;
RTC_TimeTypeDef RTC_Set_Time;

RTC_DateTypeDef RTC_Get_Date;
RTC_TimeTypeDef RTC_Get_Time;

extern RTC_HandleTypeDef hrtc;

extern AUDIO_ApplicationTypeDef AppliState;

extern FILELIST_FileTypeDef FileList;
extern uint16_t NumObs;

char SD_Path[4] = "0:/";
uint8_t retSD;    /* Return value for SD */
//char SD_Path[4];  /* SD logical drive path */
uint8_t retUSBH;    /* Return value for USBH */
char USBH_Path[4];  /* USBH logical drive path */


#if _USE_LFN
static char lfn[_MAX_LFN];
#endif

void FATFS_Init(void)
{
	retSD = FATFS_LinkDriver(&SD_Driver, SD_Path);
}

uint8_t MountFat32FileSystem(void)
{
		FRESULT ret ;
		ret = f_mount(&SDFatFs, (TCHAR const*)SD_Path, 0);
		printf("ret : %d\n",ret);
		if(ret != FR_OK)
    {
      if(ret ==FR_DISK_ERR)
			{
//				if(f_mkfs((TCHAR const*)SD_Path, 0, 0) != FR_OK)		//format the SDCard with FAT32 filesystem
//				{
//					/* FatFs Format Error */
//					Error_Handler();
//				}
			}
			else
			{
      	SD_Error_Handler();
			}
    }
		return 0;
}

uint8_t UnmountFilesystem(void)
{
	FRESULT ret;
	ret =f_mount(NULL, "0:/", 0);

	return ret;
}

uint8_t AUDIO_ShowWavFiles(void)
{
	//scan_files();
	if(FileList.ptr > 0)
  {
		printf("numosb :%d",AUDIO_GetWavObjectNumber());
		//LCD_DisplayMusicName( FileList.file[0].name);
		return 0;
  }
  else
  {
		printf("show wav files error");
		return 1;
  }
}

uint16_t AUDIO_GetWavObjectNumber(void)
{
  return NumObs;
}

void Scan_MusicFile()
{
	DIR Dir;
	FILINFO Fileinfo;
	uint8_t ret;

	FileList.ptr = 0;

#if _USE_LFN
  Fileinfo.lfname = (TCHAR *)lfn;
  Fileinfo.lfsize = sizeof(lfn);
#endif

	if(ret = f_opendir(&Dir,(const TCHAR *)CHOOSE_DIR_PATH) != FR_OK)
	{
		printf("sacn files ret : %d \n",ret);
		SD_Error_Handler();
	}
	else
	{
		printf("open dir succ \n");
		for(;;)
		{
			ret = f_readdir(&Dir,&Fileinfo);
			if(ret != FR_OK || Fileinfo.fname[0] == 0)
			{
				break;//Break on error or end of dir
			}
			//else
			//{	
			//	if(Fileinfo.lfname[0] == 0)
			//	{
			//		printf("lfname : error\n");
			//	}
			//	else
			//		printf("%s\n",Fileinfo.lfname);
			//}
			if(Fileinfo.fattrib & AM_ARC)//is a file?
			{
				if((strstr(Fileinfo.lfname, "wav")) || (strstr(Fileinfo.lfname, "WAV")) )// is a wav music file?
				{
					strcpy((char *)FileList.file[FileList.ptr].name,(char *)Fileinfo.lfname);
					FileList.file[FileList.ptr].type = WAV_TYPE;
					FileList.ptr++;
				}
				else if((strstr(Fileinfo.lfname, "MP3")) || (strstr(Fileinfo.lfname, "mp3")) )// is a mp3 music file?
				{
					strcpy((char *)FileList.file[FileList.ptr].name,(char *)Fileinfo.lfname);
					FileList.file[FileList.ptr].type = MP3_TYPE;
					FileList.ptr++;
				}
			}
		}
		f_closedir(&Dir);
	}
	NumObs = FileList.ptr;
	printf("NumObs %d \n",NumObs);
	show_filename();   // just printf filename in uart
}

void read_file(void)
{
	FIL MyFile;
	uint8_t data[32];
	uint8_t bytecounts;
	uint8_t ret;

	printf("read file\n");
	ret = f_open(&MyFile,(const TCHAR *)TEST_MUSIC_NAME3, FA_READ);
	printf("ret : %d \n",ret);
	
	if(ret == FR_OK)
	{
		printf("open file succ\n");
		ret = f_read(&MyFile, data, sizeof(data),(UINT *)&bytecounts);
		if (ret = FR_OK)
		{
			printf("read file succ\n");
		}
		else
		{
			printf("read file error\n");	
		}
	}
	else
	{
		printf("open file error\n");
	}
}

void writefile_test(void)
{
    //FIL fil;
    uint8_t ret,fr;
    char line[82]; /* Line buffer */

	#if 0
    /* Opens an existing file. If not exist, creates a new file. */
    ret = f_open(&fil, "mytest.txt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
    if (ret != FR_OK)
    {
			printf("ret : %d \n",ret);
    }
    f_printf(&fil, "%s\n", "[USB]write txt OK!0123456789");
    printf("open succ");

    /* Close the file */
    f_close(&fil);
	#else 
		fr = f_mount(&SDFatFs, (TCHAR const*)SD_Path, 0);
		if(!fr)
		{
			printf("mount succ");
	    /* Open a text file */
	    fr = f_open(&fil, "mytest.txt", FA_READ);
	    if (fr) 
	    {
				printf("fr : %d",fr);
	    }

	    /* Read all lines and display it */
	    while (f_gets(line, sizeof line, &fil))
	        printf(line);

	    /* Close the file */
	    f_close(&fil);
	  }
  #endif
}

void scan_files(void)
{
	DIR Dir;
	FILINFO Fileinfo;
	
#if _USE_LFN
  Fileinfo.lfname = (TCHAR *)lfn;
  Fileinfo.lfsize = sizeof(lfn);
#endif 

	uint8_t ret;
	FileList.ptr = 0;
	
	if(FATFS_LinkDriver(&SD_Driver, SD_Path) == 0) 
  {
    if(f_mount(&SDFatFs, (TCHAR const*)SD_Path, 0) != FR_OK)
    {
      SD_Error_Handler();
    }
    else
    {
			if((ret = f_opendir(&Dir,(const TCHAR *)ROOT_DIR_PATH)) != FR_OK)
			{
				//printf("ret : %d ",ret);
				SD_Error_Handler();
			}
			else
			{
				//printf("open music dir\n");
				for(;;)
				{
					ret = f_readdir(&Dir,&Fileinfo);
					if(ret != FR_OK || Fileinfo.fname[0] == 0)
					{
						break;//Break on error or end of dir
					}
					//else
					//{	
						//if(Fileinfo.lfname[0] == 0)
						//{
						//	printf("lfname : error\n");
						//}
						//else
						//	printf("%s\n",Fileinfo.lfname);
					//}
					if(Fileinfo.fattrib & AM_ARC)//is a file?
					{
						if((strstr(Fileinfo.lfname, "wav")) || (strstr(Fileinfo.lfname, "WAV"))
								|| (strstr(Fileinfo.lfname, "MP3")) || (strstr(Fileinfo.lfname, "mp3")) )//is a music file?
						{
							strcpy((char *)FileList.file[FileList.ptr].name,(char *)Fileinfo.lfname);
							FileList.ptr++;
						}
					}
				}
			}
		}
	}
	NumObs = FileList.ptr;
	printf("NumObs %d \n",NumObs);
	//show_filename(sum);
	//show_file(0);
	FATFS_UnLinkDriver(SD_Path);
}

void show_filename()
{
  uint8_t num;
  num = AUDIO_GetWavObjectNumber();
  for(uint8_t i = 0;i<num;i++)
  {
		printf("music name:%s\n",FileList.file[i].name);
		printf("music unicode:%s\n",FileList.file[i].unicode);
  }
}

void show_file(uint8_t idx)
{
	uint8_t data[32];
	uint8_t bytecounts;
	uint8_t ret;
	
	printf("show file\n");
	strcpy((char *)File_path,(char *)"0:/emusic/");
	strcat((char *)File_path,(char *)FileList.file[idx].name);
	
	if((ret = f_open(&fil,(const TCHAR *)File_path,FA_READ)) != FR_OK)
	{
		printf("ret : %d\n",ret);
		printf("e123");
		SD_Error_Handler();
	}
	else
	{
		if(f_read(&fil, data, sizeof(data), (UINT *)&bytecounts) != FR_OK)
		{
			printf("e456");
			SD_Error_Handler();
		}
		else
		{
			for(int i=0;i<32;i++)
			{
				printf(" %02x ",data[i]);
			}
			f_close(&fil);
		}
	}
}

void SD_Error_Handler(void)
{
	for(;;)
	{
		BSP_LED_Toggle(LED3);
		HAL_Delay(2000);
		printf("SD_ERROR");
	}
}

uint32_t GetFontAddress(uint16_t unicode)
{
	uint32_t add;
	uint8_t idx;
	if(unicode<0x0080) //ascii
	{
		idx = (uint8_t)unicode - 0x20; 
		add = (uint32_t)(idx * 3 * 24); 	//English font width 3x8, height 24 .raw 17x24
																	// for font v1 idx * 2 * 16 		
	}
	else //complete unicode //(unicode>0x4e00 && unicode<0x9fff) //中文编码 0x4e00~0x9fff
	{
		//add = (CH_UNICODE_QSPI_OFFSET + (unicode - 0x4e00) * 3 * 24);   //chinese font width 3x8, height 24
		add = (CH_UNICODE_QSPI_OFFSET + (unicode) * 3 * 24);   //unicode font width 3x8, height 24

		//printf("get font add -> unicode:%#x,add:%#x\n",unicode,add); // for test
	}
//	else    // 特殊符号的处理，例如中文的括号 
//	{
//		//add = (uint32_t)(0x21 * 3 *24); // 字符'A'的地址
//		add = 0xffffffff; // 特殊字符处理
//	}
	return add;
} 

uint8_t GBK2Unicode(uint8_t ptr)
{
	uint8_t idx = 0; // unicode count
	uint8_t namesize;
	namesize = strlen(FileList.file[ptr].name);

	//printf("GBK2Unicode -> file namesize:%d\n",namesize);
	
	//for(uint8_t i = 0;i<FileList.ptr;i++)
	//{
		for(uint8_t j = 0;j<namesize;j++,idx++)
		{
			//printfhex(FileList.file[i].name[j]);
			//printf("+++");
			uint8_t ch = 0x00;
			uint8_t cl;
			uint16_t GBK;
			if(FileList.file[ptr].name[j]>0x80) // 双字节表示
			{
				ch = FileList.file[ptr].name[j];
				cl = FileList.file[ptr].name[j+1];
				GBK = MAKE_WORD(ch, cl);
				j+=1;
			}
			else  // ascii
			{
				cl = FileList.file[ptr].name[j];
				GBK = MAKE_WORD(ch, cl);
			}
			
			FileList.file[ptr].unicode[idx]= ff_convert(GBK, 1); // 查表转换 GBK to Unicode
			//printfhex16(FileList.file[i].unicode[j]);
			//printf("\n============\n");
		}
		//for(uint8_t x = 0; x<40;x++)
		//{
			//printf("\nGBK2Uincode====");
		//	printfhex(FileList.file[ptr].unicode[x]);
		//}
		//printf("end a file");
	//}
	
	printf("GBK2Unicode -> file name unicode size:%d\n",idx);
	return idx;  // real unicode num;
}

void USB_Debug()
{
	//uint8_t PA9_Status = LOW;
	PA9_Status = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);
	//printf("USB_Debug -> PA9_Status = %d\n",PA9_Status);
}

void MonitorUSBOperation()
{
	//USB_Debug(); // detect usb otg connect by PA9
	//if(bUSB_Connect)
	if(PA9_Status)  // connect
	{
		printf("MointorUSBOperation -> bUSB_Connect = true \n ");
		if(bFirstDisplay_USB_Connect)
		{
			LCD_DisplayUSB();
		}
		bFirstDisplay_USB_Connect = false;
		bUSB_DisConnect_Enable = true;

		// stop play music
		AppliState = APPLICATION_DISCONNECT;
	}
	#if 0
	else // not connect
	{
		if(bUSB_DisConnect_Enable)
		{			
			#if 1
				printf("USB DISCONNECT \n");
				Scan_MusicFile();
				AppliState = APPLICATION_READY;
				BSP_LCD_Clear(LCD_COLOR_BLACK);
				LCD_DisplayPlayButton(ON);
			#endif
			
			bUSB_DisConnect_Enable = false;
		}
	}
	#endif
}


void Set_Time()
{
	RTC_Set_Date.Year=17;
	RTC_Set_Date.Month=3;
	RTC_Set_Date.Date=1;
	RTC_Set_Time.Hours=13;
	RTC_Set_Time.Minutes=10;
	RTC_Set_Time.Seconds=0;

	if (HAL_RTC_SetDate(&hrtc, &RTC_Set_Date, RTC_FORMAT_BIN) != HAL_OK)
	{
		printf("Rtc Date init failed\n");
	}
	if (HAL_RTC_SetTime(&hrtc, &RTC_Set_Time, RTC_FORMAT_BIN) != HAL_OK)
	{
		printf("Rtc time init failed\n");
	}
}

void update_time()
{
	if(HAL_RTC_GetDate(&hrtc, &RTC_Get_Date, RTC_FORMAT_BIN) == HAL_OK)
	{
		//printf("get data succ");
		printf("20%d/%d/%d ",RTC_Get_Date.Year,RTC_Get_Date.Month,RTC_Get_Date.Date);
	}

	if(HAL_RTC_GetTime(&hrtc, &RTC_Get_Time, RTC_FORMAT_BIN) == HAL_OK)
	{
		//printf("get time succ");
		printf("%d:%d:%d \n",RTC_Get_Time.Hours,RTC_Get_Time.Minutes,RTC_Get_Time.Seconds);
	}
}

void printf_time()
{
	if(HAL_RTC_GetDate(&hrtc, &RTC_Get_Date, RTC_FORMAT_BIN) == HAL_OK)
	{
		//printf("get data succ");
		printf("20%d/%d/%d ",RTC_Get_Date.Year,RTC_Get_Date.Month,RTC_Get_Date.Date);
	}

	if(HAL_RTC_GetTime(&hrtc, &RTC_Get_Time, RTC_FORMAT_BIN) == HAL_OK)
	{
		//printf("get time succ");
		printf("%d:%d:%d \n",RTC_Get_Time.Hours,RTC_Get_Time.Minutes,RTC_Get_Time.Seconds);
	}
}

void printfhex(uint8_t hex8)
{
	uint8_t s[10];
	if (hex8 == 0) 
	{
		printf("0");
		return;
	}
	sprintf((char *)s,"%x ",(int)hex8);
	printf(s);	
}
void printfhex16(uint16_t hex16)
{
	uint8_t s[10];
	if (hex16 == 0) 
	{
		printf("0");
		return;
	}
	sprintf((char *)s,"%0x",(int)hex16);
	printf(s);
}
void printfhex32(uint32_t hex32)
{
	uint8_t s[10];
	if (hex32 == 0) 
	{
		printf("0");
		return;
	}
	sprintf((char *)s,"%0x ",(int)hex32);
	printf(s);
}
