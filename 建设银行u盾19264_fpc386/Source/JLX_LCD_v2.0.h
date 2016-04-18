#ifndef __JLX_LCD_V2_0_H__
#define __JLX_LCD_V2_0_H__

#include <intrins.h>
#include "STC15F2K60S2.H"


sbit SDAT = P2 ^ 0;//数据
sbit SCLK = P2 ^ 1;//时钟
sbit CD = P2 ^ 2;//寄存器选择，高电平传输数据，低电平传输指令
sbit RS = P2 ^ 3;//复位，低电平复位，复位完成后，回到高电平
sbit CS = P2 ^ 4;//片选，低电平片选
sbit LCD_LED = P2 ^ 5;//


void timer(unsigned int t);
void wlc(unsigned char dat);//写入液晶屏指令，write lcd command
void wld(unsigned char dat);//写入液晶屏数据，write lcd data
void InitLCD();//初始化液晶屏
void CleanLCD();//清屏
void LCDAddress(unsigned char page, unsigned char column);//设置写入地址，页、列均从0开始
void DrawPoint(unsigned char line, unsigned char column);//在指定行列画一个点，会覆盖原来的数据
void display0805(unsigned char page, unsigned char column, unsigned char *p);//在指定页、指定列写入8*5的图形数据
void display1608(unsigned char page, unsigned char column, unsigned char *p);//在指定页、指定列写入16*8的图形数据
void display1616(unsigned char page, unsigned char column, unsigned char *p);//在指定页、指定列写入16*16的图形数据
void display3232(unsigned char page, unsigned char column, unsigned char *p);//在指定页、指定列写入32*32的图形数据
void display6464(unsigned char column, unsigned char *p);//在指定列写入64*64的图形数据
void display19264(unsigned char *p);//这个函数的功能不解释
//在指定页、指定列显示尺寸为0805的英文字符串，支持自动换行。不支持%d这样的，别的功能和printf()一样。
void DisplayString0805(unsigned char page, unsigned char column, unsigned char *p);
//在指定页、指定列显示尺寸为0805的数字，弥补DisplayString()不能动态显示的缺陷。
//注意：数据类型是无符号整型；不支持自动换行，使用时显示的数字不要超出边界！！
void DisplayNumber0805(unsigned char page, unsigned char column, unsigned int a);
//在指定页、指定列显示尺寸为1608的英文字符串，不支持自动换行！不支持自动换行！不支持自动换行！
void DisplayString1608(unsigned char page, unsigned char column, unsigned char *p);
//在指定页、指定列显示尺寸为1608的数字，细节问题同DisplayNumber
void DisplayNumber1608(unsigned char page, unsigned char column, unsigned int a);

//在指定页、指定列显示尺寸为1616的字符串
void DisplayString1616(unsigned char page, unsigned char column,unsigned char *p);

#endif
