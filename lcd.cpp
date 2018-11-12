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

static LCD display(4, 20);

LCD* LCD::get()
{
    return &display;
}

LCD::LCD(unsigned char rows, unsigned char cols)
{
    this->rows = rows;
    this->cols = cols;
}

void LCD::init()
{
    INITPIN(LCD_ENABLE, OUTPUT, LOW);
    INITPIN(LCD_RS, OUTPUT, LOW);
    INITPIN(LCD_DATA0, OUTPUT, LOW);
    INITPIN(LCD_DATA1, OUTPUT, LOW);
    INITPIN(LCD_DATA2, OUTPUT, LOW);
    INITPIN(LCD_DATA3, OUTPUT, LOW);

    this->curRow = 0;

    this->disp_func = LCD_FUNC_SET | LCD_MODE_4LINE; // 4 bit interface mode, 2 lines
    write4bits(this->disp_func >> 4);

    sendCommand(this->disp_func);
    sendCommand(this->disp_func);

    this->disp_mode = LCD_FUNC_DIS | LCD_DISP_ON | LCD_CURS_OFF | LCD_BLNK_OFF;
    sendCommand(this->disp_mode);

    clear();

    this->disp_entry_mode = LCD_FUNC_MODE | LCD_ENTRY_NSHIFT | LCD_ENTRY_INCREMENT;
    sendCommand(this->disp_entry_mode);

    home();
}

void LCD::clear()
{
    sendCommand(LCD_FUNC_CLR);
    this->curRow = 0;
    this->curCol = 0;
    _delay_ms(2);
}

void LCD::home()
{
    sendCommand(LCD_FUNC_HOME);
    this->curRow = 0;
    this->curCol = 0;
    _delay_ms(2);
}

void LCD::cursorOn()
{
    this->disp_mode |= LCD_CURS_ON;
    sendCommand(this->disp_mode);
}

void LCD::cursorOff()
{
    this->disp_mode &= ~LCD_CURS_ON;
    sendCommand(this->disp_mode);
}

void LCD::blinkOn()
{
    this->disp_mode |= LCD_BLNK_ON;
    sendCommand(this->disp_mode);
}

void LCD::blinkOff()
{
    this->disp_mode &= ~LCD_BLNK_ON;
    sendCommand(this->disp_mode);
}

void LCD::setCursor(unsigned char row, unsigned char col)
{
    this->curRow = row;
    this->curCol = col;
    col += (row / 2 * 20) + ((row % 2) * 64);
    sendCommand(LCD_FUNC_DRAM | col);
}

void LCD::print(const char *msg)
{
    for (; *msg != NULL && this->curCol < this->cols; msg++)
    {
        sendData(*msg);
        this->curCol++;
    }
}

void LCD::printRight(const char *msg)
{
    this->printAt(msg, this->curRow, this->cols - strlen(msg));
}

void LCD::printAt(const char* msg, unsigned char row, unsigned char col)
{
    this->setCursor(row, col);
    this->print(msg);
}

void LCD::printCenter(char* msg, unsigned char row)
{
    this->printAt(msg, row, (this->cols - strlen(msg)) / 2);
}

void LCD::sendCommand(char cmd)
{
    SETPIN(LCD_RS, LOW);
    _delay_us(20);
    write4bits(cmd >> 4);
    write4bits(cmd & 0x0F);
    SETPIN(LCD_RS, HIGH);
}

void LCD::sendData(char data)
{
    write4bits(data >> 4);
    write4bits(data & 0x0F);
}

void LCD::write4bits(char data)
{
    SETPIN(LCD_DATA0, data & 0x01);
    SETPIN(LCD_DATA1, data & 0x02);
    SETPIN(LCD_DATA2, data & 0x04);
    SETPIN(LCD_DATA3, data & 0x08);
    pulseEnable();
}

void LCD::pulseEnable()
{
    SETPIN(LCD_ENABLE, HIGH);
    _delay_us(1200);
    SETPIN(LCD_ENABLE, LOW);
    _delay_us(120);
}
