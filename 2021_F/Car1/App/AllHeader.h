#ifndef __ALLHEADER_H
#define __ALLHEADER_H

#include "ti_msp_dl_config.h"

// System层
#include "Tim.h"
#include "PWM.h"
#include "Serial.h"
#include "Encoder.h"

// Hardware层
#include "Key.h"
#include "OLED.h"
#include "Y8_Track.h"
#include "Motor.h"

// tools层
#include "LED_Flash.h"
#include "Key_Check.h"

// app层
#include "Menu_Key.h"

// function层
#include "Con_Motor.h"

// ================== 初始化函数 ==================
void Initial_All(void) ;

// 定时器初始化必须在最后面
void Timer_Init(void) ;

#endif
