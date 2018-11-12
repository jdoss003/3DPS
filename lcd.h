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

class LCD
{
    public:
        LCD(unsigned char rows, unsigned char cols);
        void init();
		
		void clear();
        void home();
		void cursorOn();
		void cursorOff();
		void blinkOn();
		void blinkOff();
		
		void setCursor(unsigned char row, unsigned char col);
		
		void print(const char* msg);
		void printRight(const char* msg);
		void printAt(const char* msg, unsigned char row, unsigned char col);
		void printCenter(char*, unsigned char row);

        static LCD* get();

    private:

		void sendCommand(char);
        void sendData(char);
		void write4bits(char);
		void pulseEnable();

        unsigned char rows;
        unsigned char cols;

        unsigned char curRow;
		unsigned char curCol;

        char disp_func;
        char disp_mode;
        char disp_entry_mode;
};

extern unsigned char getLength(char *str);

#endif //LCD_H
