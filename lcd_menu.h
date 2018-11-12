/*
 * File: lcd_menu.h
 * Author : Justin Doss
 *
 * LCD menu
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef LCD_MENU_H
#define LCD_MENU_H

#include "defs.h"

#define MAX_MENU_ITEMS 25

class LCD_MENU;

class LCD_MENU_ITEM
{
    public:
        virtual ~LCD_MENU_ITEM();
        virtual char* getName();
        virtual void onSelect();

    protected:
        char* name;
};

class LCD_MENU_ITEM_MENU : public LCD_MENU_ITEM
{    
        public:
            LCD_MENU_ITEM_MENU(char* name, LCD_MENU* menu);
            ~LCD_MENU_ITEM_MENU();
            void onSelect();
        
        private:
            LCD_MENU* menu;
};

class LCD_MENU_ITEM_ACTION : public LCD_MENU_ITEM
{
    public:
        LCD_MENU_ITEM_ACTION(char* name, void (*action)());
        void onSelect();
        
    private:
        void (*action)();
};

class LCD_MENU_ITEM_FILE : public LCD_MENU_ITEM
{
    public:
        LCD_MENU_ITEM_FILE(char* path, char* name, char isDir);
        ~LCD_MENU_ITEM_FILE();
        void onSelect();

    private:
        char* path;
        char isDir;
};

class LCD_MENU
{
    public:
        LCD_MENU();
        virtual ~LCD_MENU();
        virtual LCD_MENU* getParent();
        void setParent(LCD_MENU*);
        virtual void update();
        virtual void updateButtons();

        virtual void next();
        virtual void prev();
        virtual void select();
        virtual void back();
        virtual void menu();
        
        virtual void reset();

        void addMenuItem(LCD_MENU_ITEM*);
        
        static LCD_MENU* getCurrent();

    private:
        LCD_MENU* parent;
        LCD_MENU_ITEM* items[MAX_MENU_ITEMS];
        unsigned char index;
        unsigned char count;
        unsigned char curBtn;
        unsigned char nameIndex;
        unsigned char nameCount;
};

class LCD_FILE_MENU : public LCD_MENU
{
    public:
        virtual void back();
};

class LCDReadFileMenu : public LCD_MENU
{
    public:
        LCDReadFileMenu();
        ~LCDReadFileMenu();
        void update();
        void back();
        void menu();

        FIL* getFile();

    private:
        TCHAR* line;
        FIL file;
};

class LCDMainMenu : public LCD_MENU
{
    public:
        LCDMainMenu();
};

class LCDMainScreen : public LCD_MENU
{
    public:
        void update();
        void menu();
        
        static void makeCurrent();
};

#endif //LCD_MENU_H
