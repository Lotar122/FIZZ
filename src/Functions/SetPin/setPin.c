#include "setPin.h"

void setPin(GPIO_TypeDef* gpio, uint32_t pin, uint32_t value)
{
	gpio->ODR &= ~(1UL << pin);
	gpio->ODR |= (value << pin);
}
