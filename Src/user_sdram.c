#include "main.h"

#define BUFFER_SIZE         ((uint32_t)0x0100) //256
#define WRITE_READ_ADDR     ((uint32_t)0x0800) //2048

#define SDRAM_JPEG_SIZE_TEST       ((uint32_t)20735)
#define SDRAM_JPEG_WRITE_ADDR_TEST  SDRAM_DEVICE_SIZE/2 //((uint32_t)0x0900)

extern unsigned char Jpeg_Cover_Flash[20735];


/* Read/Write Buffers */
uint32_t aTxBuffer[BUFFER_SIZE];
//uint32_t aRxBuffer[BUFFER_SIZE];

uint8_t aRxBuffer[SDRAM_JPEG_SIZE_TEST];

/* Status variables */
__IO uint32_t uwWriteReadStatus = 0;

/* Counter index */
uint32_t uwIndex = 0;

//uint8_t SDRAM_Test[100]__attribute__((at(0xC0000000)))={0};

static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;

  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}

void SDRAM_CpAndCk()
{
/* Write data to the SDRAM memory */
  for (uwIndex = 0; uwIndex < SDRAM_JPEG_SIZE_TEST; uwIndex++)
  {
    *(__IO uint8_t*) (SDRAM_BANK_ADDR + SDRAM_JPEG_WRITE_ADDR_TEST + uwIndex) = Jpeg_Cover_Flash[uwIndex];
  }

	/* Read back data from the SDRAM memory */
	for (uwIndex = 0; uwIndex < SDRAM_JPEG_SIZE_TEST; uwIndex++)
	{
		aRxBuffer[uwIndex] = *(__IO uint8_t*) (SDRAM_BANK_ADDR + SDRAM_JPEG_WRITE_ADDR_TEST + uwIndex);
	}

	/*##-3- Checking data integrity ############################################*/

	for (uwIndex = 0; (uwIndex < SDRAM_JPEG_SIZE_TEST) && (uwWriteReadStatus == 0); uwIndex++)
	{
		if (aRxBuffer[uwIndex] != Jpeg_Cover_Flash[uwIndex])
		{
			uwWriteReadStatus++;
		}
	}
	
  if (uwWriteReadStatus != PASSED)
	{
			/* KO, turn on LED2 */
			printf("SDRAM_CpAndCk-> not ok\n");
			BSP_LED_On(LED2);
	}
	else
	{
			/* OK, turn on LED1 */
			printf("SDRAM_CpAndCk-> ok\n");
			BSP_LED_On(LED1);
	}
}

void SDRAM_Debug()
{
	/* Fill the buffer to write */
  Fill_Buffer(aTxBuffer, BUFFER_SIZE, 0xA244250F);

	/* Write data to the SDRAM memory */
  for (uwIndex = 0; uwIndex < BUFFER_SIZE; uwIndex++)
  {
    *(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + 4*uwIndex) = aTxBuffer[uwIndex];
  }

  /* Read back data from the SDRAM memory */
  for (uwIndex = 0; uwIndex < BUFFER_SIZE; uwIndex++)
  {
    aRxBuffer[uwIndex] = *(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + 4*uwIndex);
  }

  /*##-3- Checking data integrity ############################################*/

  for (uwIndex = 0; (uwIndex < BUFFER_SIZE) && (uwWriteReadStatus == 0); uwIndex++)
  {
    if (aRxBuffer[uwIndex] != aTxBuffer[uwIndex])
    {
      uwWriteReadStatus++;
    }
  }
  
	if (uwWriteReadStatus != PASSED)
	{
			/* KO, turn on LED2 */
			BSP_LED_On(LED2);
	}
	else
	{
			/* OK, turn on LED1 */
			BSP_LED_On(LED1);
	}
}
