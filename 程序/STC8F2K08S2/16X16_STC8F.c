#include <STC8.h>
#include <intrins.h>

#define u8 unsigned char
#define u16 unsigned int

sbit HC595_DS=P1^2;
sbit HC595_SH=P1^3;
sbit HC595_ST=P1^4;

u8 code ceshi[2][32]=
{
	{0x00,0x00,0x26,0x06,0x23,0x18,0x30,0x60,0x00,0x02,0x3F,0xE6,0x20,0x1C,0x27,0xF0,0x20,0x18,0x3F,0xE6,0x00,0x00,0x1F,0xF0,0x00,0x02,0x7F,0xFE,0x7F,0xFE,0x00,0x00},
	{0x00,0x00,0x02,0x00,0x22,0x04,0x13,0xFC,0x00,0x08,0x08,0x14,0x09,0x84,0x09,0xFC,0x09,0x8C,0x08,0x08,0x7F,0xC0,0x08,0x78,0x48,0x0E,0x38,0x02,0x08,0x0E,0x00,0x00}
};

u8 xdata LED_2X16[34];

u8 xdata Mould[4];

u8 xdata Upright[4];

void HC595_Write()
{
	u8 i,j;
	HC595_DS=1;
	HC595_SH=1;
	HC595_ST=1;
	for(j=2;j>0;j--)
	{
		for(i=0;i<8;i++)
		{
			HC595_SH=0;
			HC595_DS=Upright[j*2-1]&0x80;
			Upright[j*2-1]<<=1;
			HC595_SH=1;
		}
		for(i=0;i<8;i++)
		{
			HC595_SH=0;
			HC595_DS=Upright[j*2-2]&0x80;
			Upright[j*2-2]<<=1;
			HC595_SH=1;
		}
		for(i=0;i<8;i++)
		{
			HC595_SH=0;
			HC595_DS=Mould[j*2-1]&0x80;
			Mould[j*2-1]<<=1;
			HC595_SH=1;
		}
		for(i=0;i<8;i++)
		{
			HC595_SH=0;
			HC595_DS=Mould[j*2-2]&0x80;
			Mould[j*2-2]<<=1;
			HC595_SH=1;
		}
	}
	HC595_ST=0;
	HC595_DS=1;
	HC595_ST=1;
	
}

void Timer0_Routnie() interrupt 1
{
	static u8 i=0;
	static u16 kkk=0x0001;
	
	Upright[3]=kkk/256;
	Upright[2]=kkk%256;
	Mould[3]=~LED_2X16[i+33];
	Mould[2]=~LED_2X16[i+32];
	
	Upright[1]=kkk/256;
	Upright[0]=kkk%256;
	Mould[1]=~LED_2X16[i+1];
	Mould[0]=~LED_2X16[i];
	
	kkk=_irol_(kkk,1);
	i+=2;
	if(i>=31)
	{
		i=0;
		kkk=0x0001;
	}
	HC595_Write();
	
	
}

void Timer0Init(void)		//1041微秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x34;		//设置定时初值
	TH0 = 0xCF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
	EA=1;
}


void main()
{
	u8 i=0,num=0,ww=0;
	u16 timer=0;
	Timer0Init();
	
//	for(i=0;i<64;i++)
//	{
//		LED_2X16[i]=0;
//	}
	for(i=0;i<64;i++)
	{
		LED_2X16[i]=ceshi[i/32][i%32];
	}
	
	while(1)
	{
		timer=50000;
		while(timer--);
		LED_2X16[32]=ceshi[ww][num];
		num++;
		LED_2X16[33]=ceshi[ww][num];
		num++;
		if(num>=32)
		{
			num=0;
			ww++;
			if(ww>=10)
			{
				ww=0;
			}
		}
		for(i=0;i<64;i++)
		{
			LED_2X16[i]=LED_2X16[i+2];
		}
	}
}
