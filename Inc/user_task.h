#ifndef __USER_TASK_H
#define __USER_TASK_H

//#include "main.h"

extern uint8_t AUDIO_StorageInit(void);
extern FRESULT AUDIO_StorageParse(void);
extern uint8_t AUDIO_ShowWavFiles(void);
extern uint16_t AUDIO_GetWavObjectNumber(void);

extern void FATFS_Init(void);
extern uint8_t MountFat32FileSystem(void);
extern uint8_t UnmountFilesystem(void);


extern void read_file(void);
extern void writefile_test(void);

extern void Scan_MusicFile(void);
extern void scan_files(void);
extern void show_filename(void);
extern void show_file(uint8_t idx);

extern void SD_Error_Handler(void);

extern uint8_t GBK2Unicode(uint8_t ptr);
extern uint32_t GetFontAddress(uint16_t unicode);

extern void USB_Debug();

extern void Set_Time(void);
extern void update_time(void);
extern void printf_time(void);

extern void printfhex(uint8_t hex8);
extern void printfhex16(uint16_t hex16);
extern void printfhex32(uint32_t hex32);

#endif