#ifndef __LCDTEST_H__
#define __LCDTEST_H__


typedef  unsigned char uchar;
typedef  unsigned int uint;

extern void LCD_ST7567_INIT (void);
extern void Lcd12864_ClearScreen (void);
extern void WRITE_STRING816 (uchar Row, uchar Col,  uchar *pStr, uchar c);
extern void WRITE_STRING88 (uchar Row, uchar Col,  uchar *pStr, uchar c);
extern void WRITE_STRING88_ADDR (uchar Row, uchar Col/*,  uchar *pStr, uchar c*/);
extern void LcdSt7567_WriteData (dat);
extern void LcdSt7567_WriteCmd (cmd);
extern uchar getkey(void);

#endif