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
#include "MPU6050_base.h"

// tools层
#include "LED_Flash.h"
#include "Key_Check.h"

// app层
#include "Menu_Key.h"

// function层
#include "Con_Motor.h"

// Mode层
#include "Mode_G.h"
#include "Mode_1.h"
#include "Mode_2.h"
#include "Mode_3.h"

// ================== 初始化函数 ==================
void Initial_All(void) ;

// 定时器初始化必须在最后面
void Timer_Init(void) ;

#endif
