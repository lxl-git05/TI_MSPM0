#ifndef __MODE_2_H
#define __MODE_2_H

#include "ti_msp_dl_config.h"
#include "MyPID.h"

extern Pid_Typedef PID_Angle ; 

void Mode_2_Setup(void);

void Mode_2_Loop(void);

void Mode_2_Exit(void);

void Mode_2_Tick(void);

#endif


