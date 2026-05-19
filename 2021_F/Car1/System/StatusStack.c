#include "StatusStack.h"
#include "ti_msp_dl_config.h"
// 初始化
void StatusStack_Init(StatusStack_Typedef *stack)
{
    stack->top = -1;
}

// 判空
bool StatusStack_IsEmpty(StatusStack_Typedef *stack)
{
    return (stack->top == -1);
}

// 判满
bool StatusStack_IsFull(StatusStack_Typedef *stack)
{
    return (stack->top >= STATUS_STACK_SIZE - 1);
}

// 清空
void StatusStack_Clear(StatusStack_Typedef *stack)
{
    stack->top = -1;
}

// 入栈
bool StatusStack_Push(
    StatusStack_Typedef *stack,
    Car_Status_Typedef value
)
{
    if (StatusStack_IsFull(stack))
    {
        return false;
    }

    stack->data[++stack->top] = value;

    return true;
}

// 出栈
bool StatusStack_Pop(
    StatusStack_Typedef *stack,
    Car_Status_Typedef *value
)
{
    if (StatusStack_IsEmpty(stack))
    {
        return false;
    }

    *value = stack->data[stack->top--];

    return true;
}

// 查看栈顶
Car_Status_Typedef StatusStack_Peek(
    StatusStack_Typedef *stack,
    Car_Status_Typedef *value
)
{
    if (StatusStack_IsEmpty(stack))
    {
        return -1;
    }

    if (value != NULL)
    {
        *value = stack->data[stack->top];
    }

    return stack->data[stack->top];
}

// 查看栈的元素数
int StatusStack_Size(StatusStack_Typedef *stack)
{
    return (stack->top + 1);
}