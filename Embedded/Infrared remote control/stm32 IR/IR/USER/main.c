#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "remote.h"
 
/************************************************
 ALIENTEK精英STM32开发板实验27
 红外遥控 实验    
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

            
 int main(void)
 {	 
	u8 key;
	u16 t=0;	
 	u8 *str=0;

	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			     //LED端口初始化
	LCD_Init();	
	KEY_Init();	 	
	Remote_Init();			//红外接收初始化	
	PWM_Init(1895,00); 
 
 	POINT_COLOR=RED;		//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"ELITE STM32");
	LCD_ShowString(30,70,200,16,16,"REMOTE TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
 		    							  
	while(1)
	{
		//if(KEY_Scan(0)==1)
		//IR_Send();
		Infrared_Send(RmtRec);
		delay_ms(500);
		TIM_Cmd(TIM3, ENABLE);
		delay_ms(500);

	}
}






