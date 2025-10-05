#include "readPin.h"

uint32_t readPin(GPIO_TypeDef* gpio, uint32_t pin)
{
	return ((1UL << pin) & gpio->IDR) >> pin;
}
