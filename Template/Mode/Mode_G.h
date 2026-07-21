#ifndef __MODE_G_H
#define __MODE_G_H

typedef enum
{
    Mode_Null = 0U ,
    // 加模式在下面加即可
    Mode_1 ,      // 模式1:保留
    Mode_2 ,      // 模式2:专门测试模式
    Mode_3 ,      // 模式3:电机驱动模式
    Mode_End ,      // 注册模式不能在这个下面!!!
}Mode_Typedef;

extern Mode_Typedef curr_mode ;    // 当前模式
extern Mode_Typedef next_mode ;    // 下一个模式

void Mode_To_Next(void) ;
void Mode_ChangeTo(Mode_Typedef nextmode) ;

void Mode_G_Setup(void) ;
void Mode_G_Loop(void) ;

#endif
