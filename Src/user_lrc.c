#include "main.h"
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <ctype.h>  

FIL MusicLrcFile;
LRC_struct lrc;

#define DLS_LEN		(uint8_t)129

uint8_t *tmp;  

/******************************************************************** 
*  Function：  str_replace() 
*  Description: 在一个字符串中查找一个子串，并且把所有符合的子串用 
另一个替换字符串替换。 
*  Input：      p_source:要查找的母字符串； p_seach要查找的子字符串; 
p_repstr：替换的字符串; 
*  Output：      p_result:存放结果; 
*  Return :      返回替换成功的子串数量; 
*  Others:      p_result要足够大的空间存放结果，所以输入参数都要以\0结束; 
*********************************************************************/  
int str_replace(char *p_result, const char* p_source, char* p_seach, char *p_repstr)  
{  
    int c = 0;  
    int repstr_leng = 0;  
    int searchstr_leng = 0;  
    char *p1;  
    char *presult = p_result;  
    const char *psource = p_source;  
    char *prep = p_repstr;  
    char *pseach = p_seach;  
    int nLen = 0;  
    repstr_leng = strlen(prep);  
    searchstr_leng = strlen(pseach);  
  
    do {  
        p1 = strstr(psource, p_seach);  
        if (p1 == 0)  
        {  
            strcpy(presult, psource);  
            return c;  
        }  
        c++;  //匹配子串计数加1;  
        //printf("结果:%s\r\n", p_result);  
        //printf("源字符:%s\r\n", p_source);  
        // 拷贝上一个替换点和下一个替换点中间的字符串  
        nLen = p1 - psource;  
        memcpy(presult, psource, nLen);  
        // 拷贝需要替换的字符串  
        memcpy(presult + nLen, p_repstr, repstr_leng);  
        psource = p1 + searchstr_leng;  
        presult = presult + nLen + repstr_leng;  
    } while (p1);  
  
    return c;  
}  


void UTF8ToUnicode(uint8_t *UTF8,uint16_t *Unicode)
{
	uint16_t i=0,j=0;
	uint8_t buf[4];
	
	while(UTF8[i])
	{
		if((UTF8[i]&0x80)==0x00)
		{
			Unicode[j]=(uint16_t)DWBYTE(0,0,0,UTF8[i]);
			i+=1;
		}
		else if((UTF8[i]&0xe0)==0xc0)
		{
			buf[1]=(UTF8[i]&0x1c)>>2;
			buf[0]=(UTF8[i]<<6)|(UTF8[i+1]&0x3f);
			Unicode[j]=(uint16_t)DWBYTE(0,0,buf[1],buf[0]);
			i+=2;
		}
		else if((UTF8[i]&0xf0)==0xe0)
		{
			buf[1]=(UTF8[i]<<4)|((UTF8[i+1]&0x3c)>>2);
			buf[0]=(UTF8[i+1]<<6)|(UTF8[i+2]&0x3f);
			Unicode[j]=(uint16_t)DWBYTE(0,0,buf[1],buf[0]);
			i+=3;
		}
		else if((UTF8[i]&0xf8)==0xf0)
		{
			buf[2]=((UTF8[i]&0x07)<<2)|((UTF8[i+1]&0x30)>>4);
			buf[1]=((UTF8[i+1]&0x0f)<<4)|((UTF8[i+2]&0x3c)>>2);
			buf[0]=(UTF8[i+2]<<6)|(UTF8[i+3]&0x3f);
			Unicode[j]=(uint16_t)DWBYTE(0,buf[2],buf[1],buf[0]);
			i+=4;
		}
		else
		{
			Unicode[j] = (uint16_t)UTF8[i];
			i++;
		}
		if(Unicode[j]<0x20)
			Unicode[j]=0x20;
		j++;
		if(i>=(DLS_LEN-1))
			break;
	}
	if(j>=(DLS_LEN-1))
		Unicode[DLS_LEN-1]=0x00;
	else
		Unicode[j]=0x00;
}
  
int chartoint(char ch){  
    return ch - '0';  
}  
  
uint16_t strtoint(char *str){//计算时间，返回秒   
    if(isdigit(str[0]) && isdigit(str[1])  
        && isdigit(str[0]) && isdigit(str[0])  
        && isdigit(str[0]) && isdigit(str[0])){  
            int mintue = chartoint(str[0]) * 10 + chartoint(str[1]);  
            int second = chartoint(str[3]) * 10 + chartoint(str[4]);  
            //int microsecond = chartoint(str[6]) * 10 + chartoint(str[7]);  
            return (mintue * 60 + second);  
        }  
    return -1;  
}  
  
uint8_t * praseLRC(uint8_t *str, uint16_t *time){  

		//printf("parse lrc\n");
	      
    if(strlen(str) == 0){//空的行   
        return NULL;  
    }else{  
        char *p, *temp;  
           
        p = strchr(str, '[');  
        if(p != NULL)   
            if(isdigit(*(p + 1))){  
                temp = p + 1;  
                p = strchr(str, ']');  
                temp[p - temp] = '\0';  
                //printf("%s\n", temp);  
                if((*time = strtoint(temp)) < 0){  
                    printf("error time");  
                    exit(1);  
                }  
                tmp = p + 1;  
                while(*p != '\n'){  
                    p++;  
                }  
                tmp[p - tmp] = '\0';  
                //printf("%s", lrc);  
                return tmp;  
            }  
        return NULL;  
    }  
    return NULL;  
}   
  
void Lrc_Parse(uint8_t idx)
{
	uint16_t ret,bytesread;
	uint8_t ret_sum;			
	char result_a[100] = { 0 };//存放替换结果;
	printf("Lrc_Parse\n");
	
	strcpy((char *)File_path,(char *)"0:/music/");  //change file path
	ret_sum =	str_replace(result_a, (const char *)FileList.file[idx].name,".mp3", ".lrc");

	if(ret_sum != 1)
	{
		printf("@@@@ replace lrc str error\n");
		return; // 没找到歌词
	}
	else
	{
		strcat((char *)File_path,(const TCHAR *)result_a);
	}

	//strcat((char *)File_path,(const TCHAR *)FileList.file[idx].name);
	//strcat((char *)File_path,(const TCHAR *)"1.lrc");
	
	ret = f_open(&MusicLrcFile, (const char *)File_path, FA_READ);   
	if(ret != 0) //error
	{
		printf("file_path:%s\n",File_path);
		printf("open music lrc file error ,ret: %d\n",ret);
		f_close(&MusicLrcFile);
	}
	else
	{
		uint16_t line = 0;
		
    tmp = (char *)malloc(sizeof(char) * MAXUTF8);  
    if(tmp == NULL){  
        printf("malloc buf err");   
    }

		printf("open **lrc** file succ \n");
		//GlobalPtr = (uint8_t *)LRC_Ram;
		//ret = f_read(&MusicLrcFile,LRC_Ram,MusicLrcFile.fsize, (void *)&bytesread); // read first 2048 bytes
		//printf("f_read_ret: %d,read lrc file byte num:%d\n",ret,bytesread);
		
		
    while((f_gets(LRC_Ram, MAXUTF8, &MusicLrcFile) != NULL) && line<MAXBUF){  //&& line<2
        if((GlobalPtr = praseLRC(LRC_Ram, &lrc.str_time[line])) != NULL){  

//						for(uint8_t xx=0; xx<MAXLINE; xx++)
//						{
//							printf("0x%x ",GlobalPtr[xx]);
//							if((xx%10==0)&&(xx!=0))printf("\n");
//						}

						for(uint8_t idx=0; idx<MAXUTF8; idx++)
						{
							lrc.lrc_sub_str[line].str_utf8[idx] = GlobalPtr[idx];
						}
            printf("\n utf-8 -> %d\t%s \n", lrc.str_time[line], lrc.lrc_sub_str[line].str_utf8);        

//						for(uint8_t xx=0; xx<MAXUTF8; xx++)
//						{
//							printf("0x%x ",lrc.lrc_sub_str[line].str_utf8[xx]);
//							if((xx%10==0)&&(xx!=0))printf("\n");
//						}

						UTF8ToUnicode(lrc.lrc_sub_str[line].str_utf8, lrc.lrc_sub_str[line].str_unicode);
						printf("\n unicode -> %d\t%s \n", lrc.str_time[line], lrc.lrc_sub_str[line].str_unicode);        

//						for(uint8_t xx=0; xx<MAXUNICODE; xx++)
//						{
//							printf("0x%x ",lrc.lrc_sub_str[line].str_unicode[xx]);
//							if((xx%10==0)&&(xx!=0))printf("\n");
//						}

//						LCD_ClearZone(0,440,800,40,LCD_COLOR_BLACK); // lcd clear
//						LCD_DisplayStringAt(0,440,lrc.lrc_sub_str[line].str_unicode, CENTER_MODE);
//            HAL_Delay(3000);

						line++;  
        }  
    }   

		printf("line=%d ,%d\t%s\n",line,lrc.str_time[0],lrc.lrc_sub_str[0].str_utf8);

		f_close(&MusicLrcFile);	
	}
}

void clear_Lrc_buff()
{
	LCD_ClearZone(0,440,800,40,LCD_COLOR_BLACK); // lcd display clear
	memset(lrc.str_time,0,sizeof(lrc.str_time));
	memset(lrc.lrc_sub_str,0,sizeof(lrc.lrc_sub_str));
}


