#ifndef _USER_SDRAM_H
#define _USER_SDRAM_H

#define SDRAM_BANK_ADDR                 ((uint32_t)0xC0000000)

typedef enum 
{
	PASSED = 0, 
	FAILED = !PASSED
}TestStatus_t;


extern void SDRAM_CpAndCk(void);
extern void SDRAM_Debug(void);

#endif
