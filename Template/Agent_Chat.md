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



































































































