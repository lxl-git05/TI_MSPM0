#include "Mode_G.h"
#include "AllHeader.h"

Mode_Typedef curr_mode = Mode_Null   ;       // 当前模式
Mode_Typedef next_mode = Mode_2      ;        // 下一个模式

// ========================== 系统setup loop ==========================

// 初始化
void Mode_G_Setup(void)
{
    // 全局初始化
    Initial_All() ;
    // 定时器必须最后初始化!!!
    Timer_Initial() ;
    // 参数注册:只需要第一次注册即可
    // PARAM_FORCE(curr_mode, Mode_1);
    // PARAM_FORCE(IMU_Mahony_GyroBiasX, -9.26840305f);
    // PARAM_FORCE(IMU_Mahony_GyroBiasY, 0.428176761f);
    // PARAM_FORCE(IMU_Mahony_GyroBiasZ, -1.27142811f);
    
    // 外存初始化会导致模式直接切换，跳过setup，所以这里进行更新处理，使得可以进入setup
    if (curr_mode > Mode_Null && curr_mode < Mode_End)
    {
        next_mode = curr_mode;
        curr_mode = Mode_Null;
    }
    
}

// 循环loop — 全局按键 + 模式分发
void Mode_G_Loop(void)
{
    // ===== 全局按键（所有模式共享）=====
    // Key2 单击: 切换 LED 闪烁模式
    if (Key_Check(KEY_0, KEY_SINGLE))
    {
        Flash_Mode_Set(Flash_Mode_Fast) ;
    }
    // Key2 双击: 进入下一个模式
    if (Key_Check(KEY_0, KEY_DOUBLE))
    {
        Mode_To_Next() ;
    }
    // 全局模式展示
    if (curr_mode == Mode_Null)
    {
        OLED_Printf(0,0,OLED_6X8,"===Mode_G===") ;
    }
}

// ========================== 系统定时器配置 ==========================

// 1ms定时器
void Timer_1ms_Callback(void)
{
    // 功能1:按键
    Key_Tick() ;
    // 功能2:LED闪烁监控
    Flash_Mode_Tick() ;
    // 功能3:步进电机速度ramp（加速度渐进）
    Stepper_PWM_Speed_Tick(&Stepper1);
    Stepper_PWM_Speed_Tick(&Stepper2);
    // 功能4:步进电机位控状态机（T型/三角形速度曲线）
    Stepper_PWM_Pos_Tick(&Stepper1);
    Stepper_PWM_Pos_Tick(&Stepper2);
}

// 20ms定时器
void Timer_20ms_Callback(void)
{
    // 0. 任务系统
    Con_Task_Tick();
    // 1. 陀螺仪更新
    IMU_Mahony_Update_Tick();
    // 2. 香橙派数据更新
    Oran_Update() ;
    // 3. 蓝牙数据更新
    // BLE_Update() ;
    // 当前模式的 Tick
    switch (curr_mode)
    {
        case Mode_1: Mode_1_Tick(); break;
        case Mode_2: Mode_2_Tick(); break;
        case Mode_3: Mode_3_Tick(); break;
        case Mode_4: Mode_4_Tick(); break;
        case Mode_5: Mode_5_Tick(); break;
        case Mode_6: Mode_6_Tick(); break;
        case Con_Mode_1: Con_Mode_1_Tick(); break;
        case Con_Mode_2: Con_Mode_2_Tick(); break;
        case Con_Mode_3: Con_Mode_3_Tick(); break;
        case Con_Mode_4: Con_Mode_4_Tick(); break;
        case Con_Mode_5: Con_Mode_5_Tick(); break;
        case Con_Mode_6: Con_Mode_6_Tick(); break;
        default: break;
    }
    // 电机速度内环最后驱动
    Motor_Speed_Update_Tick(20);    
}

// ========================== 系统状态配置 ==========================
// 进入下一状态
void Mode_To_Next(void)
{
    // Mode_End纯属标记模式尽头防止越界
    uint32_t next_val = (uint32_t)next_mode + 1;
    next_mode = (next_val >= (uint32_t)Mode_End) ? Mode_Null : (Mode_Typedef)next_val;
    // ★ 模式记忆由 empty.c 在 curr_mode = next_mode 后统一保存
}

// 将当前状态转换为:
void Mode_ChangeTo(Mode_Typedef nextmode)
{
    if (nextmode >= Mode_End) { return;}

    next_mode = nextmode ;
}

// ====================================================================
// GROUP1 中断总入口 — 所有GPIO端口(GPIOA/GPIOB等)共享此中断向量
// 通过 IIDX 区分具体是哪个GPIO端口触发的中断
// ====================================================================
void GROUP1_IRQHandler(void)
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        // GPIOB端口中断 — 编码器A/B电机测速
        case GPIO_MULTIPLE_GPIOB_INT_IIDX:
            MyEncoder_ISR(&Motor_A_Encoder);
            MyEncoder_ISR(&Motor_B_Encoder);
            break;

        // GPIOA端口中断 — EC11 旋转编码器
        case DL_INTERRUPT_GROUP1_IIDX_GPIOA:
            EC11_Encoder_ISR();
            break;

        // 其他GROUP1中断源在此扩展...

        default:
            break;
    }
}

// ====================================================================
// TIMG6 脉冲中断 — 步进电机1（Stepper1）
// 每个 PWM 周期（脉冲）触发一次，用于步数累加 + 到位停止
// ====================================================================
void TIMG6_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(PWM_Stepper1_INST))
    {
        case DL_TIMER_IIDX_ZERO:
            Stepper_PWM_Pulse_Count(&Stepper1);
            DL_TimerG_clearInterruptStatus(PWM_Stepper1_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
            break;
        default:
            break;
    }
}

// ====================================================================
// TIMG7 脉冲中断 — 步进电机2（Stepper2）
// ====================================================================
void TIMG7_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(PWM_Stepper2_INST))
    {
        case DL_TIMER_IIDX_ZERO:
            Stepper_PWM_Pulse_Count(&Stepper2);
            DL_TimerG_clearInterruptStatus(PWM_Stepper2_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
            break;
        default:
            break;
    }
}
