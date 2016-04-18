#include "STC15F2K60S2.H"
#include "JLX_LCD_v2.0.h"

extern unsigned char code zhongguo[];

void main()
{
	P2M1 = 0x00;
	P2M0 = 0x2F;
	LCD_LED = 1;
	CS = 0;
    InitLCD();
    CleanLCD();
//    DisplayString1608(0,0,"ABCDEFGHIJKLMNOPQRSTUVWX");
//    DisplayString1608(2,0,"YZabcdefghijklmnopqrstuv");
//    DisplayString1608(4,0,"wxyz12345678901234567890");
//    DisplayString1608(6,0,"123456789012345678901234");
//------------------------------------------------	
	DisplayString1608(2,8*8,"bacon05");
	DisplayString1608(6,8*18,"4GOT10");
	
//	DisplayString1616(6,96,"中国建设银行");
//------------------------------------------------	
//	DisplayString1616(6,96,"中国建设银行");
	
    while(1);
}