#ifndef __ALLHEADER_H
#define __ALLHEADER_H

#include "MySystem.h"

// System层

// Hardware层
#include "Key.h"
#include "OLED.h"
#include "Buzzer.h"
#include "Elec.h"
#include "RGB.h"

// Hardware层
#include "MyPID.h"

// tools层
#include "LED_Flash.h"
#include "Timer_Counter.h"

// app层

// function层
#include "MPU6050_Angle.h"

// Mode层
#include "Mode_G.h"
#include "Mode_1.h"
#include "Mode_2.h"
#include "Mode_3.h"

// ================== 初始化函数 ==================
void Initial_All(void) ;

#endif
