/*
 * File: lcd.cpp
 * Author : Justin Doss
 *
 * LCD display diver for the {insert device}
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "defs.h"
#include "string.h"

#define LCD_FUNC_CLR  0x01
#define LCD_FUNC_HOME 0x02
#define LCD_FUNC_MODE 0x04
#define LCD_FUNC_DIS  0x08
#define LCD_FUNC_SHFT 0x10
#define LCD_FUNC_SET  0x20
#define LCD_FUNC_CRAM 0x40
#define LCD_FUNC_DRAM 0x80

#define LCD_MODE_8BIT 0x10
#define LCD_MODE_4LINE 0x08
#define LCD_MODE_LFONT 0x04

#define LCD_ENTRY_NSHIFT 0x00
#define LCD_ENTRY_SHIFT  0x01
#define LCD_ENTRY_INCREMENT 0x02
#define LCD_ENTRY_DECREMENT 0x00

#define LCD_BLNK_ON  0x01
#define LCD_BLNK_OFF 0x00
#define LCD_CURS_ON  0x02
#define LCD_CURS_OFF 0x00
#define LCD_DISP_ON  0x04
#define LCD_DISP_OFF 0x00

#define LCD_DISP_MOVE  0x08
#define LCD_CURS_MOVE  0x00
#define LCD_MOVE_RIGHT 0x04
#define LCD_MOVE_LEFT  0x00

#define LCD_ROWS 4
#define LCD_COLS 20

volatile unsigned char curRow = 0;
volatile unsigned char curCol = 0;

volatile char disp_func = 0;
volatile char disp_mode = 0;
volatile char disp_entry_mode = 0;

/// private
void sendCommand(char);
void sendData(char);
void write4bits(char);
void pulseEnable();

void LCD_init()
{
    INITPIN(LCD_ENABLE, OUTPUT, LOW);
    INITPIN(LCD_RS, OUTPUT, LOW);
    INITPIN(LCD_DATA0, OUTPUT, LOW);
    INITPIN(LCD_DATA1, OUTPUT, LOW);
    INITPIN(LCD_DATA2, OUTPUT, LOW);
    INITPIN(LCD_DATA3, OUTPUT, LOW);

    disp_func = LCD_FUNC_SET | LCD_MODE_4LINE; // 4 bit interface mode, 2 lines
    write4bits(disp_func >> 4);

    sendCommand(disp_func);
    sendCommand(disp_func);

    disp_mode = LCD_FUNC_DIS | LCD_DISP_ON | LCD_CURS_OFF | LCD_BLNK_OFF;
    sendCommand(disp_mode);

    LCD_clear();

    disp_entry_mode = LCD_FUNC_MODE | LCD_ENTRY_NSHIFT | LCD_ENTRY_INCREMENT;
    sendCommand(disp_entry_mode);

    LCD_home();
}

void LCD_clear()
{
    sendCommand(LCD_FUNC_CLR);
    curRow = 0;
    curCol = 0;
    _delay_ms(2);
}

void LCD_home()
{
    sendCommand(LCD_FUNC_HOME);
    curRow = 0;
    curCol = 0;
    _delay_ms(2);
}

void LCD_cursorOn()
{
    disp_mode |= LCD_CURS_ON;
    sendCommand(disp_mode);
}

void LCD_cursorOff()
{
    disp_mode &= ~LCD_CURS_ON;
    sendCommand(disp_mode);
}

void LCD_blinkOn()
{
    disp_mode |= LCD_BLNK_ON;
    sendCommand(disp_mode);
}

void LCD_blinkOff()
{
    disp_mode &= ~LCD_BLNK_ON;
    sendCommand(disp_mode);
}

void LCD_setCursor(unsigned char row, unsigned char col)
{
    curRow = row;
    curCol = col;
    col += (row / 2 * 20) + ((row % 2) * 64);
    sendCommand(LCD_FUNC_DRAM | col);
}

void LCD_print(const char *msg)
{
    for (; *msg != 0 && curCol < LCD_COLS; msg++) //&& *msg != '\n' && *msg != '\r'
    {
        sendData(*msg);
        curCol++;
    }
}

void LCD_printAt(const char *msg, unsigned char row, unsigned char col)
{
    LCD_setCursor(row, col);
    LCD_print(msg);
}

void LCD_printRight(const char *msg)
{
	unsigned char l = strlen(msg);
	if (l > LCD_COLS)
	{
		l = LCD_COLS;
	}
    LCD_printAt(msg, curRow, LCD_COLS - l);
}

void LCD_printCenter(char *msg, unsigned char row)
{
	unsigned char l = strlen(msg);
	if (l > LCD_COLS)
	{
		l = LCD_COLS;
	}
    LCD_printAt(msg, row, (LCD_COLS - l) / 2);
}

void sendCommand(char cmd)
{
    SETPIN(LCD_RS, LOW);
    _delay_us(40);
    write4bits(cmd >> 4);
    write4bits(cmd & 0x0F);
    SETPIN(LCD_RS, HIGH);
}

void sendData(char data)
{
    write4bits(data >> 4);
    write4bits(data & 0x0F);
}

void write4bits(char data)
{
    SETPIN(LCD_DATA0, data & 0x01);
    SETPIN(LCD_DATA1, data & 0x02);
    SETPIN(LCD_DATA2, data & 0x04);
    SETPIN(LCD_DATA3, data & 0x08);
    pulseEnable();
}

void pulseEnable()
{
    SETPIN(LCD_ENABLE, HIGH);
    _delay_us(1200);
    SETPIN(LCD_ENABLE, LOW);
    _delay_us(200);
}
