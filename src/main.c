#include "stm32f4xx.h"
#include "stm32f4xx_nucleo.h"

#include "spi.h"

#define INIT      (0)
#define TOP       (1)
#define BOTTOM    (2)
#define LEFT      (3)
#define RIGHT     (4)

#define DAC_A     (0x00)
#define DAC_B     (0x80)
#define DAC_BUF   (0x40)
#define DAC_NGA   (0x20)
#define DAC_NSHDN (0x10)

#define DAC_MAX   (4095)
#define DAC_MIN   (0)

typedef struct POINT_T
{
   float x;
   float y;
} POINT_T;

POINT_T points[4];

int pointIndex = 0;

float scale = 1.0;
float scaleStep = 0.01;

void TIM2_IRQHandler(void);

TIM_HandleTypeDef timerHandle;

uint16_t x, y;

int main(void)
{
    HAL_Init();

    BSP_LED_Init(LED2);

    spi_setup();

    __HAL_RCC_TIM2_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM2_IRQn, TICK_INT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    // Set Timer 2 to a frequency of 1 MHz
    uint32_t timerFreq = 1000000;

    // Use a interrupt frequency
    uint32_t interruptFreq = 2000;

    timerHandle.Instance = TIM2;
    timerHandle.Init.Prescaler = (SystemCoreClock / timerFreq) - 1;
    timerHandle.Init.Period =  (timerFreq / interruptFreq) - 1;
    timerHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    timerHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    HAL_TIM_Base_Init(&timerHandle);
    HAL_TIM_Base_Start_IT(&timerHandle);

    points[0].x = -1.0;
    points[0].y = -1.0;

    points[1].x = 1.0;
    points[1].y = -1.0;

    points[2].x = 1.0;
    points[2].y = 1.0;

    points[3].x = -1.0;
    points[3].y = 1.0;

    while (1)
    {
       HAL_Delay(25);

       scale += scaleStep;

       if (scale > 1.0)
       {
          scale = 1.0;
          scaleStep *= -1.0;
       }

       if (scale < 0.0)
       {
          scale = 0.0;
          scaleStep *= -1.0;
       }
    }
}

void TIM2_IRQHandler(void)
{
   HAL_TIM_IRQHandler(&timerHandle);
}

uint8_t dacCmd[2];

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
   POINT_T p = points[pointIndex++];

   if (pointIndex == 4) pointIndex = 0;

   uint32_t x = ((((p.x * scale) / 2.0) + 0.5)) * DAC_MAX;
   uint32_t y = ((((p.y * scale) / 2.0) + 0.5)) * DAC_MAX;

   dacCmd[0] = DAC_A | DAC_NGA | DAC_NSHDN | (0x0F & (x >> 8));
   dacCmd[1] = 0xFF & x;
   spi_transfer(dacCmd, NULL, 2);

   dacCmd[0] = DAC_B | DAC_NGA | DAC_NSHDN | (0x0F & (y >> 8));
   dacCmd[1] = 0xFF & y;
   spi_transfer(dacCmd, NULL, 2);
}
