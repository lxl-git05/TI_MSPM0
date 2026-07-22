#ifndef __MODE_1_H
#define __MODE_1_H

#include "MySystem.h"

// 向外引出地址表
extern const AT_ParamItem s_AT_Params[] ;
extern int At_Size ;

void Mode_1_Setup(void);

void Mode_1_Loop(void);

void Mode_1_Exit(void);

void Mode_1_Tick(void);

#endif


