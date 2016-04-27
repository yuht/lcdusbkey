#include "STC15F2K60S2.H"
#include "JLX_LCD_v2.0.h"

extern unsigned char code zhongguo[];

void main()
{
	P5M1 &= ~(0xff);
	P5M0 |= 0xff;
	
	P0M1 &= ~(0xff);
	P0M0 |= (0xff);
	
	P6M1 &= ~(0xff);
	P6M0 |= (0xff);
	
	
	LCD_LED = 1;
	
    InitLCD();
    CleanLCD(0xf0);
//    DisplayString1608(0,0,"ABCDEFGHIJKLMNOPQRSTUVWX");
//    DisplayString1608(2,0,"YZabcdefghijklmnopqrstuv");
//    DisplayString1608(4,0,"wxyz12345678901234567890");
//    DisplayString1608(6,0,"123456789012345678901234");
//------------------------------------------------	

	DisplayString1608(6,8*5,"4GOT10");
	
//	DisplayString1616(6,96,"中国建设银行");
//------------------------------------------------	
//	DisplayString1616(6,96,"中国建设银行");
	
    while(1);
}