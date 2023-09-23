#include "main.h"
#include "decode.h"

//#define SWAP_RB //

#define IMAGE_HEIGHT 640//320       // 高
#define IMAGE_WIDTH  640//320//320//227  // 宽

FIL PicFile;     /* Pic File object */
FIL MusicWavFile;
FIL MusicJpegFile;

RGB_typedef *RGB_matrix;
uint8_t _aucLine[2048];  // 图片中一行的数据
uint32_t line_counter = 0;  // 行数
uint16_t Xpos = 240;  // jpeg 第一个点的坐标
uint16_t Ypos = 60;  // jpeg 第一个点的坐标

/* This struct contains the JPEG decompression parameters */
struct jpeg_decompress_struct cinfo;
/* This struct represents a JPEG error handler */
struct jpeg_error_mgr jerr;

Cover_Jpeg CoverJpeg;

extern unsigned char Jpeg_Cover_Flash[5764];
extern unsigned char Jpeg_Cover_Ram[NumByte2ShowJpeg];

/**
  * @brief  Jpeg Decode
  * @param  callback: line decoding callback
  * @param  file1:    pointer to the jpg file
  * @param  width:    image width
  * @param  buff:     pointer to the image line    
  * @retval None
  */
void show_jepg(FIL *file,uint32_t width, uint8_t * buff, uint8_t (*callback)(uint8_t*, uint32_t), uint8_t datafrom)
{		
	printf("enter in show_jpeg\n");
	/* Decode JPEG Image */
	JSAMPROW buffer[2] = {0}; /* Output row buffer */
	uint32_t row_stride = 0; /* physical row width in image buffer */

	buffer[0] = buff;

	/* Step 1: allocate and initialize JPEG decompression object */
	cinfo.err = jpeg_std_error(&jerr);
	//printf("enter in show_jpeg step 1\n");

	/* Initialize the JPEG decompression object */	
	jpeg_create_decompress(&cinfo);
	printf("enter in show_jpeg step 2\n");

	if(datafrom ==  JPEG_From_Buff)
		jpeg_mem_src(&cinfo, Jpeg_Cover_Ram,CoverJpeg.Jpeg_Size);	// 从内存打开
	else if(datafrom == JPEG_From_FLASH)
		jpeg_mem_src(&cinfo, Jpeg_Cover_Flash, 20735);  // 从内置flash打开
	else if(datafrom == JPEG_From_SD)
	{	
		jpeg_stdio_src (&cinfo, file);    // 从文件打开
	}
	//jpeg_stdio_buffer_src(&cinfo, Jpeg_Cover_Ram, 5764);
	printf("enter in show_jpeg step 3\n");

	/* Step 3: read image parameters with jpeg_read_header() */
	jpeg_read_header(&cinfo, TRUE);
	printf("enter in show_jpeg step 4\n");

	/* Step 4: set parameters for decompression */
	cinfo.dct_method = JDCT_FLOAT;
	printf("enter in show_jpeg step 5\n");

	/* Step 5: start decompressor */
	jpeg_start_decompress(&cinfo);
	printf("enter in show_jpeg step 6\n");

	row_stride = width * 3;
	while (cinfo.output_scanline < cinfo.output_height)
	{
	
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);   
		//printf("enter in show_jpeg step 7 in while() \n");
		if (cinfo.output_scanline%2 != 0)       // 奇数行显示 1,3,5 640->320
			{
				if (callback(buffer[0], row_stride) != 0)
				{
					break;
				}
			}
	}

	/* Step 6: Finish decompression */
	jpeg_finish_decompress(&cinfo);
	printf("enter in show_jpeg step 8\n");

	/* Step 7: Release JPEG decompression object */
	jpeg_destroy_decompress(&cinfo);
	printf("enter in show_jpeg step 9\n");

	/* Step 8: 清空行计数器，以便下一次显示*/  
	line_counter = 0;
	printf("out of show_jpeg\n");
}

/**
  * @brief  Copy decompressed data to display buffer.
  * @param  Row: Output row buffer
  * @param  DataLength: Row width in output buffer
  * @retval None
  */
  // 显示一行
static uint8_t Jpeg_CallbackFunction(uint8_t* Row, uint32_t DataLength)  
{
	//printf("enter in jpeg callback \n");
	uint32_t i = 0,j = 0;
  RGB_matrix =  (RGB_typedef*)Row;
  uint32_t  ARGB8888Buffer[IMAGE_WIDTH];

#ifdef SWAP_RB
  for(i = 0; i < IMAGE_WIDTH; i++)
  {
    ARGB8888Buffer[i]  = (uint32_t)
      (
        0xFF000000                                       |
       (((uint32_t)(RGB_matrix[i].B) & 0x000000FF) >> 0) |
       (((uint32_t)(RGB_matrix[i].G) & 0x000000FF) << 8) |
       (((uint32_t)(RGB_matrix[i].R) & 0x000000FF) << 16)
      );

    BSP_LCD_DrawPixel((i + Xpos), (line_counter + Ypos), ARGB8888Buffer[i]);
  }
#else
  for(i = 0,j = 0; j < IMAGE_WIDTH; i++)
  {
    ARGB8888Buffer[j]  = (uint32_t)
      (
        0xFF000000                                       |
       (((uint32_t)(RGB_matrix[j].R) & 0x000000FF) >> 0) |
       (((uint32_t)(RGB_matrix[j].G) & 0x000000FF) << 8) |
       (((uint32_t)(RGB_matrix[j].B) & 0x000000FF) << 16)
      );

    BSP_LCD_DrawPixel((i + Xpos), (line_counter + Ypos), ARGB8888Buffer[j]);
		j+=2;//2  // 偶数列显示 2,4,6 640->320
  }
#endif
  line_counter++;
  return 0;
}
#if 1
void ReadStrUnit(uint8_t *src,uint8_t *desc, uint16_t idx, uint8_t NbrOfBytes)
{
	uint16_t index = 0;
	for (index = 0; index < NbrOfBytes; index++)
	{
		desc[index] = src[idx + index] ;
	}
	desc[index]='\0';
}

Jpeg_Error Get_MusicCoverJpeg(uint8_t idx)
{
	uint32_t HeadTabIdx;
	uint8_t ret,bytesread;
				
	printf("Get_MusicCoverJpeg\n");
	
	//strcpy((char *)File_path,(char *)"0:/music/test.mp3");  //change file path
	//strcat((char *)File_path,(char *)FileList.file[0].name);
	strcpy((char *)File_path,(char *)"0:/music/");  //change file path
	strcat((char *)File_path,(const TCHAR *)FileList.file[idx].name);

	ret = f_open(&MusicWavFile, (const char *)File_path, FA_READ);   
	if(ret != 0) //error
	{
		printf("file_path:%s\n",File_path);
		printf("open music jpeg file error ,ret: %d\n",ret);
		f_close(&MusicWavFile);
		return NOT_FIND_APIC;
	}
	else
	{
		printf("open music jpeg file succ \n");
		GlobalPtr = (uint8_t *)Jpeg_Cover_Ram;
		//f_lseek(&MusicWavFile,0x31d);
		ret = f_read(&MusicWavFile,GlobalPtr,NumByte2FindAPIC, (void *)&bytesread); // read first 2048 bytes
		printf("f_read_ret: %d,read music file byte num:%d\n",ret,bytesread);
		//for(uint32_t xx=0; xx < NumByte2FindAPIC; xx++)printf("readBUff[%d]:%#x\n",xx,GlobalPtr[xx]);
		strcpy((char *)Unicodebuf,"APIC"); // 图像数据
		HeadTabIdx=0;
		while (1)
		{
			ReadStrUnit(GlobalPtr,str,HeadTabIdx,strlen((char *)Unicodebuf));
			//printf("substr :%s\n",str);
			if(strcmp((char *)str,(char *)Unicodebuf)==0)break;
			HeadTabIdx++;
			if(HeadTabIdx>=NumByte2FindAPIC)
			{
				f_close(&MusicWavFile);
				printf("not find jpeg ,return and close music file,headTabIdx:%d\n",HeadTabIdx);
				return NOT_FIND_APIC;
			}
		}			
		printf("find APIC in pos: %#x\n",HeadTabIdx);
		HeadTabIdx+=strlen(Unicodebuf);   // +4
		CoverJpeg.Tag_Size = MAKE_DWORD(GlobalPtr[HeadTabIdx],GlobalPtr[HeadTabIdx+1],GlobalPtr[HeadTabIdx+2],GlobalPtr[HeadTabIdx+3]);
		printf("CoverJpeg.Tag_Szie:%#x\n",CoverJpeg.Tag_Size);

		f_lseek(&MusicWavFile,HeadTabIdx+6+14);		// 移动文件指针到 ff d8
		
		CoverJpeg.Jpeg_Size = CoverJpeg.Tag_Size - 14;
		printf("CoverJpeg.Jpeg_Szie:%#x, file size:%d KB\n",CoverJpeg.Jpeg_Size,CoverJpeg.Jpeg_Size/1024);

		if (CoverJpeg.Jpeg_Size > NumByte2ShowJpeg)
		{
			printf("jpeg is to big\n");
			f_close(&MusicWavFile);			
			return JPEG_TOO_BIG;
		}
		else f_read(&MusicWavFile,GlobalPtr,CoverJpeg.Jpeg_Size,(void *)&bytesread); // copy jpeg file from sd to ram

		printf("Jpeg_Cover_buff[%d]:%#x\n",0,GlobalPtr[0]);  // ff
		printf("Jpeg_Cover_buff[%d]:%#x\n",1,GlobalPtr[1]);  // d8
		if(GlobalPtr[0] != 0xff || GlobalPtr[1] != 0xd8)
		{
			if(GlobalPtr[0] == 0x89 || GlobalPtr[1] == 0x50)
			{
				printf("Jpeg is PNG type\n");
				return JPEG_PNG_TYPE;				
			}
			else 
			{
				printf("Jpeg is unknow type\n");
				return JPEG_UNKNOW_TYPE;				
			}
		}
			
		//printf("Jpeg_Cover_buff[%d]:%#x\n",83735,GlobalPtr[83735]);  // ff 
		//printf("Jpeg_Cover_buff[%d]:%#x\n",83736,GlobalPtr[83736]);  // d9

	}
	f_close(&MusicWavFile);
	return JPEG_NO_ERROR;
}
#endif

void Show_MusicJPEG(uint8_t idx)
{
	//LCD_ClearZone(240,50,320,320,LCD_COLOR_WHITE);  // clear lcd for display next cover
	
	int8_t flag,datafrom = JPEG_From_Buff;	
	if(idx == 0xff)  // music file is wav type
	{
		flag = 1;	  //  load cover from flash
	}
	else flag = Get_MusicCoverJpeg(idx);
	if(flag != 0)  // read jpeg error
	{
		datafrom = JPEG_From_FLASH;
		f_open(&PicFile,DEFAULT_COVER_JPEG_PATH,FA_READ);
		show_jepg(&PicFile,IMAGE_WIDTH, _aucLine, Jpeg_CallbackFunction, datafrom);		
		f_close(&PicFile);
	}
	else show_jepg(&PicFile,IMAGE_WIDTH, _aucLine, Jpeg_CallbackFunction, datafrom);		
}

void JPEG_Debug(void)
{
	uint8_t ret,bytesread;
	
	printf("JPEG_Debug\n");
	
	strcpy((char *)File_path,(char *)"0:/image.jpg");  //change file path
	//strcat((char *)File_path,(char *)FileList.file[0].name);

	ret = f_open(&PicFile, File_path, FA_READ);
	if(ret != 0) //error
	{
		printf("open jpeg file error ,ret: %d\n",ret);
	}
	else
	{
		printf("show the jpeg pic in lcd\n");
		show_jepg(&PicFile, IMAGE_WIDTH, _aucLine, Jpeg_CallbackFunction, JPEG_From_SD);		
		f_close(&PicFile);
	}

	HAL_Delay(5000);

	show_jepg(&PicFile, IMAGE_WIDTH, _aucLine, Jpeg_CallbackFunction, JPEG_From_FLASH);
		
	HAL_Delay(5000);

	Show_MusicJPEG(2);
}

