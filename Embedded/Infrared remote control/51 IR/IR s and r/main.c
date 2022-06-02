/*************************************************************************************

*
实验现象：下载程序后，数码管显示红外遥控键值数据
		  
接线说明： (具体接线图可见开发攻略对应实验的“实验现象”章节)
		   1，单片机-->红外接收模块
		   		P32-->J11
		   2，单片机-->动态数码管模块
		   		J22-->J6
				P22-->J9(A)
				P23-->J9(B)
				P24-->J9(C)	
	
注意事项：红外遥控器上的电池绝缘隔片要拿掉										

										  
**************************************************************************************

*/

#include "reg52.h"			 //此文件中定义了单片机的一些特殊功能寄存器
	

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;

sbit IRIN=P3^2;   //接收
sbit IR_TX = P2^4;         //定义红外发射端口

u8 IrValue[6];
u8 Time;

u8 DisplayData[8];
u8 code smgduan[17]={
0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0X76};
//0、1、2、3、4、5、6、7、8、9、A、b、C、d、E、F、H的显示码




static bit OP;        //红外发射管的亮灭
static unsigned int count;       //延时计数器
static unsigned int endcount; //终止延时计数
static unsigned char flag;      //红外发送标志
char iraddr1;  //十六位地址的第一个字节
char iraddr2;  //十六位地址的第二个字节
	
	 unsigned char IR_Data[] = {0x22,0xdd,0x00,0xff}        ;//待发数据
   unsigned char num;
	 
/*******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
*******************************************************************************/
void delay(u16 i)
{
	while(i--);	
}
void delay0()
{
  int i,j;
  for(i=0;i<400;i++)
  {
    for(j=0;j<100;j++)
    {
    }
  }
}
//倒序
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
/*******************************************************************************
* 函数名         :DigDisplay()
* 函数功能		 :数码管显示函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void DigDisplay()
{
	u8 i;
	for(i=0;i<3;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LSA=1;LSB=1;LSC=1; break;//显示第0位	
			case(1):
				LSA=0;LSB=1;LSC=1; break;//显示第1位
			case(2):
				LSA=1;LSB=0;LSC=1; break;//显示第2位 
		}
		P0=DisplayData[i];//发送数据
		delay(100); //间隔一段时间扫描	
		P0=0x00;//消隐
	}		
}


/*******************************************************************************
* 函数名         : IrInit()
* 函数功能		   : 初始化红外线接收
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/

void IrInit()
{
	IT0=1;//下降沿触发
	EX0=1;//打开中断0允许
	EA=1;	//打开总中断

	IRIN=1;//初始化端口
}


//============================================================================
void T0_init()        //初始化定时器0
{
        TMOD = 0x01;
        ET0 = 1;
        EA = 1;
}
void T1_Init(void)                //13微秒@11.0592MHz
{
        TMOD &= 0x0F;                //设置定时器模式
        TMOD |= 0x20;                //设置定时器模式
        TL1 = 0xF4;                //设置定时初始值  38KHz
        TH1 = 0xF4;                //设置定时重载值
        TF1 = 0;                //清除TF1标志
        TR1 = 1;                //定时器1开始计时
}

/****中断产生红外发射不同时长延时函数560us 1680us 4500us 9000us*****/

void IR_TXD_time( unsigned char th0,tl0)
{
                TH0 = th0;
                TL0 = tl0;
                TR0 = 1;  //打开定时器计时
                IR_TX = 1;
                while(TF0 == 0); //延时，等待产生中断
                TF0 = 0;
                TR0 = 0;
                TR1 = 0;
                IR_TX = 1;//结束时拉高发射端口，结束发射
}
/*********红外发送4字节数据函数**********/

void IR_Send_4B_Date(unsigned char *p)
{
        unsigned char i,j;         
        ET1 = 1; TR1 = 1;        //打开T1中断，产生38KHz频率波，调制产生高电平信号
        IR_TXD_time(0xdf,0x9a);// 产生9000us信号
        ET1 = 0;//关闭38KHz频率波，调制产生低电平信号 ，下同
        IR_TXD_time(0xef,0xcd);//  产生4500us信号
        for(i = 0 ; i < 4 ; i++)
        {
                for(j = 0x01 ; j != 0 ; j <<= 1)//发送32位数据
                {
                        ET1 = 1; TR1 = 1;
                        IR_TXD_time(0xfe,0x08);//  产生560us高电平信号
                        ET1 = 0;
                        if(*(p+i) & j) //按位与是1则
                                IR_TXD_time(0xfa,0x4c);//  产生1680us低电平信号
                         else
                                IR_TXD_time(0xfe,0x3f);//  产生560us低电平信号
                }
        }
        ET1 = 1;TR1 = 1;
        IR_TXD_time(0xfd,0xfc);//  产生560us高电平信号，结束32位数据发送
        ET1 = 0;
}



void key_send(void){

								P3 = 0x0f;
                if(P3 != 0x0f)
                {
                        IR_TXD_time(0x8f,0x9a);// 延时消抖
                        if(P3 !=0x0f)
                        {  
                                num = P3&0x0f;
                        }

                if(num == 0x0e)           //我家机顶盒的遥控数据，简单测试四个键值
									{
										IR_Data[2] =Convert_Byte(0xdc)   ; 
										IR_Data[3] = ~IR_Data[2];
									} //        电源
                else if(num == 0x0d)
                  {
										IR_Data[2] = Convert_Byte(0x99)  ; 
										IR_Data[3] = ~IR_Data[2];
									}//左移
                else if(num == 0x0b)
                  {
										IR_Data[2] = Convert_Byte(0xc1)  ; 
										IR_Data[3] = ~IR_Data[2];
									} //右移
                else if(num == 0x07)
                  {
										IR_Data[2] = Convert_Byte(0xd2)  ; 
										IR_Data[3] = ~IR_Data[2];
									}  //下翻
                IR_Send_4B_Date(IR_Data); //发送
                IR_TXD_time(0xff,0x01);// 延时间隔连发
                }          



}
void S_R(void){
			P3 = 0x0f;
      if(P3 != 0x0f)
    {
      IR_TXD_time(0x8f,0x9a);// 延时消抖
     if(P3 !=0x0f)
       {  
              num = P3&0x0f;
        }

    if(num == 0x0d)
			{
				IR_Data[0] =Convert_Byte(IR_Data[0])   ; 
				IR_Data[1] = ~IR_Data[0];
				IR_Data[2] =IrValue[2]; 
				IR_Data[3] = ~IR_Data[2];
			} 

      IR_Send_4B_Date(IR_Data); //发送
      IR_TXD_time(0xff,0x01);// 延时间隔连发
      }          
							         



}
//========================================================================================
/*******************************************************************************
* 函 数 名       : main
* 函数功能		 : 主函数
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void main()
{	
	IrInit();

   T0_init();
   T1_Init();
	 
	while(1)
	{	
		DisplayData[0] = smgduan[Convert_Byte(IrValue[2])/16];
		DisplayData[1] = smgduan[Convert_Byte(IrValue[2])%16];
		DisplayData[2] = smgduan[16];
	    DigDisplay();	
			S_R();		
	}		
}
//中断=======================================================================================
/*******************************************************************************
* 函数名         : ReadIr()
* 函数功能		   : 读取红外数值的中断函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/

void ReadIr() interrupt 0
{
	u8 j,k;
	u16 err;
	Time=0;					 
	delay(700);	//7ms
	if(IRIN==0)		//确认是否真的接收到正确的信号
	{	 
		
		err=1000;				//1000*10us=10ms,超过说明接收到错误的信号
		/*当两个条件都为真是循环，如果有一个条件为假的时候跳出循环，免得程序出错的时
		侯，程序死在这里*/	
		while((IRIN==0)&&(err>0))	//等待前面9ms的低电平过去  		
		{			
			delay(1);
			err--;
		} 
		if(IRIN==1)			//如果正确等到9ms低电平
		{
			err=500;
			while((IRIN==1)&&(err>0))		 //等待4.5ms的起始高电平过去
			{
				delay(1);
				err--;
			}
			for(k=0;k<4;k++)		//共有4组数据
			{				
				for(j=0;j<8;j++)	//接收一组数据
				{

					err=60;		
					while((IRIN==0)&&(err>0))//等待信号前面的560us低电平过去
					{
						delay(1);
						err--;
					}
					err=500;
					while((IRIN==1)&&(err>0))	 //计算高电平的时间长度。
					{
						delay(10);	 //0.1ms
						Time++;
						err--;
						if(Time>30)
						{
							return;
						}
					}
					IrValue[k]>>=1;	 //k表示第几组数据
					if(Time>=8)			//如果高电平出现大于565us，那么是1
					{
						IrValue[k]|=0x80;
					}
					Time=0;		//用完时间要重新赋值							
				}
			}
		}
		if(IrValue[2]!=~IrValue[3])
		{
			return;
		}
	}			
}

void Timer1() interrupt 3
{
        IR_TX = ~IR_TX;  //13微秒@11.0592MHz ,产生38KHz载波
}
void Timer0() interrupt 1
{
}