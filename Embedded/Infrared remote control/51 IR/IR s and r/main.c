/*************************************************************************************

*
ʵ���������س�����������ʾ����ң�ؼ�ֵ����
		  
����˵���� (�������ͼ�ɼ��������Զ�Ӧʵ��ġ�ʵ�������½�)
		   1����Ƭ��-->�������ģ��
		   		P32-->J11
		   2����Ƭ��-->��̬�����ģ��
		   		J22-->J6
				P22-->J9(A)
				P23-->J9(B)
				P24-->J9(C)	
	
ע���������ң�����ϵĵ�ؾ�Ե��ƬҪ�õ�										

										  
**************************************************************************************

*/

#include "reg52.h"			 //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���
	

typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;

sbit IRIN=P3^2;   //����
sbit IR_TX = P2^4;         //������ⷢ��˿�

u8 IrValue[6];
u8 Time;

u8 DisplayData[8];
u8 code smgduan[17]={
0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0X76};
//0��1��2��3��4��5��6��7��8��9��A��b��C��d��E��F��H����ʾ��




static bit OP;        //���ⷢ��ܵ�����
static unsigned int count;       //��ʱ������
static unsigned int endcount; //��ֹ��ʱ����
static unsigned char flag;      //���ⷢ�ͱ�־
char iraddr1;  //ʮ��λ��ַ�ĵ�һ���ֽ�
char iraddr2;  //ʮ��λ��ַ�ĵڶ����ֽ�
	
	 unsigned char IR_Data[] = {0x22,0xdd,0x00,0xff}        ;//��������
   unsigned char num;
	 
/*******************************************************************************
* �� �� ��         : delay
* ��������		   : ��ʱ������i=1ʱ����Լ��ʱ10us
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
//����
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
* ������         :DigDisplay()
* ��������		 :�������ʾ����
* ����           : ��
* ���         	 : ��
*******************************************************************************/
void DigDisplay()
{
	u8 i;
	for(i=0;i<3;i++)
	{
		switch(i)	 //λѡ��ѡ�����������ܣ�
		{
			case(0):
				LSA=1;LSB=1;LSC=1; break;//��ʾ��0λ	
			case(1):
				LSA=0;LSB=1;LSC=1; break;//��ʾ��1λ
			case(2):
				LSA=1;LSB=0;LSC=1; break;//��ʾ��2λ 
		}
		P0=DisplayData[i];//��������
		delay(100); //���һ��ʱ��ɨ��	
		P0=0x00;//����
	}		
}


/*******************************************************************************
* ������         : IrInit()
* ��������		   : ��ʼ�������߽���
* ����           : ��
* ���         	 : ��
*******************************************************************************/

void IrInit()
{
	IT0=1;//�½��ش���
	EX0=1;//���ж�0����
	EA=1;	//�����ж�

	IRIN=1;//��ʼ���˿�
}


//============================================================================
void T0_init()        //��ʼ����ʱ��0
{
        TMOD = 0x01;
        ET0 = 1;
        EA = 1;
}
void T1_Init(void)                //13΢��@11.0592MHz
{
        TMOD &= 0x0F;                //���ö�ʱ��ģʽ
        TMOD |= 0x20;                //���ö�ʱ��ģʽ
        TL1 = 0xF4;                //���ö�ʱ��ʼֵ  38KHz
        TH1 = 0xF4;                //���ö�ʱ����ֵ
        TF1 = 0;                //���TF1��־
        TR1 = 1;                //��ʱ��1��ʼ��ʱ
}

/****�жϲ������ⷢ�䲻ͬʱ����ʱ����560us 1680us 4500us 9000us*****/

void IR_TXD_time( unsigned char th0,tl0)
{
                TH0 = th0;
                TL0 = tl0;
                TR0 = 1;  //�򿪶�ʱ����ʱ
                IR_TX = 1;
                while(TF0 == 0); //��ʱ���ȴ������ж�
                TF0 = 0;
                TR0 = 0;
                TR1 = 0;
                IR_TX = 1;//����ʱ���߷���˿ڣ���������
}
/*********���ⷢ��4�ֽ����ݺ���**********/

void IR_Send_4B_Date(unsigned char *p)
{
        unsigned char i,j;         
        ET1 = 1; TR1 = 1;        //��T1�жϣ�����38KHzƵ�ʲ������Ʋ����ߵ�ƽ�ź�
        IR_TXD_time(0xdf,0x9a);// ����9000us�ź�
        ET1 = 0;//�ر�38KHzƵ�ʲ������Ʋ����͵�ƽ�ź� ����ͬ
        IR_TXD_time(0xef,0xcd);//  ����4500us�ź�
        for(i = 0 ; i < 4 ; i++)
        {
                for(j = 0x01 ; j != 0 ; j <<= 1)//����32λ����
                {
                        ET1 = 1; TR1 = 1;
                        IR_TXD_time(0xfe,0x08);//  ����560us�ߵ�ƽ�ź�
                        ET1 = 0;
                        if(*(p+i) & j) //��λ����1��
                                IR_TXD_time(0xfa,0x4c);//  ����1680us�͵�ƽ�ź�
                         else
                                IR_TXD_time(0xfe,0x3f);//  ����560us�͵�ƽ�ź�
                }
        }
        ET1 = 1;TR1 = 1;
        IR_TXD_time(0xfd,0xfc);//  ����560us�ߵ�ƽ�źţ�����32λ���ݷ���
        ET1 = 0;
}



void key_send(void){

								P3 = 0x0f;
                if(P3 != 0x0f)
                {
                        IR_TXD_time(0x8f,0x9a);// ��ʱ����
                        if(P3 !=0x0f)
                        {  
                                num = P3&0x0f;
                        }

                if(num == 0x0e)           //�Ҽһ����е�ң�����ݣ��򵥲����ĸ���ֵ
									{
										IR_Data[2] =Convert_Byte(0xdc)   ; 
										IR_Data[3] = ~IR_Data[2];
									} //        ��Դ
                else if(num == 0x0d)
                  {
										IR_Data[2] = Convert_Byte(0x99)  ; 
										IR_Data[3] = ~IR_Data[2];
									}//����
                else if(num == 0x0b)
                  {
										IR_Data[2] = Convert_Byte(0xc1)  ; 
										IR_Data[3] = ~IR_Data[2];
									} //����
                else if(num == 0x07)
                  {
										IR_Data[2] = Convert_Byte(0xd2)  ; 
										IR_Data[3] = ~IR_Data[2];
									}  //�·�
                IR_Send_4B_Date(IR_Data); //����
                IR_TXD_time(0xff,0x01);// ��ʱ�������
                }          



}
void S_R(void){
			P3 = 0x0f;
      if(P3 != 0x0f)
    {
      IR_TXD_time(0x8f,0x9a);// ��ʱ����
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

      IR_Send_4B_Date(IR_Data); //����
      IR_TXD_time(0xff,0x01);// ��ʱ�������
      }          
							         



}
//========================================================================================
/*******************************************************************************
* �� �� ��       : main
* ��������		 : ������
* ��    ��       : ��
* ��    ��    	 : ��
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
//�ж�=======================================================================================
/*******************************************************************************
* ������         : ReadIr()
* ��������		   : ��ȡ������ֵ���жϺ���
* ����           : ��
* ���         	 : ��
*******************************************************************************/

void ReadIr() interrupt 0
{
	u8 j,k;
	u16 err;
	Time=0;					 
	delay(700);	//7ms
	if(IRIN==0)		//ȷ���Ƿ���Ľ��յ���ȷ���ź�
	{	 
		
		err=1000;				//1000*10us=10ms,����˵�����յ�������ź�
		/*������������Ϊ����ѭ���������һ������Ϊ�ٵ�ʱ������ѭ������ó�������ʱ
		�������������*/	
		while((IRIN==0)&&(err>0))	//�ȴ�ǰ��9ms�ĵ͵�ƽ��ȥ  		
		{			
			delay(1);
			err--;
		} 
		if(IRIN==1)			//�����ȷ�ȵ�9ms�͵�ƽ
		{
			err=500;
			while((IRIN==1)&&(err>0))		 //�ȴ�4.5ms����ʼ�ߵ�ƽ��ȥ
			{
				delay(1);
				err--;
			}
			for(k=0;k<4;k++)		//����4������
			{				
				for(j=0;j<8;j++)	//����һ������
				{

					err=60;		
					while((IRIN==0)&&(err>0))//�ȴ��ź�ǰ���560us�͵�ƽ��ȥ
					{
						delay(1);
						err--;
					}
					err=500;
					while((IRIN==1)&&(err>0))	 //����ߵ�ƽ��ʱ�䳤�ȡ�
					{
						delay(10);	 //0.1ms
						Time++;
						err--;
						if(Time>30)
						{
							return;
						}
					}
					IrValue[k]>>=1;	 //k��ʾ�ڼ�������
					if(Time>=8)			//����ߵ�ƽ���ִ���565us����ô��1
					{
						IrValue[k]|=0x80;
					}
					Time=0;		//����ʱ��Ҫ���¸�ֵ							
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
        IR_TX = ~IR_TX;  //13΢��@11.0592MHz ,����38KHz�ز�
}
void Timer0() interrupt 1
{
}