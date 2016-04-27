/***************************************************************

sbit LCD_LED = P2 ^ 5;//
sbit CS = P2 ^ 4; // 
sbit RST = P2 ^ 3; // 
sbit AO = P2 ^ 2; // 
sbit SDA = P2 ^ 0; // 
sbit SCK = P2 ^ 1; //  

sbit KEY_up		= P0^2;
sbit KEY_down	=P5^2;
sbit KEY_cancel	=P0^3;
sbit KEY_ok		=P0^4;






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


#define delay() //{_nop_();_nop_();}
 
void timer(unsigned int t)
{
    unsigned int i;
    unsigned char j;
    for(i=0; i<t; i++)
        for(j=0; j<200; j++);
}
//初始化液晶屏
void InitLCD()
{
    timer(1000);//复位前的延时必须大于3ms
    RS = 0;
    timer(1000);//RS置低时间必须大于3us
    RS = 1;
    timer(1000);//复位完成后写入操作命令前的延时必须大于150ms
    
    wlc(0xE2);//系统设置复位
    timer(100);//这个延时可以去掉
    
/*
    //以下参数在系统设置复位后均置为默认值，可以不用更改    
    wlc(0x2e);//设置电源为内部升压、电荷泵工作电流1.4mA
    wlc(0xeb);//设置偏压比，采用默认值，不要更改！
    wlc(0xa1);//设置帧数，采用默认值，0:76fps,1:95fps,2:132fps,3:168fps    
    wlc(0x89);//自动换行
*/
    wlc(0x81);//设置对比度，该命令后必须紧跟对比度的值，取值范围0-255
    wlc(0x84);//设置对比度的值为0x84
    wlc(0xc2);//设置行列扫描顺序，c0 默认扫描顺序，c2 翻转x，c4 翻转y，c6 翻转xy
    wlc(0xAF);//开启显示
    timer(1000);
}
//写入液晶屏指令，write lcd command
void wlc(unsigned char dat)
{
    unsigned char i;
    CD = 0;
    
    for(i=0; i<8; i++)
    {
        dat = dat << 1;
        SCLK = 0;
        delay();//数据手册中的要求是保持时间大于100ns，51的速度比这个慢，可以不要延时
        SDAT = CY;
        delay();//同上
        SCLK = 1;
        delay();//同上
    }
}
//写入液晶屏数据，write lcd data
void wld(unsigned char dat)
{
    unsigned char i;
    CD = 1;
    
    for(i=0; i<8; i++)
    {
        dat = dat << 1;
        
        SCLK = 0;
        delay();//数据手册中的要求是保持时间大于100ns，51的速度比这个慢，可以不要延时
        SDAT = CY;
        delay();//同上
        SCLK = 1;
        delay();//同上
    }
}
//清屏
void CleanLCD()
{
    unsigned int i = 0;
    wlc(0xB0);
    wlc(0x00);
    wlc(0x10);
    CD = 1;
    SDAT = 0;
    SCLK = 0;
    
    while(i < 24576)
    {
        SCLK = ~SCLK ;
        i++;
    }
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
