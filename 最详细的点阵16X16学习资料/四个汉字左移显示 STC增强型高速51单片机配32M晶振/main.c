//====================================================================================
//1、本程序适用于：由个595驱动的16*16点阵。先发16位阳极列选信号。再发送阴极字节模数据。
//2、本程序，并未使用WQX-51编程技巧，为方便公开，使用传统编程思路编写。
//3、交流方式：QQ群318199961。 淘宝连接：http://wqxdz.taobao.com
//====================================================================================
#include<STC12C5A60S2.H>
#include<intrins.h>
#define uchar unsigned char 
#define uint unsigned int   
sbit HC595_DS=P0^3;									//74HC595的数据
sbit HC595_SCLK=P0^1;								//74HC595的时钟
sbit HC595_LCLK=P0^0;								//74HC595的锁存
uchar code hanzi[10][32]=
{
//五角星
	{	0x00,0x00,0x04,0x00,0x06,0x02,0x07,0x0C,0x07,0xBC,0x07,0xF8,0x0F,0xF8,0x3F,0xF0,0xFF,0xE0,0x3F,0xF0,0x0F,0xF8,0x07,0xF8,0x07,0xBC,0x07,0x0C,0x06,0x02,0x04,0x00		},
//祁
	{	0x10,0x20,0x10,0x40,0x10,0x80,0x91,0x00,0x73,0xFF,0x15,0x00,0x18,0x80,0x00,0x00,0x7F,0xFF,0x40,0x10,0x42,0x08,0x4D,0x04,0x70,0x88,0x00,0x70,0x00,0x00,0x00,0x00		},
//绪
	{	0x04,0x44,0x0C,0xC6,0x35,0x44,0xC6,0x48,0x0C,0x68,0x02,0x48,0x22,0x80,0x23,0xFF,0x22,0x92,0xFE,0x92,0x2A,0x92,0x32,0x92,0x23,0xFF,0x46,0x80,0x02,0x00,0x00,0x00		},
//电
	{	0x00,0x00,0x1F,0xE0,0x12,0x40,0x12,0x40,0x12,0x40,0x12,0x40,0xFF,0xFC,0x12,0x42,0x12,0x42,0x12,0x42,0x12,0x42,0x3F,0xE2,0x10,0x02,0x00,0x0E,0x00,0x00,0x00,0x00		},
//子
	{	0x01,0x00,0x01,0x00,0x41,0x00,0x41,0x00,0x41,0x00,0x41,0x02,0x41,0x01,0x47,0xFE,0x45,0x00,0x49,0x00,0x51,0x00,0x61,0x00,0x01,0x00,0x03,0x00,0x01,0x00,0x00,0x00		},
//黑桃
	{ 	0x00,0x00,0x03,0xC0,0x07,0xE0,0x0F,0xF0,0x1F,0xF0,0x3F,0xE1,0x7F,0xC3,0xFF,0xFF,0xFF,0xFF,0x7F,0xC3,0x3F,0xE1,0x1F,0xF0,0x0F,0xF0,0x07,0xE0,0x03,0xC0,0x00,0x00		},
//红心
	{	0x00,0x00,0x07,0x80,0x0F,0xC0,0x1F,0xE0,0x1F,0xF0,0x1F,0xF8,0x0F,0xFC,0x07,0xFE,0x07,0xFE,0x0F,0xFC,0x1F,0xF8,0x1F,0xF0,0x1F,0xE0,0x0F,0xC0,0x07,0x80,0x00,0x00		},
//梅花
	{ 	0x00,0x00,0x00,0xE0,0x01,0xF0,0x03,0xF8,0x33,0xF8,0x79,0xF1,0xFD,0xE3,0xFF,0xFF,0xFF,0xFF,0xFD,0xE3,0x79,0xF1,0x33,0xF8,0x03,0xF8,0x01,0xF0,0x00,0xE0,0x00,0x00		},
//方片
	{	0x00,0x00,0x01,0x00,0x03,0x80,0x07,0xC0,0x0F,0xE0,0x1F,0xF0,0x3F,0xF8,0x7F,0xFC,0xFF,0xFE,0x7F,0xFC,0x3F,0xF8,0x1F,0xF0,0x0F,0xE0,0x07,0xC0,0x03,0x80,0x01,0x00		},
//花点		
	{	0xAA,0xAA,0x55,0x55,0xAA,0xAA,0x55,0x55,0xAA,0xAA,0x55,0x55,0xAA,0xAA,0x55,0x55,0xAA,0xAA,0x55,0x55,0xAA,0xAA,0x55,0x55,0xAA,0xAA,0x55,0x55,0xAA,0xAA,0x55,0x55		}
	};
uchar xdata Mould[8];  								//控制该列的取模值，低电平亮灯，每个模组16行，占2字节
uchar xdata upright[8];								//控制某一列，高电平亮灯,每个模组16列，占2字节
uchar xdata LED2X64[130];							//开辟缓存空间，被定时器随时调用。每列两个字节，开发板总共64列。加2个移位。

void Timer0_Init()									//定时器配置：16位定时器，1.25ms显示1列，帧率50帧。
{
	TMOD&=0Xf0;
	TMOD|=0X01;
	TL0=0xfb;										//定时器赋初始值1.25ms，STC系列1T增强型51单片机。
	TH0=0xf2;										//定时器赋初始值1.25ms，STC系列1T增强型51单片机。
	//TL0=0x1E;										//定时器赋初始值1.25ms，传统12T指令周期的8951单片机。
	//TH0=0xFB;										//定时器赋初始值1.25ms，传统12T指令周期的8951单片机。
	ET0=1;
	TR0=1;
	EA=1;
}
void HC595_Write()									//74HC595发送数据。
{
	uchar i,j;
	HC595_DS=1;
	HC595_SCLK=1;
	HC595_LCLK=0;
	for(j=4;j>0;j--)								//4块点阵模组，每个模组有4片595芯片。
	{
		for(i=0;i<8;i++) 							//发送给每个模组的 U4 ，高电平整列可以亮，低电平整列灭。
		{
			HC595_SCLK=0;							//时钟拉低
			HC595_DS=(upright[j*2-1]&0x80);  		//从最后一个字节开始，先高位后低位。
			upright[j*2-1]<<=1;						//字节左移
			HC595_SCLK=1;							//时钟上升沿拷贝
		}
		for(i=0;i<8;i++)							//发送给每个模组的 U3 ，高电平整列可以亮，低电平整列灭。
		{
			HC595_SCLK=0;
			HC595_DS=(upright[j*2-2]&0x80);  
			upright[j*2-2]<<=1;
			HC595_SCLK=1;
		}
		for(i=0;i<8;i++)							//发送给每个模组的 U2 ，低电平整行可以亮，高电平整行全灭。
		{
			HC595_SCLK=0;
			HC595_DS=(Mould[j*2-1]&0x80);  
			Mould[j*2-1]<<=1;
			HC595_SCLK=1;
		}
		for(i=0;i<8;i++)							//发送给每个模组的 U1 ，低电平整行可以亮，高电平整行全灭。
		{
			HC595_SCLK=0;
			HC595_DS=(Mould[j*2-2]&0x80);  
			Mould[j*2-2]<<=1;
			HC595_SCLK=1;
		}
	}
	HC595_LCLK=1;									//锁存脉冲信号，上升沿。
	HC595_DS=1;										//延时
	HC595_LCLK=0;									//锁存脉冲信号复位。
}
void Timer0_Routnie (void) interrupt 1				//定时器中断处理，1.25ms一次。
{
	uchar i;	
	static uint kkk=0x0001;							//用于16列轮流控制
	TL0=0xfb;										//定时器赋初始值1.25ms，STC系列1T增强型51单片机。
	TH0=0xf2;										//定时器赋初始值1.25ms，STC系列1T增强型51单片机。
	//TL0=0x1E;										//定时器赋初始值1.25ms，传统12T指令周期的8951单片机。
	//TH0=0xFB;										//定时器赋初始值1.25ms，传统12T指令周期的8951单片机。
	
	
	Mould[7]=~LED2X64[i+97];						//i=0,把缓存的第97号字节发送到第四个模组的最左边的下8位。
	Mould[6]=~LED2X64[i+96];						//i=0,把缓存的第96号字节发送到第四个模组的最左边的上8位。
	Mould[5]=~LED2X64[i+65];						//i=0,把缓存的第65号字节发送到第三个模组的最左边的下8位。	
	Mould[4]=~LED2X64[i+64];						//i=0,把缓存的第64号字节发送到第三个模组的最左边的上8位。
	Mould[3]=~LED2X64[i+33];						//i=0,把缓存的第33号字节发送到第二个模组的最左边的下8位。
	Mould[2]=~LED2X64[i+32];						//i=0,把缓存的第32号字节发送到第二个模组的最左边的上8位。
	Mould[1]=~LED2X64[i+1];							//i=0,把缓存的第01号字节发送到第一个模组的最左边的下8位。
	Mould[0]=~LED2X64[i+0];							//i=0,把缓存的第00号字节发送到第一个模组的最左边的上8位。
	
	upright[7]=kkk/256;								//16位变量KKK，取高8位。控制第四个模组的0~7列。
	upright[6]=kkk%256;								//16位变量KKK，取低8位。控制第四个模组的8~15列。
	upright[5]=kkk/256;								//16位变量KKK，取高8位。控制第三个模组的0~7列。
	upright[4]=kkk%256;								//16位变量KKK，取低8位。控制第三个模组的8~15列。
	upright[3]=kkk/256;								//16位变量KKK，取高8位。控制第二个模组的0~7列。
	upright[2]=kkk%256;								//16位变量KKK，取低8位。控制第二个模组的8~15列。
	upright[1]=kkk/256;								//16位变量KKK，取高8位。控制第一个模组的0~7列。
	upright[0]=kkk%256;								//16位变量KKK，取低8位。控制第一个模组的8~15列。
	
	kkk=_irol_(kkk,1);								//循环控制0~16列
	i++;i++;
	i=i%32;
	HC595_Write();									//595的缓存Mould[8]、upright[8]，得到数据后，立马发送到芯片
}

void main()				   							//主函数
{
	uchar k=0,num=0,ww=0;
	uint timer=50000;
	Timer0_Init();
	for(k=0;k<128;k++)
	{
		LED2X64[k]=0;
	}
 	while(1)			   							//大循环
	{
		timer=50000;								//增强型1T指令STC12、STC15单片机
		//timer=1500;								//传统12T指令的8951单片机		
		while(timer--);								//延时，可以干其他事情
		
		LED2X64[128]=hanzi[ww][num]; 				//把要显示的内容放到最右边，上八位。
		num++;
		LED2X64[129]=hanzi[ww][num]; 				//把要显示的内容放到最右边，下八位。
		num++;
		if(num>=32)									//每个字最多0~31个字节的模。
		{
			num=0;
			ww++;									//决定了 指向每个汉字的数组
			if(ww>=10)								//总共10个汉字（英文都算16*16).
			{
				ww=0;
			}
		}
		for(k=0;k<128;k++)							//把数组2字节的内容拷贝给0字节，再把4拷贝给2，以此类推。
		{											//把数组3字节的内容拷贝给1字节，再把5拷贝给3，以此类推。
			LED2X64[k]=LED2X64[k+2];
		}
	}
}
