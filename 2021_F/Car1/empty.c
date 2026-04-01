#include "ti_msp_dl_config.h"
#include "AllHeader.h"

int count ;
int speed ;
int k = 0 ;

int main(void)
{
    SYSCFG_DL_init();
    Initial_All() ;    

    Encoder_Init(GPIO_MULTIPLE_GPIOB_INT_IRQN) ;
    
    while (1) 
    {
        if (Key_Check(KEY_0, KEY_SINGLE))
        {
            // 单击
            Flash_Mode_Set(Flash_Mode_Fast) ;
        }
        speed = Encoder_Get_CNT(&count) ;
        // OLED检查
        OLED_ShowNum(0,  0, k++, 3, OLED_8X16);
        // 编码器检查
        OLED_ShowSignedNum(0, 20, speed, 3, OLED_8X16);
        // Y8检查
        Y8_LineSensor_Update() ;
        OLED_ShowNum( 0, 40, Y8_Line_Array[1] , 1, OLED_8X16) ;
        OLED_ShowNum(10, 40, Y8_Line_Array[2] , 1, OLED_8X16) ;
        OLED_ShowNum(20, 40, Y8_Line_Array[3] , 1, OLED_8X16) ;
        OLED_ShowNum(30, 40, Y8_Line_Array[4] , 1, OLED_8X16) ;
        OLED_ShowNum(40, 40, Y8_Line_Array[5] , 1, OLED_8X16) ;
        OLED_ShowNum(50, 40, Y8_Line_Array[6] , 1, OLED_8X16) ;
        OLED_ShowNum(60, 40, Y8_Line_Array[7] , 1, OLED_8X16) ;
        OLED_ShowNum(70, 40, Y8_Line_Array[8] , 1, OLED_8X16) ;
        // OLED更新
        OLED_Update();
    }
}

// 1ms定时器
void Timer_0_Callback(void)
{
    // 功能1:LED闪烁
    static int cnt = 0 ;
    cnt ++ ;
    if (cnt >= 1000 -1)
    {
        cnt = 0 ;
    }

    // 功能2:按键
    Key_Tick() ;

    // 功能3:LED闪烁监控
    Flash_Mode_Tick() ;
}

void GROUP1_IRQHandler(void)
{
    DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_LED_Time_PIN) ;
    // === 必须先判断是不是 GPIOB 的中断 ===
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        
        case GPIO_MULTIPLE_GPIOB_INT_IIDX:
            Encoder_Counter_Tick(GPIO_ENCODER_A_PORT,GPIO_ENCODER_A_A_Encoder_1_PIN, GPIO_ENCODER_A_A_Encoder_2_PIN,&count);
            break;

        // 如果以后还有其他 GPIOB 的中断，可以继续加 case
        default:
            break;
    }
    DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_LED_Time_PIN) ;
}
