#include "spi.h"

#include <string.h>

#include "stm32f4xx.h"

SPI_HandleTypeDef spi;

void spi_setup(void)
{
    memset(&spi, 0, sizeof(spi));

    spi.Instance                 = SPI1;
    spi.Init.Mode                = SPI_MODE_MASTER;
    spi.Init.Direction           = SPI_DIRECTION_2LINES;
    spi.Init.DataSize            = SPI_DATASIZE_8BIT;
    spi.Init.CLKPhase            = SPI_PHASE_1EDGE;
    spi.Init.CLKPolarity         = SPI_POLARITY_LOW;
    spi.Init.NSS                 = SPI_NSS_SOFT;
    spi.Init.BaudRatePrescaler   = SPI_BAUDRATEPRESCALER_16;
    spi.Init.FirstBit            = SPI_FIRSTBIT_MSB;

    HAL_SPI_Init(&spi);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, SET);
}

void spi_transfer(uint8_t *tx, uint8_t *rx, uint32_t len)
{
   if (tx != NULL || rx != NULL)
   {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, RESET);
   }

   if (tx != NULL && rx != NULL)
   {
      HAL_SPI_TransmitReceive(&spi, tx, rx, len, HAL_MAX_DELAY);
   }
   else if (tx != NULL)
   {
      HAL_SPI_Transmit(&spi, tx, len, HAL_MAX_DELAY);
   }
   else if (rx != NULL)
   {
      HAL_SPI_Receive(&spi, rx, len, HAL_MAX_DELAY);
   }

   if (tx != NULL || rx != NULL)
   {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, SET);
   }
}
