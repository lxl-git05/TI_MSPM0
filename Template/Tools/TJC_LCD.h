#ifndef __TJC_LCD_H
#define __TJC_LCD_H

#include "MySystem.h"

// ============== 移植配置（#ifndef 允许在包含前覆盖）==============
// 示例: #define TJC_LCD_SERIAL Serial2   // 改用Serial2
//       #include "TJC_LCD.h"

#ifndef TJC_LCD_SERIAL
#define TJC_LCD_SERIAL       Serial4             // 使用的串口实例
#endif

#ifndef TJC_LCD_KEY_MAX
#define TJC_LCD_KEY_MAX      6                   // 虚拟按键数量上限
#endif

#ifndef TJC_LCD_PARAM_MAX
#define TJC_LCD_PARAM_MAX    4                   // 滑块参数数量上限
#endif

#ifndef TJC_LCD_BUF_SIZE
#define TJC_LCD_BUF_SIZE     40                  // 接收缓冲区大小
#endif

// 协议关键字前缀（需与LCD端页面设计保持一致）
#define TJC_LCD_KEY_KEYWORD    "LCD_KEY"         // 按键消息前缀
#define TJC_LCD_PARAM_KEYWORD  "LCD_Param"       // 滑块消息前缀

// ============== 虚拟按键号宏定义 ==============
#define LCD_KEY_1   1
#define LCD_KEY_2   2
#define LCD_KEY_3   3
#define LCD_KEY_4   4
#define LCD_KEY_5   5
#define LCD_KEY_6   6
// 若 TJC_LCD_KEY_MAX > 6，用户可自行追加

// ============== 公开 API ==============
void TJC_LCD_Init(void);
void TJC_LCD_Process(void);                                            // 主循环调用：解析 ABC 数据
bool LCD_Key_Pressed(uint8_t key_num);                                 // 检测虚拟按键（1~TJC_LCD_KEY_MAX），读后清零
bool LCD_Param_Set(uint8_t param_id, int *var, int min, int max);      // 滑块→整数映射，true=已更新
bool LCD_Param_Set_Float(uint8_t param_id, float *var, float min, float max); // 滑块→浮点映射

#endif
