#include "main.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "MI1640.h"


void mi48ConfigFullFrame(void)
{
    uint8_t buf[2];
    buf[0] = 0xb1; 
    // 0x00 代表：Full-Frame Readout (Bit 2-4 = 0) + 包含 Header (Bit 5 = 0)
    buf[1] = 0x00;
    HAL_I2C_Master_Transmit(&hi2c1, 0x40<<1, buf, 2, osWaitForever);
}

void mi48Reset(void)
{
	HAL_GPIO_WritePin(MI48_RST_GPIO_Port, MI48_RST_Pin, GPIO_PIN_RESET);
	osDelay(100);
	HAL_GPIO_WritePin(MI48_RST_GPIO_Port, MI48_RST_Pin, GPIO_PIN_SET);
	osDelay(1000);
}

void mi48EnbleTemporalFilter(void)
{
	uint8_t buf[16];

	buf[0] = 0xd0;
	buf[1] = 0x0b;
	HAL_I2C_Master_Transmit(&hi2c1, 0x40<<1, buf, 2, osWaitForever);

	osDelay(1000);
}


void mi48SetFrameRateDivisor(uint8_t framerateDivisor)
{
	uint8_t buf[16];

	buf[0] = 0xb4;
	buf[1] = framerateDivisor;
	HAL_I2C_Master_Transmit(&hi2c1, 0x40<<1, buf, 2, osWaitForever);
}

void mi48StartContinuousCapture(void)
{
	uint8_t buf[16];

	buf[0] = 0xb1;
	buf[1] = 0x02;
	HAL_I2C_Master_Transmit(&hi2c1, 0x40<<1, buf, 2, osWaitForever);
}






