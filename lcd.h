/*
 * File: lcd.h
 * Author : Justin Doss
 *
 * LCD display diver for the {insert device}
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef LCD_H
#define LCD_H

#ifdef __cplusplus
extern "C" {
#endif

void LCD_init();

void LCD_clear();
void LCD_home();
void LCD_cursorOn();
void LCD_cursorOff();
void LCD_blinkOn();
void LCD_blinkOff();

void LCD_setCursor(unsigned char row, unsigned char col);

void LCD_print(const char *msg);
void LCD_printAt(const char *msg, unsigned char row, unsigned char col);
void LCD_printRight(const char *msg);
void LCD_printCenter(char *msg, unsigned char row);

#ifdef __cplusplus
}
#endif

#endif //LCD_H
