extern "C" {

#include "stm32l476xx.h"

#include "Functions/SysTickInit/SysTickInit.h"
#include "Functions/Delay/delay.h"
#include "Functions/ReadPin/readPin.h"
#include "Functions/SetPin/setPin.h"
#include "Heap/Heap.h"

void servoSetAngle(int deg, uint8_t channel) 
{
    // Map 0-180° to 1000-2000us
    int pulse = 500 + (deg * 2000) / 180;
    
	switch(channel)
	{
		case 1:
			TIM3->CCR1 = pulse;
			break;
		case 2:
			TIM3->CCR2 = pulse;
			break;
		case 3:
			TIM3->CCR3 = pulse;
			break;
		case 4:
			TIM3->CCR4 = pulse;
			break;
	}
}

}

#include "Functions/InitializeTimerForServos/initializeTimerForServos.hpp"

template<typename T>
class SizedArray
{
public:
	T* data;
	size_t size;

	void destroy() { free(data); };
	SizedArray(void* data, size_t size) : data(data), size(size) {};
};

int main()
{
	// 1. Enable HSI (16 MHz internal RC)
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)); // Wait until ready

    // 2. Switch system clock (SYSCLK) to HSI
    RCC->CFGR &= ~RCC_CFGR_SW;          // Clear
    RCC->CFGR |= RCC_CFGR_SW_HSI;       // Select HSI
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

    // 3. Set Prescalers to 1 (no division)
    RCC->CFGR &= ~RCC_CFGR_HPRE;        // AHB = SYSCLK
    RCC->CFGR &= ~RCC_CFGR_PPRE1;       // APB1 = AHB
    RCC->CFGR &= ~RCC_CFGR_PPRE2;       // APB2 = AHB

    // 4. Update SystemCoreClock (needed for libraries or your own tracking)
    SystemCoreClock = 16000000;

    SysTickInit(SystemCoreClock / 1000);

	// 1. Enable GPIOA clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	GPIOC->MODER &= ~(0x3 << (7 * 2));      // Clear mode bits
    GPIOC->MODER |=  (0x2 << (7 * 2));      // Set to AF mode

	GPIOA->MODER &= ~(2UL << (5 * 2));

	initializeTimer3ForServos();

	delay(3000);

	uint8_t currentTrend = 1;
	int32_t angle = 0;
	while(1)
	{
		uint32_t currentValue = readPin(GPIOA, 5);
		if(currentValue) currentValue = 0;
		else currentValue = 1;
		setPin(GPIOA, 5, currentValue);

		servoSetAngle(0, 1);
		servoSetAngle(45, 2);
		delay(1000);
		
		currentValue = readPin(GPIOA, 5);
		if(currentValue) currentValue = 0;
		else currentValue = 1;
		setPin(GPIOA, 5, currentValue);

		servoSetAngle(180, 1);
		servoSetAngle(135, 2);
		delay(1000);
	}
}