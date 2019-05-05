#include <STC8.h>
#include <intrins.h>

#define u8 unsigned char
#define u16 unsigned int

sbit HC595_DS=P1^2;
sbit HC595_SH=P1^3;
sbit HC595_ST=P1^4;

u8 xdata LED_2X16[34]={0x01,0x00,0x21,0x10,0x22,0x10,0x22,0x10,
					  0x24,0x90,0x28,0x92,0x20,0x91,0xFF,0xBE,
					  0x20,0xB0,0x28,0xD0,0x24,0x90,0x24,0x10,
					  0x62,0x30,0x23,0x10,0x02,0x00,0x00,0x00,
						0xff,0xff};

u8 xdata Mould[2];

u8 xdata Upright[2];

void HC595_Write()
{
	u8 i;
	HC595_DS=1;
	HC595_SH=1;
	HC595_ST=1;
	for(i=0;i<8;i++)
	{
		HC595_SH=0;
		HC595_DS=Upright[1]&0x80;
		Upright[1]<<=1;
		HC595_SH=1;
	}
	for(i=0;i<8;i++)
	{
		HC595_SH=0;
		HC595_DS=Upright[0]&0x80;
		Upright[0]<<=1;
		HC595_SH=1;
	}
	for(i=0;i<8;i++)
	{
		HC595_SH=0;
		HC595_DS=Mould[1]&0x80;
		Mould[1]<<=1;
		HC595_SH=1;
	}
	for(i=0;i<8;i++)
	{
		HC595_SH=0;
		HC595_DS=Mould[0]&0x80;
		Mould[0]<<=1;
		HC595_SH=1;
	}
	HC595_ST=0;
	HC595_DS=1;
	HC595_ST=1;
	
}

void LED16()
{
	static u8 i=0;
	static u16 kkk=0x0001;
	Upright[1]=kkk/256;
	Upright[0]=kkk%256;
	Mould[1]=~LED_2X16[i+1];
	Mould[0]=~LED_2X16[i];
//	Mould[1]=0x55;
//	Mould[0]=0x55;
	kkk=_irol_(kkk,1);
	i+=2;
	if(i>=31)
	{
		i=0;
		kkk=0x0001;
	}
	HC595_Write();
	
	
}

void main()
{
	while(1)
	{
		LED16();
	}
}
