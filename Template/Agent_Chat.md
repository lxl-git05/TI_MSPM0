# 1. 库移植

todo:

+ [ ] 首先读取和记忆E:\TI\TI_MSPM0\F407内的所有文件，作为库移植的参考
+ [ ] 记忆：
  + [ ] 进行测试实验只能在Mode2写代码，并且需要尽量简洁，验证可行性即可
  + [ ] 需要人参与的代码测试尽量使用Key1和2，单击双击，Key0不要使用，我已经用在别处了
  + [ ] todo完成可以打钩
+ [ ] 参考MySystem，重构GPIO和Timer的模板工程，空引脚先进行定义，然后结构体为空
+ [ ] 实现OLED和Key以及FLash的移植，并且写一个测试
+ [ ] 后续我再补充

+++

记住：所有需要修改的库函数，首先去F407找，能有的就直接进行参考和复制，并且使用MySystem进行移植

+++

接着

+ [ ] 进行Mode的移植，再加入3个Mode模式
+ [ ] 进行Serial的移植，与我进行探讨，关于DMA和Serial硬件配置等问题，然后选定移植方案，再进行移植

+++

 Timer_Counter我想要像STM32一样使用DWT，但是我发现没有，那么能不能使用定时器进行1us计数，并且进行溢出判断，实现全局时钟，达成同样的效果呢

+++

接着实现电机的PWM的移植，也就是首先配置Mysystem的PWM，我检查一下，移植Motor.c .h，并且写一个例程，纯按键控制PWM，我看看效果，再考虑下一步的编码器移植

# 2. ICM42688移植

首先读取本工程的MPU6050的实现逻辑，由于ICM42688和MPU6050的引脚基本一样，都是使用I2C驱动，我打算使用同样的硬件I2C进行驱动，那么首先你需要

+ [ ] 读取本工程，明确只在Mode2和ICM_42688_base.c / .h里面进行代码编写
+ [ ] 读取示例文件E:\TI\TI_MSPM0\F407\New，进行代码编写计划
+ [ ] API尽量相同
+ [ ] MPU6050也进行适当的跟随移植
+ [ ] 最终实现的效果就是TI和STM32的API和结构体基本一样
+ [ ] 开始计划

+++

去到D:\github\HUST_STI\HUST_STI_1\Templete\Templete_STM32F407ZGT6\Template_F407ZGT6\ICM42688_Portable_Lib，里面有所有最新的驱动和滤波代码，不需要加入互补滤波，直接加入M滤波和底层驱动即可，加入的就查看有没有需要更改的，然后去到Mode2实现一个例程，要求是和MPU6050的引脚基本一样，都是使用I2C驱动

+++

首先去到D:\github\HUST_STI\HUST_STI_1\Templete\Templete_STM32F407ZGT6\Template_F407ZGT6\IMU_Portable_Lib，阅读Readme，明确MPU6050的M滤波思路，然后在本工程也新建这个库，并且最后导入IMU.c / .h库，删除Type库，并将本工程目前所有涉及陀螺仪的部分都替换成IMU的函数，实现陀螺仪底层解耦，也就是不再关心底层是MPU还是ICM



# 3. AT外存移植

首先明确：只能在Mode2和bsp_at24c02.c / .h进行代码编写

可以参考MyI2C库，进行硬件IIC驱动AT外存模块，不使用软件驱动

你需要：

+ [ ] 参考目前加入的bsp_at24c02的STM32硬件IIC实现方案，进行TI的硬件IIC底层的编写
+ [ ] 暂时删去本文件bsp_at24c02软件IIC的所有内容
+ [ ] 在Mode2进行简单读写实验

+++

+ 现在去到D:\github\HUST_STI\HUST_STI_1\Templete\Templete_STM32F407ZGT6\Template_F407ZGT6

+ [ ] 读取Con_Mode_1的全局表，在本工程暂时也这样配置
+ [ ] 实现Param_AT24C02、at24c02_manager、ParamEdit、Encoder_Key的编写
+ [ ] 先进行移植计划



# 4. 模式队列

参考E:\TI\TI_MSPM0\2021_F\Car2的小车旋转固定角度任务，去到本工程的Control，添加// 6. 任务6: 小车顺时针/逆时针旋转一定角度然后Exit

+++

+ 首先去到我的Con_Motor，目前Motor有速度环和角度环，但是我还想的是实现一个位置环，其实和角度环差不多，只是旋转圈数 * 周长，要求是
  + 实现和Angle环基本相同的函数API，输入是目标移动距离(cm)(goalPoint)
  + 初始化解注释
  + 可以利用和改造被注释或者没有用上的Con_Motor内的函数
  + 在Mode3的1进行实验，要和Angle一样的调节PID环节

+++

+ 添加任务
  + 小车向前走 x cm(编码器相对值，可以取两个电机向前运动的平均值作为向前的距离参考)(使用陀螺仪保持走直线)，之后停下,如果输入的距离接近0（输入float 0)，那就是一直直走直到后续外部进行任务强制结束

+++

现在我的任务系统已经包含相当多的任务，我想尝试建立一个开环系统，实现编码器+IMU的位置定位，具体接口函数是输入x,y的cm float值，实现小车首先转向(IMU转向)，然后去到x,y，也就是去到新的(x,y)，首先转向对准，然后计算直线行驶的距离，最后完成任务，那么你现在可以建立这么一个库，然后写好4大函数，，然后放入任务队列表，再在Mode2进行实验，我看看效果如何，以及我们讨论一下基于现有IMU和编码器，怎么实现小车感知自己的绝对位置和方向，找到最优的全图定位和导航算法











```c
/*
	1. 初始化
	2. 丢位置(开始旋转,直到找到位置)
    2. 定方向(目标的大致方位,然后直行)
    3. 前进
    4. 精细搬运(目标的具体方位)
*/



// 初始化完成：
// Item_Goal[4]
if ()
{
    if (丢画)
    {
        // call Orange 找位置
    }
    else	// 没丢画
    {
        
    }
}
else
{
    // 开始回家
}
```



# 5. Y8寻迹

+ 

+ 
+ 

首先去到





# 6. LCD调参

我现在在准备使用TJC_LCD进行调参，使用Serial4检测是否能接收到消息，但是现在只要串口屏发送消息就会进入

```c
/* This is the code that gets called when the processor receives an unexpected  */
/* interrupt.  This simply enters an infinite loop, preserving the system state */
/* for examination by a debugger.                                               */
void Default_Handler(void)
{
    /* Enter an infinite loop. */
    while (1) {
    }
}

```

为什么会这样

+++

现在我建立了一个TJC_LCD库，用来进行调试，目前有两种信息：

+ 按键信息：按下松开后会发送：`@LCD_KEY_X$#` , X取1-6
+ 滑块信息：滑块滑动后会发送：`@LCD_Param_1=%d$#`，%d取0-100

那么需要的伪代码

```c
// 1. LCD虚拟按键
if (LCD_Key(1-6,使用宏定义))
{
    // 业务逻辑
}
// 2. 滑块模块
LCD_Param(&变量名,min值,max值)	// 实现变量赋值,滑块发送的是0-100，那么min->max需要进行映射，如果是整数就取整数，小数就正常映射
```

+ 具体Serial4解析其实就是以下函数：

```c
// ============== ABC协议 ==============
uint8_t Serial_GetNewPackageFlag_ABC(Serial_Typedef *pSerial);
int Serial_GetError_ABC(Serial_Typedef *pSerial);
bool Serial_SetFloatData(Serial_Typedef *pSerial, char *KeyWord, char *cmd, float *Data);
bool Serial_SetIntData(Serial_Typedef *pSerial, char *KeyWord, char *cmd, int *Data);
bool Serial_Check_Str(Serial_Typedef *pSerial, char *KeyWord);     // 子串匹配（strstr）
bool Serial_CheckCmd(Serial_Typedef *pSerial, char *cmd);          // 精确匹配（strcmp，借鉴待移植库）
```

+ 开始进行计划，可以否认我的API，也可以提出改进意见，包括LCD端和TI板子端，实验代码写在Mode2，库写在TJC_LCD.c/.h里面

+++

加入宏定义：包含串口号、按键个数、滑块个数等，方便后续移植



































































