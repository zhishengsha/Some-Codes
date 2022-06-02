/*****************************************************
        主机：AT89C52  主频为11.0592Mhz   红外NEC协议编码
                  编程：wzqwxx 水上人家
******************************************************/

#include <REGX52.H>
sbit IR_TX = P2^4;         //定义红外发射端口

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

/*********主函数**********/

void main()
{
        unsigned char IR_Data[] = {0x22,0xdd,0x00,0xff}        ;//待发数据
        unsigned char num;
        T0_init();
        T1_Init();
        while(1)
        {
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
}
void Timer1() interrupt 3
{
        IR_TX = ~IR_TX;  //13微秒@11.0592MHz ,产生38KHz载波
}
void Timer0() interrupt 1
{
}
