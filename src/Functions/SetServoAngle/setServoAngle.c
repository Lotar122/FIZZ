#include "setServoAngle.h"

void servoSetAngle(int deg, TIM_TypeDef* TIMx, uint8_t channel) 
{
    // Map 0-180° to 1000-2000us
    int pulse = 500 + (deg * 2000) / 180;
    
	switch(channel)
	{
		case 1:
			TIMx->CCR1 = pulse;
			break;
		case 2:
			TIMx->CCR2 = pulse;
			break;
		case 3:
			TIMx->CCR3 = pulse;
			break;
		case 4:
			TIMx->CCR4 = pulse;
			break;
	}
}