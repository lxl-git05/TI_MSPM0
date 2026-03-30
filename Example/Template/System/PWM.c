#include "PWM.h"

// ================= 底层PWM =================

// PWM初始化
// void PWM_Init(TIM_HandleTypeDef htimx , uint32_t Channel)
// {
// 	HAL_TIM_PWM_Start(&htimx , Channel) ;
//     不需要
// }

// 设置PWM值,自己在上层加上限幅
// 例: DL_TimerG_setCaptureCompareValue(PWM_LED_INST,PWM_val,GPIO_PWM_LED_C1_IDX);
void PWM_SetCompare1(GPTIMER_Regs *gptimer, uint32_t value, DL_TIMER_CC_INDEX ccIndex)
{
	// 设置LED的占空比,LED_PWM需要在0-ARR之间
	DL_TimerG_setCaptureCompareValue(gptimer,value,ccIndex);
}
