/*
 * I claim no ownership of the following code. It was given as for use in this course.
 */
#ifndef LCD_IO_H
#define LCD_IO_H

void LCD_init(void);
void LCD_ClearScreen(void);
void LCD_WriteCommand (unsigned char Command);
void LCD_WriteData (unsigned char Data);
void LCD_Cursor (unsigned char column);
void LCD_DisplayString(unsigned char column ,const unsigned char *string);
void delay_ms(int miliSec);

#endif //LCD_IO_H

