#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "remote.h"
 
/************************************************
 ALIENTEK��ӢSTM32������ʵ��27
 ����ң�� ʵ��    
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

            
 int main(void)
 {	 
	u8 key;
	u16 t=0;	
 	u8 *str=0;

	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();	
	KEY_Init();	 	
	Remote_Init();			//������ճ�ʼ��	
	PWM_Init(1895,00); 
 
 	POINT_COLOR=RED;		//��������Ϊ��ɫ 
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






