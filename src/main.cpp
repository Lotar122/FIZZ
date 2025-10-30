#include <cstdint>
extern "C" {

#include "stm32l476xx.h"

#include "Functions/SysTickInit/SysTickInit.h"
#include "Functions/Delay/delay.h"
#include "Functions/ReadPin/readPin.h"
#include "Functions/SetPin/setPin.h"
#include "Heap/Heap.h"
#include "Functions/SetServoAngle/setServoAngle.h"

}

#include "Functions/InitializeTimerForServos/initializeTimerForServos.hpp"

volatile uint32_t stepCount1 = 0;
volatile uint32_t stepsToSend1 = 50;
volatile bool free1 = false;

extern "C" void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;

        if (++stepCount1 >= stepsToSend1) {
            TIM3->CR1 &= ~TIM_CR1_CEN; // stop timer
			free1 = true;
        }
    }
}

void initializeTimer3ForSteppers(void) {
    // 1. Enable clocks
    RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOAEN;  // GPIOA clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;  // TIM3 clock

    // 2. Configure PA6 as alternate function (AF2 = TIM3_CH1)
    GPIOA->MODER &= ~(3U << (6 * 2));
    GPIOA->MODER |=  (2U << (6 * 2)); // AF mode
    GPIOA->AFR[0] &= ~(0xFU << (6 * 4));
    GPIOA->AFR[0] |=  (2U  << (6 * 4)); // AF2 for TIM3_CH1

    // 3. Configure timer base (assume APB1 = 80 MHz)
    TIM3->CR1 = 0;                  // upcounting, no preload
    TIM3->PSC = (SystemCoreClock / 1000000) - 1;// prescaler: 1 MHz timer clock
    TIM3->ARR = 2000 - 1;           // period = 500Hz PWM frequency
    TIM3->CCR1 = 500;               // 50% duty cycle

    // 4. Configure PWM mode 1 on CH1
    TIM3->CCMR1 &= ~(TIM_CCMR1_OC1M);
    TIM3->CCMR1 |= (6U << TIM_CCMR1_OC1M_Pos); // PWM1 mode
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;            // preload enable
    TIM3->CCER  |= TIM_CCER_CC1E;              // enable output

    // 5. Enable update interrupt
    TIM3->DIER |= TIM_DIER_UIE;

    // 6. Enable interrupt in NVIC (CMSIS style)
    NVIC->ISER[0] = (1U << (TIM3_IRQn & 0x1F));

    // 7. Start timer
    TIM3->EGR |= TIM_EGR_UG; // force update event
    // TIM3->CR1 |= TIM_CR1_CEN;
}

void turnStepper(uint32_t angle)
{
	uint32_t steps = angle / 1.8f;

	while(!free1) { __asm("nop"); };

	delay(2000);

	stepsToSend1 = steps;
	stepCount1 = 0;
	TIM3->CR1 |= TIM_CR1_CEN;
}

extern "C" void EXTI9_5_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF8)
    {
        EXTI->PR1 = EXTI_PR1_PIF8; // clear pending
        // Handle PB8 interrupt
		//setPin(GPIOA, 5, ~readPin(GPIOA, 5));
		turnStepper(90);
    }

    if (EXTI->PR1 & EXTI_PR1_PIF9)
    {
        EXTI->PR1 |= EXTI_PR1_PIF9; // clear pending
        // Handle PB9 interrupt
		//setPin(GPIOA, 5, ~readPin(GPIOA, 5));
		setPin(GPIOA, 5, readPin(GPIOA, 5) ? 0 : 1);
    }
}

int main()
{
	//Enable FPU
	SCB->CPACR |= (0xF << 20);

	__asm volatile ("dsb");
    __asm volatile ("isb");

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
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	GPIOB->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);

	SYSCFG->EXTICR[2] &= ~(SYSCFG_EXTICR3_EXTI8 | SYSCFG_EXTICR3_EXTI9);
    SYSCFG->EXTICR[2] |=  (SYSCFG_EXTICR3_EXTI8_PB | SYSCFG_EXTICR3_EXTI9_PB);

	EXTI->RTSR1 |= (EXTI_RTSR1_RT8 | EXTI_RTSR1_RT9);
    EXTI->FTSR1 &= ~(EXTI_FTSR1_FT8 | EXTI_FTSR1_FT9);

	EXTI->IMR1 |= (EXTI_IMR1_IM8 | EXTI_IMR1_IM9);

	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);   // clear bits
    GPIOB->PUPDR |=  ((1U << (8*2)) | (1U << (9*2)));         // 01: pull-up

	// 7. Enable EXTI9_5 interrupt in NVIC manually
    //    EXTI9_5_IRQn = 23 (see STM32L476 reference manual, Table 69)
    constexpr uint32_t irq_num = 23;
    constexpr uint32_t reg_index = irq_num / 32;    // 0 for 0–31
    constexpr uint32_t bit_pos   = irq_num % 32;

    NVIC->ISER[reg_index] = (1U << bit_pos);       // Enable interrupt
    // Optional: Set priority (each register holds 4 priority bytes)
    NVIC->IPR[irq_num] = 0x40;                      // Lower value = higher priority

	GPIOC->MODER &= ~(0x3 << (7 * 2));      // Clear mode bits
    GPIOC->MODER |=  (0x2 << (7 * 2));      // Set to AF mode

	GPIOA->MODER &= ~(2UL << (5 * 2));
	GPIOA->MODER &= ~(2UL << (9 * 2));

	setPin(GPIOA, 7, 0);

	initializeTimer3ForSteppers();

	uint8_t currentTrend = 1;
	int32_t angle = 0;
	while(1)
	{
		delay(1000);
		turnStepper(118000);
	}
}