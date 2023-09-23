#ifndef _AUDIO_PLAY_H
#define _AUDIO_PLAY_H

/* The Audio file is flashed with ST-Link Utility @ flash address =  AUDIO_SRC_FILE_ADDRESS */
#define AUDIO_SRC_FILE_ADDRESS       0x08080000   /* Audio file address in flash */
#define AUDIO_FILE_SIZE               0x80000

void AudioPlay_demo(void);
uint8_t AUDIO_Play_Start(uint32_t *psrc_address, uint32_t file_size);
uint8_t AUDIO_Play_Process(void);

#endif
