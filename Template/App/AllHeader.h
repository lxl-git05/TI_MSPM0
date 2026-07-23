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
#include "Encoder_Key.h"
#include "Stepper_PWM.h"
#include "Orange.h"

// Hardware层
#include "MyPID.h"
#include "ICM_42688_base.h"
#include "ICM42688_Mahony.h"
#include "Imu_Types.h"


// tools层
#include "LED_Flash.h"
#include "Timer_Counter.h"

// app层

// function层
#include "Serial_porting.h"
#include "Control.h"
#include "Con_Motor.h"
#include "Con_Stepper.h"
#include "Con_Task.h"

// 脱机阈值功能
#include "ParamEdit.h"
#include "Param_AT24C02.h"
#include "at24c02_manager.h"

// Mode层
#include "Mode_G.h"
#include "Mode_1.h"
#include "Mode_2.h"
#include "Mode_3.h"
#include "Mode_4.h"
#include "Mode_5.h"
#include "Mode_6.h"
#include "Con_Mode_1.h"
#include "Con_Mode_2.h"
#include "Con_Mode_3.h"
#include "Con_Mode_4.h"
#include "Con_Mode_5.h"
#include "Con_Mode_6.h"

// ================== 初始化函数 ==================
void Initial_All(void) ;

#endif
