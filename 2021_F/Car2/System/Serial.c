#include "Serial.h"

#include "string.h"
#include <stdarg.h>
#include <stdio.h>

#ifdef Serial1_Enable
Serial_Typedef 	Serial1 ; // 串口1
#endif
#ifdef Serial2_Enable
Serial_Typedef 	Serial2 ; // 串口2
#endif
#ifdef Serial3_Enable
Serial_Typedef 	Serial3 ; // 串口3
#endif

#ifdef Serial_Debug
int Serial_check[40] ;
int Serial_Count = 0 ;
#endif

// ============== 全局变量 ==============
Serial_Agreement_HEX_TypeDef 	Serial_Agreement_HEX ;		// 串口数据通信协议:HEX
Serial_Agreement_ABC_TypeDef 	Serial_Agreement_ABC ;		// 串口数据通信协议:ABC

// ====================== 代码固定部分,不要修改(除非是Initial) ======================

// ============== 函数:初始化 ==============
// 串口协议初始化:HEX
void Serial_Agreement_HEX_Init(Serial_Agreement_HEX_TypeDef *pSerial_Agreement)
{
	pSerial_Agreement->head1 = 0xFF ;
	
	pSerial_Agreement->head2 = 0xAA ;
	pSerial_Agreement->end1	 = 0x55 ;
	
	pSerial_Agreement->end2  = 0xFE	;
}

// 串口协议初始化:ABC
void Serial_Agreement_ABC_Init(Serial_Agreement_ABC_TypeDef *pSerial_Agreement)
{
	pSerial_Agreement->head  =  '@' ;
	pSerial_Agreement->end1	 =  '$' ;
	pSerial_Agreement->end2  =  '#' ;
}

// 串口初始化:深层
static void Serial_Initial(Serial_Typedef *pSerial , UART_Regs * const uart_INST,  DMA_Regs *dma, uint8_t channelNum, uint8_t uart_int_IRQN
							, DMA_Regs *tx_dma, uint8_t tx_channelNum)
{
	// =================== 串口数据(From ST) ===================
	// 串口的数据链初始化
	pSerial->rxCnt = 0 ;
	pSerial->rx_temp = 0 ;
	memset(pSerial->rxBuf, 0, RX_Serial_LEN);	// 数据缓存区清零

	// 初始化串口协议
	Serial_Agreement_HEX_Init(&Serial_Agreement_HEX) ;
	Serial_Agreement_ABC_Init(&Serial_Agreement_ABC) ;

	// =================== pSerial的Ti配置 ===================
	pSerial->uart_INST = uart_INST ;
	pSerial->uart_int_IRQN = uart_int_IRQN ;

	// 可选(可能没有)
	pSerial->dma = dma ;
	pSerial->channelNum = channelNum ;

	/* ===== TX初始化 ===== */
	// DMA系列
	if (pSerial->dma != NULL)
	{
		pSerial->txHead = 0;
		pSerial->txTail = 0;
		pSerial->dmaBusy = 0;
		pSerial->dmaLen = 0;
		pSerial->tx_dma = tx_dma ;
		pSerial->tx_channelNum = tx_channelNum ;

		// 配置DMA相关参数:DMA名字,通道(USART_x),DMA模式(RX),DMA触发中断大小(必须>=该size才能触发中断),DMA数据接收区等
		DL_DMA_setSrcAddr(pSerial->dma, pSerial->channelNum, (uint32_t)(&pSerial->uart_INST->RXDATA));
		DL_DMA_setDestAddr(pSerial->dma, pSerial->channelNum, (uint32_t) &(pSerial->rx_temp));
		DL_DMA_setTransferSize(pSerial->dma, pSerial->channelNum, 1);
		DL_DMA_enableChannel(pSerial->dma, pSerial->channelNum);
		
		/* 开启TX DMA完成中断 */
		DL_UART_enableInterrupt(pSerial->uart_INST, DL_UART_INTERRUPT_DMA_DONE_TX);
		// 确保DMA打开
		while (false == DL_DMA_isChannelEnabled(pSerial->dma,  pSerial->channelNum)) 
		{
			__BKPT(0);
		}
	}
	// 普通中断系列
	else 
	{
		NVIC_ClearPendingIRQ(pSerial->uart_int_IRQN);	// 初始化,防止上来就进入中断
	}

    // 使能中断
    NVIC_EnableIRQ(pSerial->uart_int_IRQN);
}

// 从高8位和低8位合成一个数据
uint16_t Merge_2Bytes(uint8_t high, uint8_t low)
{
    return ((uint16_t)high << 8) | low;
}

// 串口接收数据函数---将数据收集后触发Serial_Rx_Flag的OK
Serial_RX_FLAG_Typedef Serial_Rx_State_Check(Serial_Typedef* pSerial)
{
	// 将暂存数据计入缓冲区,防止丢失
	int rxData = pSerial->rx_temp ;

	// 状态机
	// 状态1:空闲状态,等待帧头
	if (pSerial->Status == 0)
	{
		// 操作:数据记录回到原点
		pSerial->rxCnt = 0 ;
		
		// 任务:等待帧头-HEX模式
		if ( rxData == Serial_Agreement_HEX.head1 )
		{
			pSerial->rxBuf[pSerial->rxCnt++] = rxData ;
			pSerial->Status = 1 ;	// 判断HEX帧尾
			return RX_BUSY	;			// 开始处理数据
		}
		else if ( rxData == Serial_Agreement_ABC.head )
		{
			pSerial->rxBuf[pSerial->rxCnt++] = rxData ;
			pSerial->Status = 2 ;	// 判断ABC帧尾
			return RX_BUSY	;			// 开始处理数据
		}
		else
		{
			return RX_WAIT ;	// 继续等待
		}
	}
	// 开始接收HEX原始数据包
	else if (pSerial->Status == 1)
	{
		// 操作:暂存数据转移到缓冲区
		pSerial->rxBuf[pSerial->rxCnt++] = rxData ;
		
		// 任务:检测帧尾
		// 检测到帧尾,接收完毕
		if (rxData == Serial_Agreement_HEX.end2)
		{
			pSerial->Status = 0 ;	// 状态转移
			return RX_OK_HEX ;
		}
		// 没能检测到帧尾,数据溢出
		else if (pSerial->rxCnt >= Serial_Wait_Tail_MAX)
		{
			pSerial->Status = 0 ;	// 状态转移
			memset(pSerial->rxBuf, 0, RX_Serial_LEN);	// 清空
			pSerial->Hex_Data.error_Serial = Serial_Error_Tail ;
			return RX_Error_Tail_HEX ;
		}
	}
	// 开始接收ABC原始数据包
	else if (pSerial->Status == 2)
	{
		// 操作:暂存数据转移到缓冲区
		pSerial->rxBuf[pSerial->rxCnt++] = rxData ;
		
		// 任务:检测帧尾
		// 检测到帧尾,接收完毕
		if (rxData == Serial_Agreement_ABC.end2)
		{
			pSerial->Status = 0 ;	// 状态转移
			return RX_OK_ABC ;
		}
		// 没能检测到帧尾,数据溢出
		else if (pSerial->rxCnt >= Serial_Wait_Tail_MAX)
		{
			pSerial->Status = 0 ;	// 状态转移
			memset(pSerial->rxBuf, 0, RX_Serial_LEN);	// 清空
			pSerial->ABC_Data.error_Serial = Serial_Error_Tail ;
			return RX_Error_Tail_ABC ;
		}
	}
	return RX_BUSY  ;
}

// ====================HEX:初步处理数据包(仅合并数据)====================
void Serial_Data_Deal_HEX(Serial_Typedef* pSerial)
{
	// 1. 第2个数据为数据长度(第0,1个为帧头),由于是高低位,所以除以2才是真正的数据长度
	pSerial->Hex_Data.Serial_New_Package[0] = pSerial->rxBuf[2] / 2;
	// 2. 存入数据
	for (int i = 3 , j = 1 ; i < 3 + pSerial->rxBuf[2] ; i += 2 , j ++)
	{
		pSerial->Hex_Data.Serial_New_Package[j] = Merge_2Bytes(pSerial->rxBuf[i] , pSerial->rxBuf[i + 1] ) ;
	}
}	

// HEX:数据检测+存储处理函数
void Serial_Data_Check_HEX(Serial_Typedef* pSerial)
{
	// 1. 检测帧头合规性
	if (pSerial->rxBuf[0] != Serial_Agreement_HEX.head1 || pSerial->rxBuf[1] != Serial_Agreement_HEX.head2)
	{
		// 帧头不合规
		pSerial->Hex_Data.error_Serial = Serial_Error_Head;
		// 操作:
		// 清空正式数组,后续调用会显示空数据
		memset(pSerial->Hex_Data.Serial_New_Package, 0, sizeof(pSerial->Hex_Data.Serial_New_Package));
		// 存储标志位置0
		pSerial->Hex_Data.Serial_New_Package_Flag = 0 ;
	}
	// 2. 借助数据长度检测帧尾合规性
	else if (pSerial->rxBuf[pSerial->rxBuf[2] + 3] != Serial_Agreement_HEX.end1 || pSerial->rxBuf[pSerial->rxBuf[2] + 4] != Serial_Agreement_HEX.end2)
	{
		// 帧尾不合规
		pSerial->Hex_Data.error_Serial = Serial_Error_Tail ;
		// 操作:
		// 清空正式数组,后续调用会显示空数据
		memset(pSerial->Hex_Data.Serial_New_Package, 0, sizeof(pSerial->Hex_Data.Serial_New_Package));
		// 存储标志位置0
		pSerial->Hex_Data.Serial_New_Package_Flag = 0 ;
	}
	else
	{
		// 初步处理数据包(仅合并数据)
		Serial_Data_Deal_HEX(pSerial) ;
		// 无错误
		pSerial->Hex_Data.error_Serial = Serial_Error_None ;
		// 存储标志位置1
		pSerial->Hex_Data.Serial_New_Package_Flag = 1 ;
	}
}
// *HEX:得到串口接收标志位*
uint8_t Serial_GetNewPackageFlag_HEX(Serial_Typedef *pSerial)
{
	if (pSerial->Hex_Data.Serial_New_Package_Flag == 1)			//如果标志位为1
	{
		pSerial->Hex_Data.Serial_New_Package_Flag = 0;
		return 1;					//则返回1，并自动清零标志位
	}
	return 0;						//如果标志位为0，则返回0
}

// *HEX:得到错误原因*
int Serial_GetError_HEX(Serial_Typedef *pSerial)
{
	return pSerial->Hex_Data.error_Serial	 ;
}



// ====================文本:数据检测+存储+数据处理(仅保留文本)处理函数====================
void Serial_Data_Check_ABC(Serial_Typedef *pSerial)
{
	// 开始检测文本
	// 1. 检测数据包帧头是否错误
	if (pSerial->rxBuf[0] != Serial_Agreement_ABC.head)
	{
		pSerial->ABC_Data.error_Serial = Serial_Error_Head ; // *错误*:帧头不合规
	}
	// 2. 开始一边处理数据一边检测帧尾,从1开始(跳过帧头)
	else
	{
		int i = 0 ;
		for (i = 1 ; pSerial->rxBuf[i+1] != Serial_Agreement_ABC.end1 ; i++)
		{
			pSerial->ABC_Data.Serial_New_Package_ABC[i-1] = pSerial->rxBuf[i] ;
			// 检测是否溢出
			if (i > Serial_Wait_Tail_MAX)
				break ;
		}
		// 补1位!!!,毕竟无论如何都是被迫离开for,少了1位
		pSerial->ABC_Data.Serial_New_Package_ABC[i-1] = pSerial->rxBuf[i] ;	
		// 情况1:退出for是因为检测到了帧尾
		if (pSerial->rxBuf[i+1] == Serial_Agreement_ABC.end1)
		{
			// 3. 开始检测第2个帧尾
			if (pSerial->rxBuf[i+2] != Serial_Agreement_ABC.end2)
			{
				pSerial->ABC_Data.error_Serial = Serial_Error_Tail ; // 错误3:第2个帧尾不合规
				memset(pSerial->ABC_Data.Serial_New_Package_ABC, 0, sizeof(pSerial->ABC_Data.Serial_New_Package_ABC));	// 清空记录数据
				return ;
			}
			else
			{
				// 初步处理数据包(仅合并数据)
				pSerial->ABC_Data.Serial_New_Package_ABC[i] = '\0' ;	// 加个结尾符号
				// 无错误
				pSerial->ABC_Data.error_Serial = 0 ; 
				// 存储标志位置1
				pSerial->ABC_Data.Serial_New_Package_Flag = 1 ;
			}
		}
		// 情况2:退出for是因为溢出了,说明第1个帧尾没有检测到
		else
		{
			pSerial->ABC_Data.error_Serial = Serial_Error_Tail ; // 错误2:第1个帧尾不合规
			memset(pSerial->ABC_Data.Serial_New_Package_ABC, 0, sizeof(pSerial->ABC_Data.Serial_New_Package_ABC));	// 清空记录数据
			return ;
		}
	}
}

// *文本:得到串口接收标志位*
uint8_t Serial_GetNewPackageFlag_ABC(Serial_Typedef *pSerial)
{
	if (pSerial->ABC_Data.Serial_New_Package_Flag == 1)			//如果标志位为1
	{
		pSerial->ABC_Data.Serial_New_Package_Flag = 0;
		return 1;					//则返回1，并自动清零标志位
	}
	return 0;						//如果标志位为0，则返回0
}

// *文本:得到错误原因*
int Serial_GetError_ABC(Serial_Typedef *pSerial)
{
	return pSerial->ABC_Data.error_Serial ;
}

// *文本:1. 封装一个函数,实现简易浮点数变量调试*
bool Serial_SetFloatData( Serial_Typedef *pSerial , char *KeyWord , char *cmd , float *Data)
{
	// KeyWord为关键词,有别与别的指令 cmd为整句话,包含%f等,VOFA怎么写这里也怎么写 Data为接收改变量的变量
	// 浮点数加上%f就行,位数不用管,空格也需要注意,具体协议还得看VOFA怎么输出的
	// 例:	建议VOFA发送:	@Kp=%.2f$#	串口接收:	Serial_SetFloatData("Kp" , "Kp=%f" , &test1) ;
	if ( strstr(pSerial->ABC_Data.Serial_New_Package_ABC , KeyWord) != NULL )
	{
		sscanf(pSerial->ABC_Data.Serial_New_Package_ABC, cmd , Data);
		return true ;
	}
	else
	{
		return false ;
	}
}

// *文本:2. 封装一个函数,实现简易整数变量调试*
bool Serial_SetIntData( Serial_Typedef *pSerial , char *KeyWord , char *cmd , int *Data)
{
	// KeyWord为关键词,有别与别的指令 cmd为整句话,包含%d等,VOFA没有%d,所以VOFA写%.0f即可代表%d Data为接收改变量的变量
	// 整数加上%d即可,空格也需要注意,具体协议还得看VOFA怎么输出的
	// 例:	建议VOFA发送:	@test=%.0f$#	串口接收:	Serial_SetIntData("test" , "test=%d" , &check1) ;
	if ( strstr(pSerial->ABC_Data.Serial_New_Package_ABC , KeyWord) != NULL )
	{
		sscanf(pSerial->ABC_Data.Serial_New_Package_ABC, cmd , Data);
		return true ;
	}
	else
	{
		return false ;
	}
}

// *文本:3. 封装一个函数,实现简易字符串指令检测*
bool Serial_CheckCmd( Serial_Typedef *pSerial , char *cmd )
{
    // cmd为需要检测的指令
    // 例如:
    // VOFA发送: @LED_ON$#
    // 判断:
    // if( Serial_CheckCmd(&Serial1 , "LED_ON") )
    // {
    //     LED_On();
    // }

    if ( strstr(pSerial->ABC_Data.Serial_New_Package_ABC , cmd) != NULL )
    {
        return true ;
    }
    else
    {
        return false ;
    }
}

// ========================== Serial_printf部分(有DMA的才能使用) ==========================

static void Serial_WriteBuf(Serial_Typedef *pSerial, uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        uint16_t next = (pSerial->txHead + 1) % TX_BUF_SIZE;

        if (next == pSerial->txTail)
            return;  // 满了直接丢

        pSerial->txBuf[pSerial->txHead] = data[i];
        pSerial->txHead = next;
    }
}

static void Serial_DMA_Kick(Serial_Typedef *pSerial)
{
    if (pSerial->dmaBusy) return;
    if (pSerial->txHead == pSerial->txTail) return;

    pSerial->dmaBusy = 1;

    uint16_t len;

    if (pSerial->txHead > pSerial->txTail)
        len = pSerial->txHead - pSerial->txTail;
    else
        len = TX_BUF_SIZE - pSerial->txTail;

    pSerial->dmaLen = len;

    DL_DMA_setSrcAddr(pSerial->tx_dma, pSerial->tx_channelNum,
                      (uint32_t)&pSerial->txBuf[pSerial->txTail]);

    DL_DMA_setDestAddr(pSerial->tx_dma, pSerial->tx_channelNum,
                       (uint32_t)&pSerial->uart_INST->TXDATA);

    DL_DMA_setTransferSize(pSerial->tx_dma, pSerial->tx_channelNum, len);

    DL_DMA_enableChannel(pSerial->tx_dma, pSerial->tx_channelNum);
}

void Serial_printf(Serial_Typedef *pSerial, const char *fmt, ...)
{
	if (pSerial->dma == NULL)
	{
		__BKPT(0);
		return;
	}
    char tempBuf[128];

    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(tempBuf, sizeof(tempBuf), fmt, args);
    va_end(args);

    if (len <= 0) return;

    if (len > sizeof(tempBuf))
        len = sizeof(tempBuf);

    Serial_WriteBuf(pSerial, (uint8_t *)tempBuf, len);
    Serial_DMA_Kick(pSerial);
}

// ========================== 无DMA的TX部分 ==========================
//串口发送单个字符
static void uart_send_char( Serial_Typedef *pSerial, char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while( DL_UART_isBusy(pSerial->uart_INST) == true );
    //发送单个字符
    DL_UART_Main_transmitData(pSerial->uart_INST, ch);
}
//串口发送字符串
void Serial_send_string(Serial_Typedef *pSerial,char* str)
{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
    while(*str!=0&&str!=0)
    {
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        uart_send_char(pSerial ,*str++);
    }
}

void Serial_Printf_Normal(Serial_Typedef *pSerial, const char *fmt, ...)
{
    char buffer[64];           // 根据需要可改大或改小
    va_list args;

    // 格式化字符串
    va_start(args, fmt);
    int len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (len <= 0) 
        return;

    // 防止缓冲区溢出
    if (len >= (int)sizeof(buffer))
        len = sizeof(buffer) - 1;

    // 发送字符串
    Serial_send_string(pSerial, buffer);
}

// ====================== 代码一般性修改部分 ======================

// 串口初始化:外部调用
void Serial_Init(void)
{
	#ifdef Serial1_Enable
	Serial_Initial(&Serial1 , UART_0_INST , DMA , DMA_UART_0_RX_Channel_CHAN_ID , UART_0_INST_INT_IRQN , DMA ,  DMA_UART_0_TX_Channel_CHAN_ID) ;	// 串口协议初始化
	#endif
	#ifdef Serial2_Enable
	Serial_Initial(&Serial2 , UART_1_INST , NULL , 100 , UART_1_INST_INT_IRQN , NULL , 100) ;	// 串口协议初始化
	#endif
	#ifdef Serial3_Enable
	Serial_Initial(&Serial3 , UART_2_INST , NULL , 100 , UART_2_INST_INT_IRQN , NULL , 100) ;	// 串口协议初始化
	#endif
}

// DMA完成搬运中断(单字符搬运后就触发中断)
void UART_0_INST_IRQHandler(void)
{
#ifdef Serial1_Enable
	static Serial_RX_FLAG_Typedef Serial1_Rx_State;		// 数据接收情况标志位-枚举
    switch (DL_UART_Main_getPendingInterrupt(Serial1.uart_INST)) {
        case DL_UART_MAIN_IIDX_DMA_DONE_RX:

			#ifdef Serial_Debug
			Serial_check[Serial_Count++] = Serial1.rx_temp ;	// 得到所有接收到的数据
			#endif 
			
			// 获得串口数据传输状态(更新)
			Serial1_Rx_State = Serial_Rx_State_Check(&Serial1);
			
			// HEX数据包
			if (Serial1_Rx_State == RX_OK_HEX)
			{
				// 开始处理原始数据包:HEX
				Serial_Data_Check_HEX(&Serial1) ;
			}
			// ABC数据包
			else if (Serial1_Rx_State == RX_OK_ABC)
			{
				// 开始处理原始数据包:ABC
				Serial_Data_Check_ABC(&Serial1) ;
			} 

            break;

		/* ===== TX（用于printf）===== */
        case DL_UART_MAIN_IIDX_DMA_DONE_TX:

            Serial1.txTail = (Serial1.txTail + Serial1.dmaLen) % TX_BUF_SIZE;
            Serial1.dmaBusy = 0;

            if (Serial1.txHead != Serial1.txTail)
            {
                Serial_DMA_Kick(&Serial1);
            }

            break;

        default:
            break;
    }
#endif
}


void UART_1_INST_IRQHandler(void)
{
#ifdef Serial2_Enable
    Serial_RX_FLAG_Typedef Serial2_Rx_State;
	switch (DL_UART_Main_getPendingInterrupt(Serial2.uart_INST)) 
    {
        // 接收中断
        case DL_UART_MAIN_IIDX_RX:
            // 必须存储接收到信息,即使不使用,否则中断FIFO存不下，再也进不去中断了
            Serial2.rx_temp = DL_UART_Main_receiveData(Serial2.uart_INST);  

            // 获得串口数据传输状态(更新)
			Serial2_Rx_State = Serial_Rx_State_Check(&Serial2);
			
			// HEX数据包
			if (Serial2_Rx_State == RX_OK_HEX)
			{
				// 开始处理原始数据包:HEX
				Serial_Data_Check_HEX(&Serial2) ;
			}
			// ABC数据包
			else if (Serial2_Rx_State == RX_OK_ABC)
			{
				// 开始处理原始数据包:ABC
				Serial_Data_Check_ABC(&Serial2) ;
			} 
            break;
        default:
            break;
    }
#endif
}

void UART_2_INST_IRQHandler(void)
{
#ifdef Serial3_Enable
    Serial_RX_FLAG_Typedef Serial3_Rx_State;
	switch (DL_UART_Main_getPendingInterrupt(Serial3.uart_INST)) 
    {
        // 接收中断
        case DL_UART_MAIN_IIDX_RX:
            // 必须存储接收到信息,即使不使用,否则中断FIFO存不下，再也进不去中断了
            Serial3.rx_temp = DL_UART_Main_receiveData(Serial3.uart_INST);  

            // 获得串口数据传输状态(更新)
			Serial3_Rx_State = Serial_Rx_State_Check(&Serial3);
			
			// HEX数据包
			if (Serial3_Rx_State == RX_OK_HEX)
			{
				// 开始处理原始数据包:HEX
				Serial_Data_Check_HEX(&Serial3) ;
			}
			// ABC数据包
			else if (Serial3_Rx_State == RX_OK_ABC)
			{
				// 开始处理原始数据包:ABC
				Serial_Data_Check_ABC(&Serial3) ;
			} 
            break;
        default:
            break;
    }
#endif
}
