#include "remote.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

//红外遥控初始化
//设置IO以及定时器4的输入捕获
void Remote_Init(void)    			  
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //使能PORTB时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	//TIM4 时钟使能 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //PB9 输入 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//上拉输入 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);	//初始化GPIOB.9
	
						  
 	TIM_TimeBaseStructure.TIM_Period = 10000; //设定计数器自动重装值 最大10ms溢出  
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	//预分频器,1M的计数频率,1us加1.	   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  // 选择输入端 IC4映射到TI4上
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC4F=0011 配置输入滤波器 8个定时器时钟周期滤波
  TIM_ICInit(TIM4, &TIM_ICInitStructure);//初始化定时器输入捕获通道

  TIM_Cmd(TIM4,ENABLE ); 	//使能定时器4
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	

	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC4IE捕获中断								 
}
//PA7-TIM3-CH2
void PWM_Init(u16 arr,u16 psc){

 
//	GPIO_InitTypeDef GPIO_InitStructure;
////	NVIC_InitTypeDef NVIC_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM_OCInitStructure;  
// 
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE); //使能PORTB时钟 
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	//TIM4 时钟使能 

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);
// 	GPIO_SetBits(GPIOA,GPIO_Pin_7);	
//	
//						  
// 	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 最大10ms溢出  
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器,1M的计数频率,1us加1.	   
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式

//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx

//	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
//	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
//	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_Pulse= (arr+1)/3;

//	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	
//  TIM_Cmd(TIM3,ENABLE ); 	//使能定时器3
 
//	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	

//	TIM_ITConfig( TIM3,TIM_IT_Update|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC4IE捕获中断				

	/* 初始化结构体定义 */
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;                        //定时器基本设置
	TIM_OCInitTypeDef 	TIM_OCInitStructure;                                  //定时器比较输出配置

	/* 使能相应端口的时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);                      //使能定时器2时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE); //使能GPIO外设时钟
	
	/* GPIOA.7初始化 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                                 // TIM3 CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                           // PA.7 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);

	/* TIM3 初始化*/
	TIM_TimeBaseInitStructure.TIM_Period = arr;                               //下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                            //作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;                          //时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;           //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

	/* 定时器TIM3 Ch2 PWM模式初始化 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                         //选择定时器模式:TIM PWM1   TIM_OCMode_PWM1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;             //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = (arr+1)/3;                                //占空比1:3 (arr+1)/10  3
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                 //输出极性:TIM输出比较极性高   TIM_OCPolarity_High
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);

	/* 使能TIM3在CCR1上的预装载寄存器 */
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_Cmd(TIM3,ENABLE ); 	//使能定时器3

}
unsigned char Convert_Byte(unsigned char Data) 
{
char i;
unsigned char Data1;
Data1 = Data;
Data = 0x00;

for(i = 0 ; i<(sizeof(char)*8/2) ; i++)
{
Data = Data | (Data1&(0X80>>i))>>(7-i*2);
Data = Data | (Data1&(0X01<<i))<<(7-i*2);
}

return  Data;

}


void IR_Send_Byte(u8 getdata)
{
u8 senddata,i;
senddata=getdata;
for(i=0;i<8;i++)
{
if(senddata&0x01)
{
//1
TIM_Cmd(TIM3, ENABLE);
delay_us(565);
TIM_Cmd(TIM3, DISABLE);
delay_us(565);
delay_us(565);
delay_us(565);
}
else
{
//0
TIM_Cmd(TIM3, ENABLE);
delay_us(565);
TIM_Cmd(TIM3, DISABLE);
delay_us(565);
}
senddata=senddata>>1;
}
}
void IR_Send(void)
{
//9ms
TIM_Cmd(TIM3, ENABLE);
delay_us(9);
//4.5ms
TIM_Cmd(TIM3, DISABLE);
delay_ms(4);
delay_us(500);
IR_Send_Byte(0x00); //用户码
IR_Send_Byte(0xFF); //用户反码
IR_Send_Byte(0x5A); //数据码
IR_Send_Byte(0xA5); //数据反码
//下面增加是为了避免在发送最后一位时出现错码
TIM_Cmd(TIM3, ENABLE);
delay_us(565);
TIM_Cmd(TIM3, DISABLE);
delay_us(565);

}

void Infrared_Send(u32 RmtRec)
{
//9ms
TIM_Cmd(TIM3, ENABLE);
delay_us(9);
//4.5ms
TIM_Cmd(TIM3, DISABLE);
delay_ms(4);
delay_us(500);
IR_Send_Byte(Convert_Byte(RmtRec>>24)); //用户码
IR_Send_Byte(Convert_Byte(RmtRec>>16)); //用户反码
IR_Send_Byte(Convert_Byte(RmtRec>>8)); //数据码
IR_Send_Byte(Convert_Byte(RmtRec)); //数据反码
//下面增加是为了避免在发送最后一位时出现错码
TIM_Cmd(TIM3, ENABLE);
delay_us(565);
TIM_Cmd(TIM3, DISABLE);
delay_us(565);

}

//遥控器接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
u8 	RmtSta=0;	  	  
u16 Dval;		//下降沿时计数器的值
u32 RmtRec=0;	//红外接收到的数据	   		    
u8  RmtCnt=0;	//按键按下的次数	  
//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{ 		    	 
 
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)								//上次有数据被接收到了
		{	
			RmtSta&=~0X10;							//取消上升沿已经被捕获标记
			if((RmtSta&0X0F)==0X00)
			{
				RmtSta|=1<<6;	//标记已经完成一次按键的键值信息采集
				USART_SendData(USART1, (RmtRec&0xff000000)>>16);
				delay_ms(10);
				USART_SendData(USART1, RmtRec);		
				
				LED1=!LED1;
				
//				Infrared_Send(RmtRec);
//				delay_ms(500);
//				TIM_Cmd(TIM3, ENABLE);
//				delay_ms(500);
			}
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);					//清空引导标识
				RmtSta&=0XF0;						//清空计数器	
			}
			
		}
	
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET)
	{	  
		if(RDATA)//上升沿捕获
		{
  			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);						//CC4P=1	设置为下降沿捕获
			TIM_SetCounter(TIM4,0);							//清空定时器值
			RmtSta|=0X10;							//标记上升沿已经被捕获
		}else //下降沿捕获
		{
			Dval=TIM_GetCapture4(TIM4);					//读取CCR4也可以清CC4IF标志位
  		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising);				//CC4P=0	设置为上升沿捕获
			if(RmtSta&0X10)							//完成一次高电平捕获 
			{
 				if(RmtSta&0X80)//接收到了引导码
				{
					
					if(Dval>300&&Dval<800)			//560为标准值,560us
					{
						RmtRec<<=1;					//左移一位.
						RmtRec|=0;					//接收到0	   
					}else if(Dval>1400&&Dval<1800)	//1680为标准值,1680us
					{
						RmtRec<<=1;					//左移一位.
						RmtRec|=1;					//接收到1
					}else if(Dval>2200&&Dval<2600)	//得到按键键值增加的信息 2500为标准值2.5ms
					{
						RmtCnt++; 					//按键次数增加1次
						RmtSta&=0XF0;				//清空计时器		
					}
 				}else if(Dval>4200&&Dval<4700)		//4500为标准值4.5ms
				{
					RmtSta|=1<<7;					//标记成功接收到了引导码
					RmtCnt=0;						//清除按键次数计数器
				}						 
			}
			RmtSta&=~(1<<4);
		}			
			
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update|TIM_IT_CC4);	 	    
}

































