

// (ST7565P)

#include "STC15F2K60S2.H"
#include <string.h>
#include <stdlib.h>
#include <intrins.h>
#include "lcdtest.h"
#include "key.h"
#include "englishchar.h"
#include "Chinese16_16.h"
#include "ASCII16.H"

#define NOP_() {_nop_();}
//==============引脚定义==================


sbit LCD_LED = P2 ^ 5;//
sbit CS     = P2 ^ 4; //
sbit RST    = P2 ^ 3; //
sbit AO     = P2 ^ 2; //
sbit SDA    = P2 ^ 0; //
sbit SCK    = P2 ^ 1; //


sbit KEY_up     = P0 ^ 2;
sbit KEY_down   = P5 ^ 2;
sbit KEY_cancel = P0 ^ 3;
sbit KEY_ok     = P0 ^ 4;


#define KEY_Up      (!KEY_up)
#define KEY_Down    (!KEY_down)
#define KEY_Cancel  (!KEY_cancel)
#define KEY_Ok      (!KEY_ok)



#define PAGE_S  (0xb0)  //第0页地址
#define PAGE_E  (0xb8)  //最大页地址,第8页

#define COLUMN_OFFSET (4)
#define COLUMN_H   (0x10)  //列地址(高位)
#define COLUMN_L   (0x00)  //列地址(低位)




void DelayM (uint a)
{
	//延时函数 1MS/次
	uchar i;

	while (--a != 0) {
		for (i = 0; i < 125; i++); //一个 ; 表示空语句,CPU空转。
	}                      //i 从0加到125，CPU大概就耗时1毫秒
}


uchar getkey(void)
{
	if(KEY_Up){
		DelayM(1000);
		while(KEY_Up);
		return KEY_UP;
	}else if(KEY_Down){
		DelayM(1000);
		while(KEY_Down);
		return KEY_DOWN;
	}else if(KEY_Ok){
		DelayM(1000);
		while(KEY_Ok);
		return KEY_OK;
	}else if (KEY_Cancel){
		DelayM(1000);
		while(KEY_Cancel);
		return KEY_ESC;
	}
		else{
		return KEY_NULL;
	}
}


//=============写命令=================
void LcdSt7567_WriteCmd (cmd)
{
	uchar i, j;
	CS = 0;       //片选打开
	NOP_();
	AO = 0;       //选择写命令
	NOP_();
	SCK = 0;      //时钟置零
	NOP_();

	for (i = 0; i < 8; i++) {   //写八次
		j = cmd;                //把数据赋值给变量 j
		SCK = 0;                //时钟置零
		SDA = cmd & 0x80;       //因为数据始终是 1111 1111 这样的格式 但是一根线一次只能写一个数  故屏蔽第七位 取最高位写入
		NOP_();
		SCK = 1;                //上升沿写入数据
		NOP_();
		cmd = j << 1;           //数据左移一位  然后再次写入 直到八个二进制数字全部写入
	}

	NOP_();
	CS = 1;                     //片选至高位
	NOP_();                    //延时一下
	AO = 1;                     //释放读写端口
	NOP_();
	SDA = 1;                    //释放数据端口 这样使写入完成后 没有拉电流产生 可以省那么一点点的电 或者说 也可以并联其他器件到这个端口上同时使用
}
//=============写数据====================
void LcdSt7567_WriteData (dat)
{
	uchar i, j;
	CS = 0;                 //片选打开
	NOP_();
	AO = 1;                 //选择写数据（送显示）
	NOP_();
	SCK = 0;                //时钟置零
	NOP_();

	for (i = 0; i < 8; i++) {
		j = dat;            //变量赋值为需要的数据
		SCK = 0;            //时钟置低位
		SDA = dat & 0x80;   //舍弃低八位
		NOP_();
		SCK = 1;            //上升沿写数据
		NOP_();
		dat = j << 1;       //数据左移一位
	}

	NOP_();
	CS = 1;                 //片选置高
	NOP_();
	AO = 0;                 //读写置低
	NOP_();
	AO = 1;                 //释放读写端口
	NOP_();
	SDA = 1;                //释放数据端口
}
//==================液晶初始化=============
void LCD_ST7567_INIT (void)
{
#define INITCODELEN 13
	unsigned char code InitCode[INITCODELEN] = {0xae, 0x2f, 0x22, 0x81, 0x3f, 0xa2, 0xa1, 0xc0, 0x10, 0x00, 0xb0, 0xa6, 0xaf};
	unsigned char i;

	P2M1 = 0x00;
	P2M0 = 0x2F;
	LCD_LED = 1;


	RST = 1;                 //硬复位端口置高
	DelayM (10);              //延时一下等待上电稳定
	RST = 0;                 //硬件复位
	DelayM (10);                 //延时一下
	RST = 1;                 //复位完成

	for (i = 0; i < INITCODELEN ; i++) {
		LcdSt7567_WriteCmd (InitCode[i]);
	}

//	LcdSt7567_WriteCmd (0xE2); //软件复位
//	_nop_();
//	LcdSt7567_WriteCmd (0xA2); //偏压设置 第11命令 ok
//	LcdSt7567_WriteCmd (0x00); //偏压设置 0: 1/9 bias, 1: 1/7 bias (ST7565P)
//	LcdSt7567_WriteCmd (0xA0); //显示方向 左右 第8命令 ok
//	LcdSt7567_WriteCmd (0xC8); //显示方向 上下 第15命令 ok
//	LcdSt7567_WriteCmd (0x27); //电阻率设置 第17命令 ok
//	LcdSt7567_WriteCmd (0x81); //对比度           ok  18命令
//	LcdSt7567_WriteCmd (0x05); //                ok
//	LcdSt7567_WriteCmd (0xf8); //双字节命令 增压//19命令
//	LcdSt7567_WriteCmd (0x00);
//	_nop_();
//	LcdSt7567_WriteCmd (0x2F); //电源开
//	LcdSt7567_WriteCmd (0xA6); //白底黑字 a7为白字黑底
//	LcdSt7567_WriteCmd (0xA4); //正常显示
//	LcdSt7567_WriteCmd (0x40); //显示开始位置
//	LcdSt7567_WriteCmd (0xAF); //开显示
}

//===========LCD_清12864屏=============
void Lcd12864_ClearScreen (void)
{
	uchar i, j;

	for (i = 0; i < 8; i++) {           //竖着写8次
		LcdSt7567_WriteCmd (0xB0 + i);  //竖着写 地址
		LcdSt7567_WriteCmd (0x10);      //横着写 地址 高四位
		LcdSt7567_WriteCmd (0x04);      //横着写 地址 低四位

		for (j = 0; j < (128 + COLUMN_OFFSET); j++) {     //横着写128次
//			LcdSt7567_WriteData (((j/8)%2)? i%2?0x00:0xff:i%2?0xFF:0x00); //写入数据
			LcdSt7567_WriteData (0x00); //写入数据
		}
	}

//		cmd 0xb0,0x10,0x04,
//
//		dat 00 128个
//
//		cmd b1 10 04
//		dat 00 128个
//		cmd b2 10 04
//		dat 00 128个
//		cmd b3 10 04
//		dat 00 128个
//		cmd b4  10 04
//		dat 00 128个
//		cmd b5  10 04
//		dat 00 128个
//		......
//		cmd b8  10 04
//		dat 00 128个
}

/****************第X行X列 数组S0内容*********************************************/
// Row 横向地址 范围 0-3;
// Col 竖向地址 范围0-15;
// pStr 数组;
// c 反白显示 0正常 1反白
void WRITE_STRING816 (uchar Row, uchar Col,  uchar *pStr, uchar c)
{
	uchar page , col, col_h, col_l;
	uchar i, j;
	page = Row * 2 + PAGE_S;
	col = Col * 8 + COLUMN_OFFSET;
	col_h = ((col >> 4)  | COLUMN_H) & 0x1F;
	col_l = ((col & 0x0f) | COLUMN_L) & 0x0F;

	for (j = 0; j < 2; j++) {
		LcdSt7567_WriteCmd (col_h);
		LcdSt7567_WriteCmd (col_l);
		LcdSt7567_WriteCmd (page + j);

		for (i = 0;  i < 8; i ++) {
			if (c) {
				LcdSt7567_WriteData (~ (* (pStr++)));
			}
			else {
				LcdSt7567_WriteData (* (pStr++));
			}
		}
	}
}

/****************第X行X列 数组S0内容*********************************************/
// Row 横向地址 范围 0-3;
// Col 竖向地址 范围0-15;
// pStr 数组;
// c 反白显示 0正常 1反白
void WRITE_STRING1616 (uchar Row, uchar Col,  uchar *pStr)
{
	uchar page , col, col_h, col_l;
	uchar i, j;
	page = Row * 2 + PAGE_S;
	col = Col * 8 + COLUMN_OFFSET;
	col_h = ((col >> 4)  | COLUMN_H) & 0x1F;
	col_l = ((col & 0x0f) | COLUMN_L) & 0x0F;

	for (j = 0; j < 2; j++) {
		LcdSt7567_WriteCmd (col_h);
		LcdSt7567_WriteCmd (col_l);
		LcdSt7567_WriteCmd (page+ j );

		for (i = 0;  i < 16; i ++) { 
			LcdSt7567_WriteData (* (pStr++));
			if(i==8){
				LcdSt7567_WriteCmd (page + j );
			}
		}
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
			WRITE_STRING1616(page,column,&Chinese1616[GBKPos].ChineseMAP);
			column += 2;
		}
		p+=2;
	}
	
	
//	display1616(0,0,&zhongguo[2]);
//	display1616(0,32,&zhongguo[36]);
}

/****************第X行X列 数组S0内容*********************************************/
// Row 横向地址 范围 0-7;
// Col 竖向地址 范围0-15;
// pStr 数组;
// c 反白显示 0正常 1反白
void WRITE_STRING88 (uchar Row, uchar Col,  uchar *pStr, uchar c)
{
	uchar page , col, col_h, col_l;
	uchar i, j;
	page = Row  + PAGE_S;
	col = Col * 8 + COLUMN_OFFSET;
	col_h = ((col >> 4)  | COLUMN_H) & 0x1F;
	col_l = ((col & 0x0f) | COLUMN_L) & 0x0F;

	for (j = 0; j < 1; j++) {
		LcdSt7567_WriteCmd (col_h);
		LcdSt7567_WriteCmd (col_l);
		LcdSt7567_WriteCmd (page + j);

		for (i = 0;  i < 8; i ++) {
			if (c) {
				LcdSt7567_WriteData (~ (* (pStr++)));
			}
			else {
				LcdSt7567_WriteData (* (pStr++));
			}
		}
	}
}

void WRITE_STRING88_ADDR (uchar Row, uchar Col/*,  uchar *pStr, uchar c*/)
{
	uchar page , col, col_h, col_l;
	uchar/* i,*/ j;
	page = Row  + PAGE_S;
	col = Col * 8 + COLUMN_OFFSET;
	col_h = ((col >> 4)  | COLUMN_H) & 0x1F;
	col_l = ((col & 0x0f) | COLUMN_L) & 0x0F;

//	for (j = 0; j < 1; j++) {
		LcdSt7567_WriteCmd (col_h);
		LcdSt7567_WriteCmd (col_l);
		LcdSt7567_WriteCmd (page + j);

//		for (i = 0;  i < 8; i ++) {
//			if (c) {
//				LcdSt7567_WriteData (~ (* (pStr++)));
//			}
//			else {
//				LcdSt7567_WriteData (* (pStr++));
//			}
//		}
//	}
}



void displayPhoto (uchar *pic)
{
	uchar a, x;

	for (a = 0; a < 8; a++) {
		LcdSt7567_WriteCmd (0xB0 + a);   //设置页地址
		LcdSt7567_WriteCmd (0x10);     //设置列地址“高四位”- 0000
		LcdSt7567_WriteCmd (0x00);     //设置列地址“第四位”- 0000

		for (x = 0;  x < 128; x ++) {
			LcdSt7567_WriteData (*pic++);
		}
	}
}

void DisplayString1608(unsigned char page, unsigned char column, unsigned char *p)
{
    unsigned char i=0;//, j;
    while(*p)//判断字符数组是否结束
    {
		WRITE_STRING816 (page, column + i++  , &ASCII16[(*p - 0x20)][0],0);
		p++;
    }
}

void main  (void)
{

	LCD_ST7567_INIT();          //液晶初始化
	Lcd12864_ClearScreen();     //液晶清屏

//	while (1) 
	{
//		if (KEY_Up) {
			DisplayString1608 (1, 4, "bacon05");
			DisplayString1608 (3, 10, "4GOT10");
		
//			DisplayString1616 (1, 2, "小小小小晓茜" ); //显示s0数组内容	
		
//			DisplayString1616 (3, 4, "中国建设银行" ); //显示s0数组内容	
		while(1);
//		}
//		else if (KEY_Down) {
//			WRITE_STRING816 (0, 1, &s0[0][16], 0);
//		}
//		else if (KEY_Ok) {
//			WRITE_STRING816 (0, 2, &s0[1][16], 0);
//		}
//		else if (KEY_Cancel) {
//			WRITE_STRING816 (0, 3, &s0[2][16], 0);
//		}

//		WRITE_STRING816(0,4,&s0[3][16],0);
//		WRITE_STRING816(0,5,&s0[4][16],0);
//		WRITE_STRING816(0,6,&s0[5][16],0);
//		WRITE_STRING816(0,7,&s0[6][16],0);
//		WRITE_STRING816(0,8,&s0[7][16],0);
//		WRITE_STRING816(0,9,&s0[8][16],0);
//		WRITE_STRING816(0,10,&s0[9][16],0);
//		WRITE_STRING816(0,11,&s0[10][16],0);
//		WRITE_STRING816(0,12,&s0[11][16],0);
//		WRITE_STRING816(0,13,&s0[12][16],0);
//		WRITE_STRING816(0,14,&s0[13][16],0);
//		WRITE_STRING816(0,15,&s0[14][16],0);
//
//		WRITE_STRING816(2,0,&s0[15][16],0); //显示s0数组内容
//		WRITE_STRING816(2,1,&s0[16][16],0);
//		WRITE_STRING816(2,2,&s0[17][16],0);
//		WRITE_STRING816(2,3,&s0[18][16],0);
//		WRITE_STRING816(2,4,&s0[19][16],0);
//		WRITE_STRING816(2,5,&s0[20][16],0);
//		WRITE_STRING816(2,6,&s0[21][16],0);
//		WRITE_STRING816(2,7,&s0[22][16],0);
//		WRITE_STRING816(2,8,&s0[23][16],0);
//		WRITE_STRING816(2,9,&s0[24][16],0);
//		WRITE_STRING816(2,10,&s0[25][16],0);
//		WRITE_STRING816(2,11,&s0[26][16],0);
//		WRITE_STRING816(2,12,&s0[27][16],0);
//		WRITE_STRING816(2,13,&s0[0][0],0);
//		WRITE_STRING816(2,14,&s0[0][16],0);
//		WRITE_STRING816(2,15,&s0[1][16],0);
//
//		DelayM(2000);                   //延时
//		Lcd12864_ClearScreen();         //清屏
//		WRITE_STRING816(1,0,&s0[0][0],0); //显示s0数组内容
//		WRITE_STRING816(1,1,&s0[0][16],0);
//		WRITE_STRING816(1,2,&s0[1][16],0);
//		WRITE_STRING816(1,3,&s0[2][16],0);
//		WRITE_STRING816(1,4,&s0[3][16],0);
//		WRITE_STRING816(1,5,&s0[4][16],0);
//		WRITE_STRING816(1,6,&s0[5][16],0);
//		WRITE_STRING816(1,7,&s0[6][16],0);
//		WRITE_STRING816(1,8,&s0[7][16],0);
//		WRITE_STRING816(1,9,&s0[8][16],0);
//		WRITE_STRING816(1,10,&s0[9][16],0);
//		WRITE_STRING816(1,11,&s0[10][16],0);
//		WRITE_STRING816(1,12,&s0[11][16],0);
//		WRITE_STRING816(1,13,&s0[12][16],0);
//		WRITE_STRING816(1,14,&s0[13][16],0);
//		WRITE_STRING816(1,15,&s0[14][16],0);
//
//		WRITE_STRING816(3,0,&s0[15][16],0); //显示s0数组内容
//		WRITE_STRING816(3,1,&s0[16][16],0);
//		WRITE_STRING816(3,2,&s0[17][16],0);
//		WRITE_STRING816(3,3,&s0[18][16],0);
//		WRITE_STRING816(3,4,&s0[19][16],0);
//		WRITE_STRING816(3,5,&s0[20][16],0);
//		WRITE_STRING816(3,6,&s0[21][16],0);
//		WRITE_STRING816(3,7,&s0[22][16],0);
//		WRITE_STRING816(3,8,&s0[23][16],0);
//		WRITE_STRING816(3,9,&s0[24][16],0);
//		WRITE_STRING816(3,10,&s0[25][16],0);
//		WRITE_STRING816(3,11,&s0[26][16],0);
//		WRITE_STRING816(3,12,&s0[27][16],0);
//		WRITE_STRING816(3,13,&s0[0][0],0);
//		WRITE_STRING816(3,14,&s0[0][16],0);
//		WRITE_STRING816(3,15,&s0[1][16],0);
//		DelayM(2000);                 //延时
//		Lcd12864_ClearScreen();       //清屏
//		DelayM (2000);                //延时
//		Lcd12864_ClearScreen();       //清屏
	}
}