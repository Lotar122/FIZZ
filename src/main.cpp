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

template<typename T>
class SizedArray
{
public:
	T* data;
	size_t size;

	void destroy() { free(data); };
	SizedArray(void* data, size_t size) : data(data), size(size) {};
};

void initializeTimer3ForServos(uint16_t initialStateS1 = 1500, uint16_t initialStateS2 = 1500, uint16_t initialStateS3 = 1500, uint16_t initialStateS4 = 1500)
{
	// Enable GPIOA and GPIOB clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;

    // Enable TIM3 clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

    // --- Configure GPIO pins ---
    // PA6 -> TIM3_CH1, PA7 -> TIM3_CH2
    GPIOA->MODER &= ~((3<<12) | (3<<14)); // Clear mode
    GPIOA->MODER |= ((2<<12) | (2<<14));  // AF mode

    GPIOA->AFR[0] &= ~((0xF<<24) | (0xF<<28));
    GPIOA->AFR[0] |= ((2<<24) | (2<<28)); // AF2 TIM3

    // PB0 -> TIM3_CH3, PB1 -> TIM3_CH4
    GPIOB->MODER &= ~((3<<0) | (3<<2));
    GPIOB->MODER |= ((2<<0) | (2<<2));    // AF mode

    GPIOB->AFR[0] &= ~((0xF<<0) | (0xF<<4));
    GPIOB->AFR[0] |= ((2<<0) | (2<<4));   // AF2 TIM3

    // --- Configure TIM3 ---
    TIM3->PSC = (SystemCoreClock / 1000000) - 1; // 1 MHz timer
    TIM3->ARR = 20000 - 1;                        // 20 ms period (50 Hz)

    // CH1–CH4 PWM mode 1
    TIM3->CCMR1 = (6<<4) | (6<<12);  // OC1M=110, OC2M=110 PWM mode 1
    TIM3->CCMR2 = (6<<4) | (6<<12);  // OC3M=110, OC4M=110 PWM mode 1

    // Enable outputs
    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;

    // Set initial duty cycles (pulse width in µs)
    TIM3->CCR1 = initialStateS1;
    TIM3->CCR2 = initialStateS2;
    TIM3->CCR3 = initialStateS3;
    TIM3->CCR4 = initialStateS4;

    // Enable counter
    TIM3->CR1 |= TIM_CR1_CEN;
}

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