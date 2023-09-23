#include "main.h"

extern __IO uint32_t uwVolume;   

uint8_t Mp3FileInfo[40] = {0};
uint8_t readBuf[READBUF_SIZE];
int32_t offset; 
int bytesLeft; 
uint8_t *readPtr;
short 	*Decbuf;
uint8_t Decode_state;
uint8_t buffer_switch=1;
uint8_t init;

MP3_BUFF mp3_buff;    //解码后的音频数据 buff

ID3V1 gId3v1;
ID3V2 gId3v2;

HMP3Decoder hMP3Decoder;
MP3FrameInfo mp3FrameInfo; 

uint8_t waitflag = 0;    // wait for update mp3 to PCM data buff

void InitMp3(void)							
{
	hMP3Decoder=MP3InitDecoder();
	ClearBuffer((void *)&gId3v1,sizeof(ID3V1));
	ClearBuffer((void *)&gId3v2,sizeof(ID3V2));
}

uint8_t Mp3_GetID3V2(void)  //to get mp3 audio Frame start address store in labelSize
{
	uint32_t br,labelSize; 
	ID3V2_HEADER *pLabel;
	f_read(&MusicFile,readBuf,READBUF_SIZE,(void *)&br);
	if(br==0)
	{
		return false;
	}
	
	pLabel = (ID3V2_HEADER*)readBuf;
	//Parsing ID3V2
	if (memcmp(pLabel->Header, "ID3", 3) != 0)
	{
		f_lseek(&MusicFile,0);
		printf("No ID3\n");
		return false;
	}
	labelSize = (((pLabel->Size)[0] & 0x7F) * 0x200000UL)		//获取标签大小，
		   		+ (((pLabel->Size)[1] & 0x7F) * 0x4000UL)				//一共四个字节，但是每个字节最高位为0
		   		+ (((pLabel->Size)[2] & 0x7F) * 0x80UL)	        //因此共28位，标签大小的计算公式如左边所示
		   		+ ((pLabel->Size)[3] & 0x7F)
					+ 10;					//由于标签头中的显示的标签大小不包含标签头自身的大小，所以要加上标签头的10个字节							
	printf("labelSize: %d\n",labelSize);
	f_lseek(&MusicFile,labelSize);  //移动文件指针到音频数据处
	return true;
}

void Mp3Play_init(void)
{	
	uint8_t ret;
	uint32_t duration;
	//if(init==0)//根据MP3帧信息初始化音频接口
	{
	if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, uwVolume, mp3FrameInfo.samprate/2) == 0)//init sai /2
	{
		printf("bsp_audio_out init succ\n");
		ret = BSP_AUDIO_OUT_Play((uint16_t*)&mp3_buff.buffer1[0], 4*BUFF_SIZE);//init dma
		//ret = BSP_AUDIO_OUT_Play((uint16_t*)&mp3_buff.twobuff, BUFF_SIZE);//init dma
    printf("bsp_out ret: %d\n",ret);
	}
	printf("play start the bitrate: %d\n",mp3FrameInfo.bitrate);

	duration = MusicFile.fsize * 8 / mp3FrameInfo.bitrate; //计算歌曲总时长，文件大小(Byte) * 8 / 比特率（bps）
	sprintf((char *)musicFullTime,"/%02d:%02d",(int)(duration/60), (int)(duration%60));
	printf(musicFullTime);printf("\n");
	}	
}
/*
	-1、先对读入缓存中的数据进行解码到输出缓存s
	-2、解码完成后从文件中读取数据更新缓存
*/
mp3_play_status Mp3_SubDecode(void)  
{
	uint32_t br;
	int res;
	offset=MP3FindSyncWord(readPtr, bytesLeft);//寻找下一帧头	assume EOF if no sync found
	//printf("offset:");printfhex32(offset);printf("\n");
	if(offset<0)
	{	
		//PlayDataLength=0;
		printf("sub decode mp3 file error\n");
		return MP3_PLAY_END; 
	}
	readPtr+=offset; //data start point
	bytesLeft-=offset; //number of valid bytes remaining in inbuf
	res=MP3Decode(hMP3Decoder,&readPtr,&bytesLeft,Decbuf,0);
	//sprintf((char *) Unicodebuf,"res=%d  bytesLeft=%d\n",(int)res,(int)bytesLeft);
	//printf(Unicodebuf);
	if(bytesLeft<READBUF_SIZE) // update data in the read buff
	{
		memmove(readBuf,readPtr,bytesLeft);  // 将readPtr 中的内容搬移到 readBuf 中，即将同步头之后的内容搬移到buff头部
		//br=Read_file((readBuf+bytesLeft), (READBUF_SIZE-bytesLeft));
		res=f_read(&MusicFile,(readBuf+bytesLeft),(READBUF_SIZE-bytesLeft),(void *)&br);
		if(br==0)
		{ 
			printf("mp3 play end\n");
			//while(1){}
			return MP3_PLAY_END; 					
		} 
		if(br<READBUF_SIZE-bytesLeft)
			memset(readBuf+bytesLeft+br,0,READBUF_SIZE-bytesLeft-br);
		bytesLeft=READBUF_SIZE;
		readPtr=readBuf;                
	}
	MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);

	//printf("mp3 sub decode-> mmp3 bitrate:%d\n",mp3FrameInfo.bitrate);
	
	if(mp3FrameInfo.nChans==1)Convert_Mono(Decbuf);
	//else Convert_Stereo(Decbuf);
	return MP3_PLAY_PLAYING;
}

uint8_t Demo_Mp3Parsing(void)
{
	uint32_t br;
	Mp3_GetID3V2();  //to change file ptr pointer to real mp3 music data
	//init=0;
	//XferCplt = 0;
	buffer_switch = 1;
	
	bytesLeft=0;
	readPtr=readBuf;	
	f_read(&MusicFile,readBuf,READBUF_SIZE,(void *)&br);
	bytesLeft += br;	
	sprintf((char *) Unicodebuf,"bytesLeft=%d\n",(int)bytesLeft);
	printf(Unicodebuf);
	if(br==0) return false;
	
	Decbuf=(short *)mp3_buff.buffer1;
	Mp3_SubDecode();
	Decbuf=(short *)mp3_buff.buffer2;
	Mp3_SubDecode();
	Mp3Play_init();
	sprintf((char *) Unicodebuf,"bytesLeft::::%d\n",(int)bytesLeft);
	printf(Unicodebuf);
	return true;
}

mp3_play_status Mp3_PlayStart(void)
{
	mp3_play_status status;
	if (waitflag)
	{			
		if(buffer_switch == 2)
		{
				//printf("update buff1\n");
				waitflag = 0;
				Decbuf=(short *)mp3_buff.buffer1;
				//Decbuf=(short *)mp3_buff.twobuff;
				status = Mp3_SubDecode();
		}

		if(buffer_switch == 1)
		{
				//printf("update buff2\n");
				waitflag = 0;			
				Decbuf=(short *)mp3_buff.buffer2;
				//Decbuf=(short *)mp3_buff.twobuff[BUFF_SIZE];
				status = Mp3_SubDecode();
		}
	}

	return status; 
}


void MP3_Debug()
{
	uint8_t ret,bytesread;
	
	printf("MP3_Debug\n");
	
	strcpy((char *)File_path,(char *)"0:/mp3/");  //change file path
	strcat((char *)File_path,(char *)FileList.file[0].name);

	ret = f_open(&MusicFile, File_path, FA_OPEN_EXISTING | FA_READ);
	if(ret != 0) //error
	{
		printf("open mp3 file error ,ret: %d\n",ret);
	}
	else
	{
//		 if((ret = f_read(&Mp3File, Mp3FileInfo, sizeof(ID3V2_HEADER), (void *)&bytesread)) != FR_OK)
//		 {
//				printf("read mp3 file error ,ret: %d\n",ret);		
//		 }
//		 else
//		 {
//				LCD_DisplayMusicName(0);
//				for(uint8_t xx=0; xx<sizeof(ID3V2_HEADER); xx++)printf("0x%02x ",Mp3FileInfo[xx]);
//		 }

			//Mp3_GetID3V2();
			InitMp3();
			Demo_Mp3Parsing();
			while (1)
				{
					Mp3_PlayStart();
				}
	}
}
#if 0
/**
  * @brief  Calculates the remaining file size and new position of the pointer.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	//printf("change to buff1\n");

 		waitflag = 1;
//	Decbuf=(short *)buffer2;
		buffer_switch = 1;
		//Decbuf=(short *)buffer1;
		//Mp3_SubDecode();

}

/**
  * @brief  Manages the DMA Half Transfer complete interrupt.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{ 
	//printf("change to buff2\n");
	
	waitflag = 1;
	//Decbuf=(short *)buffer1;
	buffer_switch = 2;
	//Decbuf=(short *)buffer2;
	//Mp3_SubDecode();
}
#endif

