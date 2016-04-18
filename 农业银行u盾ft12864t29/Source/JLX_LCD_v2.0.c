/***************************************************************
 



晶联讯LCD专用驱动
程 序 名：晶联讯LCD专用驱动
编 写 人：ifm
编写时间：2015年6月27日
硬件支持：STC系列单片机
接口说明：参见注释
修改日志：
2015.7.30
新增以下函数
	LCDAddress,
	display0805,
	DisplayString,
	DisplayNumber,
	display1608,
	display1616,
	display3232,
	display6464,
	display19264,
考虑到display0805这个函数的功能实在是鸡肋，屏蔽其以节约系统资源
2015.8.1
新增函数DisplayString1608,DisplayNumber1608
修改函数名DisplayString,DisplayNumber为DisplayString0805,DisplayNumber0805
恢复display0805以供DisplayNumber0805调用。
修改其他细节
2015.8.2
新增函数DrawPoint
2015.8.20
取消片选线。考虑到一般不会同时操作两块屏幕，取消片选线节约资源。
硬件中片选线直接接地。
版    本：V2.0
备    注：本驱动仅用于分辨率192*64的液晶屏，其他分辨率请自行修改
***************************************************************/

#include "string.h"
#include "JLX_LCD_v2.0.h"
#include "ASCII.h"
#include "ASCII16.h"
#include "chinese16_16.h"


#define delay()	{	_nop_();_nop_();_nop_();_nop_(); _nop_();_nop_(); 	\
				}


//1100, 100ms@12m				
void timer(unsigned int t)
{
    unsigned int i;
    unsigned char j;
    for(i=0; i<t; i++)
        for(j=0; j<200; j++);
}




//void testio(void)
//{
//	while(1)
//	{
//		SDAT  = SCLK =CD =RS =CS =LCD_LED = 1;
//		timer(10000);
//		SDAT  = SCLK =CD =RS =CS =LCD_LED = 0;
//		timer(10000);
//	}
//}


void wrLCD( unsigned char dat ,bit Cmd0_Dat1)
{
	unsigned char i;

	CD = Cmd0_Dat1?1:0;
	SCLK = 0; 
	for(i=0; i<8; i++)
    {
        
        SDAT = (dat&0x80) ;
        delay();//同上
        SCLK = 1;
        delay();//同上
		SCLK = 0;
		dat <<= 1;
    }
	SDAT = 0;
}

//写入液晶屏指令，write lcd command
void wlc(unsigned char dat)
{
    wrLCD(dat, 0);
}
//写入液晶屏数据，write lcd data
void wld(unsigned char dat)
{
    wrLCD(dat, 1);
}

//初始化液晶屏
void InitLCD()
{
	
	SDAT  = SCLK =CD =RS =CS =LCD_LED = 0;

    timer(30);//复位前的延时必须大于3ms    
    RS = 1;
//    timer(10);//复位完成后写入操作命令前的延时必须大于150ms
	
	wlc(0x2f);//0010 1111
	wlc(0x22);//0010 0010
	wlc(0x81);//1000 0001
	wlc(0x3f);//0011 1111
	wlc(0xa2);//1010 0010
	wlc(0xa1);//1010 0001
	wlc(0xc0);//1100 0000
	wlc(0x10);//0001 0000
	wlc(0x00);//0000 0000
	wlc(0xb0);//1011 0000
	wlc(0xaf);//1010 1111
	wlc(0xa6);//1010 0110
	//

//	CS =CD = 1;
//	timer(1);
//	CS  = 0;
}

//清屏
void CleanLCD()
{
	
    unsigned int i =0;
	unsigned int j=0; 
//	timer(4);
	for(j=0;j<8;j++)
	{ 
		LCD_LED= ~LCD_LED;
		
		wlc(0xb0+j);//1011 0000	//1011 0001 
		wlc(0x00);//0000 0000	//0000 0000
		wlc(0x10);//0001 0000	//0001 0000
		wlc(0x40);//0100 0000	//0100 0000
		
		for(i=0;i < 128+4;i++)
		{
			wld(0x00);
		}
	}

//	CS = 1;
//	timer(1);
//	CS = 0;

	for(j=0;j<8;j++)
	{ 
		LCD_LED= ~LCD_LED;
		
		wlc(0xb0+j);//1011 0000	//1011 0111 
		wlc(0x04);//0000 0100	 
		wlc(0x10);//0001 0000	 
		wlc(0x40);//0100 0000	 
		
		for(i=0;i < 128+4;i++)
		{
			wld(0x01);
		}
	}

	SDAT  = SCLK =CD  =LCD_LED =0;	 
//	CS = 1;
	while(1);
}



//设置写入地址，页、列均从0开始
void LCDAddress(unsigned char page, unsigned char column)
{
    wlc(0xB0 | page);//也可以用0xB0 + page，但位运算速度比加法速度快
    wlc(0x0F & column);//发送列的低四位
    wlc(0x10 | column >> 4);//发送列的高四位，移位运算的优先级高于或运算
}
//在指定行列画一个点，会覆盖原来的数据
void DrawPoint(unsigned char line, unsigned char column)
{
    LCDAddress(line>>3, column);
    wld(0x01 << (line & 0x07));
}
//在指定页、指定列写入8*5的图形数据
void display0805(unsigned char page, unsigned char column, unsigned char *p)
{
    unsigned char i;
    
    LCDAddress(page, column);
    
    for(i=0; i<5; i++)
    {
        wld(*p);
        p++;
    }
}
//在指定页、指定列写入16*8的图形数据
void display1608(unsigned char page, unsigned char column, unsigned char *p)
{
    unsigned char i, j;
    for(i=0; i<2; i++)
    {
        LCDAddress(page + i, column);
        for(j=0; j<8; j++)
        {
            wld(*p);
            p++;
        }
    }
}
//在指定页、指定列写入16*16的图形数据
void display1616(unsigned char page, unsigned char column, unsigned char *p)
{
    unsigned char i, j;
    for(i=0; i<2; i++)
    {
        LCDAddress(page + i, column);
        for(j=0; j<16; j++)
        {
            wld(*p);
            p++;
        }
    }
}


//在指定页、指定列写入32*32的图形数据
void display3232(unsigned char page, unsigned char column, unsigned char *p)
{
    unsigned char i, j;
    for(i=0; i<4; i++)
    {
		LCDAddress(page + i, column);
		for(j=0; j<32; j++)
		{
            wld(*p);
            p++;
        }
    }
}
//在指定列写入64*64的图形数据
void display6464(unsigned char column, unsigned char *p)
{
    unsigned char i, j;
    for(i=0; i<8; i++)
    {
        LCDAddress(i, column);
        for(j=0; j<64; j++)
        {
            wld(*p);
            p++;
        }
    }
}
void display19264(unsigned char *p)
{
    unsigned int i;
    LCDAddress(0, 0);
    
    for(i=0; i<1536; i++)
    {
        wld(*p);
        p++;
    }
}
//在指定页、指定列显示尺寸为0805的英文字符串
void DisplayString0805(unsigned char page, unsigned char column, unsigned char *p)
{
    unsigned char i=0, j, k;
    
    LCDAddress(page, column);
    while(p[i])//判断字符数组是否结束
    {
        k = p[i] - 0x20;//获取当前字符减32后的值
        for(j=0; j<5; j++)
        {
            wld(ASCII[j + k * 5]);
		}
		wld(0x00);//输出一列空白，让字符隔开
		i++;
	}
}
//在指定页、指定列显示尺寸为0805的数字
void DisplayNumber0805(unsigned char page, unsigned char column, unsigned int a)
{
    char i=0, j;//注意这里用的是char，不是unsigned char
    for(i=4; i>=0; i--)//判断数字是否结束
    {
        LCDAddress(page, column + i * 6);
        j = a % 10 + 16;//获取当数字加16后的值，这个值就是这个数字在精简版ASCII表中的序号
        display0805(page, column + i * 6, &ASCII[j*5]);
        wld(0x00);//输出一列空白，让字符隔开
        a /= 10;
    }
}
//在指定页、指定列显示尺寸为1608的英文字符串
void DisplayString1608(unsigned char page, unsigned char column, unsigned char *p)
{
    unsigned char i=0, j;
    while(p[i])//判断字符数组是否结束
    {
		j = p[i] - 0x20;//获取当前字符减32后的值
		display1608(page, column + i*8, &ASCII16[j*16]);
		i++;
    }
}
//在指定页、指定列显示尺寸为1608的数字
void DisplayNumber1608(unsigned char page, unsigned char column, unsigned int a)
{
    char i=0, j;//注意这里用的是char，不是unsigned char
    for(i=4; i>=0; i--)//判断数字数组是否结束
    {
		j = a % 10 + 16;//获取当数字加16后的值，这个值就是这个数字在精简版ASCII表中的序号
		display1608(page, column + i * 8, &ASCII16[j * 16]);//这里j*16以后就是它的图形的位置
		a /= 10;
    }
}

unsigned char FindChnGBK(unsigned char *p, unsigned int *GBKpos)
{
	unsigned int i,j;
	j = sizeof(Chinese1616)/sizeof(tagChinese); 
	for(i=0;i<j;i++)
	{ 
		if (!memcmp(&Chinese1616[i].ChineseGBK,p,2)){
			(*GBKpos) = i;
			return 1;
		}
	}
	return 0;	
}


void DisplayString1616(unsigned char page, unsigned char column,unsigned char *p)
{
	unsigned int GBKPos;
	while(*p){
		if (FindChnGBK(p, &GBKPos)){
			display1616(page,column,&Chinese1616[GBKPos].ChineseMAP);
			column += 16;
		}
		p+=2;
	}
	
	
//	display1616(0,0,&zhongguo[2]);
//	display1616(0,32,&zhongguo[36]);
}
