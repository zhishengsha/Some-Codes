/*****************************************************
        ������AT89C52  ��ƵΪ11.0592Mhz   ����NECЭ�����
                  ��̣�wzqwxx ˮ���˼�
******************************************************/

#include <REGX52.H>
sbit IR_TX = P2^4;         //������ⷢ��˿�

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

/*********������**********/

void main()
{
        unsigned char IR_Data[] = {0x22,0xdd,0x00,0xff}        ;//��������
        unsigned char num;
        T0_init();
        T1_Init();
        while(1)
        {
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
}
void Timer1() interrupt 3
{
        IR_TX = ~IR_TX;  //13΢��@11.0592MHz ,����38KHz�ز�
}
void Timer0() interrupt 1
{
}
