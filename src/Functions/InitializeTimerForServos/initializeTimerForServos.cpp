#include "initializeTimerForServos.hpp"

extern "C" {
    #include "stm32l476xx.h"
    #include "Functions/SysTickInit/SysTickInit.h"
}

void initializeTimer3ForServos(uint16_t initialStateS1, uint16_t initialStateS2, uint16_t initialStateS3, uint16_t initialStateS4)
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