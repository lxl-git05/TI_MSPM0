#ifndef __STATUS_STACK_H
#define __STATUS_STACK_H

#include "Control.h"

#define STATUS_STACK_SIZE 50

typedef struct
{
    Car_Status_Typedef data[STATUS_STACK_SIZE];
    int top;
}StatusStack_Typedef;

// 初始化
void StatusStack_Init(StatusStack_Typedef *stack);

// 入栈
bool StatusStack_Push(StatusStack_Typedef *stack,Car_Status_Typedef value);

// 出栈
bool StatusStack_Pop(StatusStack_Typedef *stack,Car_Status_Typedef *value);

// 查看栈顶
Car_Status_Typedef StatusStack_Peek(StatusStack_Typedef *stack,Car_Status_Typedef *value);

// 判空
bool StatusStack_IsEmpty(StatusStack_Typedef *stack);

// 判满
bool StatusStack_IsFull(StatusStack_Typedef *stack);

// 清空
void StatusStack_Clear(StatusStack_Typedef *stack);

// 查看栈数据总数
int StatusStack_Size(StatusStack_Typedef *stack); 

#endif