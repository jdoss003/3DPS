/*
 * File: lcd_menu.cpp
 * Author : Justin Doss
 *
 * LCD menu
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "lcd_menu.h"
#include "defs.h"

#define WITHIN(val, low, high) (val >= low && val <= high)

static LCDMainScreen mainScreen;
static LCD_MENU* currentScreen;

static FATFS fatfs;
static FRESULT fr;
static DIR dir;

char mountCard(char* vol)
{
	return f_mount(&fatfs, vol, 0) == FR_OK;
}

void listFiles(char* path)
{
	fr = f_chdir(path);
	if (fr == FR_OK)
	{
		fr = f_opendir(&dir, "");
		if (fr == FR_OK)
		{
			FILINFO finfo;
			LCD_MENU* fmenu = new LCD_FILE_MENU();
			while (1) {
				fr = f_readdir(&dir, &finfo);
				if ((fr != FR_OK) || !finfo.fname[0])
				{
					break;
				}
				if (finfo.fname[0] != '.' && finfo.fname[0] != '_')
				{
					fmenu->addMenuItem(new LCD_MENU_ITEM_FILE(strdup(path), strdup(&finfo.fname[0]), finfo.fattrib & AM_DIR));
				}
			}
			fmenu->setParent(currentScreen);
			currentScreen = fmenu;
			f_closedir(&dir);
		}
		else
		{
			systemFailure("Opening dir");
		}
	}
	else
	{
		systemFailure(path);
	}
}

void listRootDir()
{
	if (!mountCard("1:"))
	{
		systemFailure("SD card read 2");
	}
	listFiles("1:/");
}

/*
 *
 */
LCD_MENU_ITEM::~LCD_MENU_ITEM()
{
    delete this->name;
};

char* LCD_MENU_ITEM::getName()
{
	char* ret = "";
	return this->name ? this->name : ret;
};

void LCD_MENU_ITEM::onSelect() {};

/*
 *
 */
LCD_MENU_ITEM_MENU::LCD_MENU_ITEM_MENU(char* name, LCD_MENU* menu)
{
	this->name = name;
	this->menu = menu;
}

LCD_MENU_ITEM_MENU::~LCD_MENU_ITEM_MENU()
{
    delete this->menu;
}

void LCD_MENU_ITEM_MENU::onSelect()
{
	currentScreen->reset();
	this->menu->setParent(currentScreen);
	currentScreen = this->menu;
}

/*
 *
 */
LCD_MENU_ITEM_ACTION::LCD_MENU_ITEM_ACTION(char* name, void (*action)())
{
	this->name = name;
	this->action = action;
}

void LCD_MENU_ITEM_ACTION::onSelect()
{
	this->action();
}

/*
 *
 */
LCD_MENU_ITEM_FILE::LCD_MENU_ITEM_FILE(char* path, char* name, char isDir)
{
	this->path = path;
	this->name = name;
	this->isDir = isDir;
}

LCD_MENU_ITEM_FILE::~LCD_MENU_ITEM_FILE()
{
    delete this->path;
}

void LCD_MENU_ITEM_FILE::onSelect()
{
	if (this->isDir)
	{
		listFiles(this->name);
	}
	else
	{
		LCDReadFileMenu* m = new LCDReadFileMenu();
		FRESULT fr = f_open(m->getFile(), this->name, FA_READ | FA_OPEN_EXISTING);
		if (fr != FR_OK)
		{
			systemFailure("File read");
		}
		m->setParent(currentScreen);
		currentScreen->reset();
		currentScreen = m;
	}
}

/*
 *
 */
LCD_MENU* LCD_MENU::getCurrent()
{
    return currentScreen;
}

LCD_MENU::LCD_MENU()
{
	this->parent = NULL;
	this->index = 0;
	this->count = 0;
	this->curBtn = 10;
	this->nameIndex = 0;
	this->nameCount = 0;
	for (unsigned char i = 0; i < MAX_MENU_ITEMS; ++i)
	{
		this->items[i] = NULL;
	}
}

LCD_MENU::~LCD_MENU()
{
	for (unsigned char i = 0; i < MAX_MENU_ITEMS; ++i)
	{
        delete this->items[i];
	}
};

LCD_MENU* LCD_MENU::getParent()
{
	return this->parent;
}

void LCD_MENU::setParent(LCD_MENU* parent)
{
	this->parent = parent;
}

void LCD_MENU::update()
{
	LCD::get()->blinkOn();
    LCD::get()->clear();

    unsigned char line = 0;
	unsigned char i = this->index >= 3 ? this->index - 3 : 0;
	unsigned char max = i + 4;
    for (; i < this->count && i < max; ++i)
    {
        if (i == this->index)
        {
            unsigned char l = strlen(this->items[i]->getName());
            if (l > 19)
            {
                if (this->nameCount > 2)
                {
                    if (l - 19 == this->nameIndex)
                    {
                        this->nameIndex = 0;
                        this->nameCount = 0;
                    }
                    else
                    {
                        ++this->nameIndex;
                    }
                }
                else
                {
                    ++this->nameCount;
                }
            }
            LCD::get()->printAt(this->items[i]->getName() + this->nameIndex, line++, 1);
        }
        else
        {
            LCD::get()->printAt(this->items[i]->getName(), line++, 1);
        }
    }
	
	LCD::get()->blinkOn();
	LCD::get()->setCursor(this->index >= 3 ? 3 : this->index, 0);
}

void LCD_MENU::updateButtons()
{
	unsigned short xADC = GETADC(LCD_BTTNS);
	
	if (WITHIN(xADC, LCD_DOWN_LOW, LCD_DOWN_HIGH))
	{
		if (this->curBtn == 0)
		{
			this->next();
			this->curBtn = 1;
		}
	}
	else if (WITHIN(xADC, LCD_UP_LOW, LCD_UP_HIGH))
	{
		if (this->curBtn == 0)
		{
			this->prev();
			this->curBtn = 2;
		}
	}
	else if (WITHIN(xADC, LCD_BACK_LOW, LCD_BACK_HIGH))
	{
		if (this->curBtn == 0)
		{
			this->back();
			this->curBtn = 3;
		}
	}
	else if (WITHIN(xADC, LCD_ENTER_LOW, LCD_ENTER_HIGH))
	{
		if (this->curBtn == 0)
		{
			this->select();
			this->curBtn = 4;
		}
	}
	else if (WITHIN(xADC, LCD_MENU_LOW, LCD_MENU_HIGH))
	{
		if (this->curBtn == 0)
		{
			this->menu();
			this->curBtn = 5;
		}
	}
	else
	{
		this->curBtn = 0;
	}
}

void LCD_MENU::next()
{
    if (this->index < this->count - 1)
    {
        this->index++;
		this->nameCount = 0;
		this->nameIndex = 0;
    }
}

void LCD_MENU::prev()
{
    if (this->index > 0)
    {
        this->index--;
		this->nameCount = 0;
		this->nameIndex = 0;
    }
}

void LCD_MENU::select()
{
	if (this->items[this->index])
	{
		this->items[this->index]->onSelect();
	}
}

void LCD_MENU::back()
{
	if (this->parent)
	{
		currentScreen->reset();
		currentScreen = this->getParent();
	}
}

void LCD_MENU::menu()
{
	mainScreen.makeCurrent();
}

void LCD_MENU::addMenuItem(LCD_MENU_ITEM *item)
{
    if (this->count < MAX_MENU_ITEMS)
    {
        this->items[this->count++] = item;
    }
}


void LCD_MENU::reset()
{
	this->index = 0;
	this->curBtn = 10;
	this->nameIndex = 0;
	this->nameCount = 0;
}

/*
 *
 */
void LCD_FILE_MENU::back()
{
	f_chdir("..");
	LCD_MENU::back();
	delete this;
}

/*
 *
 */
LCDReadFileMenu::LCDReadFileMenu()
{
	this->line = (TCHAR*)malloc(sizeof(TCHAR) * 100);
}

LCDReadFileMenu::~LCDReadFileMenu()
{
    delete this->line;
}

void LCDReadFileMenu::update()
{
	if (!f_eof(&this->file))
	{
		memset(this->line, NULL, 100);
		f_gets(this->line, 100, &this->file);
		LCD::get()->blinkOff();
		LCD::get()->clear();
		LCD::get()->print(this->line);
	}
	else
	{
		this->back();
	}
}

void LCDReadFileMenu::back()
{
	f_close(&this->file);
	LCD_MENU::back();
}

void LCDReadFileMenu::menu()
{
	f_close(&this->file);
	LCD_MENU::menu();
}

FIL* LCDReadFileMenu::getFile()
{
	return &this->file;
}

/*
 *
 */
LCDMainMenu::LCDMainMenu()
{
	LCD_MENU* actionMenu = new LCD_MENU();

	actionMenu->addMenuItem(new LCD_MENU_ITEM_ACTION("Home All", &MovController::goHomeAll));
	actionMenu->addMenuItem(new LCD_MENU_ITEM_ACTION("Home X", &MovController::goHomeX));
	actionMenu->addMenuItem(new LCD_MENU_ITEM_ACTION("Home Y", &MovController::goHomeY));
	actionMenu->addMenuItem(new LCD_MENU_ITEM_ACTION("Home Z", &MovController::goHomeZ));
	actionMenu->setParent(this);

	this->addMenuItem(new LCD_MENU_ITEM_MENU("Actions", actionMenu));
	this->addMenuItem(new LCD_MENU_ITEM_ACTION("Print from file", &listRootDir));
}

/*
 *
 */
void LCDMainScreen::update()
{
	LCD::get()->blinkOff();
    LCD::get()->clear();
    
	char temp1[6];
	dtostrf(Extruder::getTemp(), 5, 1, &temp1[0]);
	LCD::get()->print(&temp1[0]);
	LCD::get()->print("/");
	char temp1d[4];
	utoa(Extruder::getDesiredTemp(), &temp1d[0], 10);
	LCD::get()->print(&temp1d[0]);
	LCD::get()->print("C\xDF");
	LCD::get()->printRight("00.0/00C\xDF");
	
	char num[6];
	LCD::get()->printCenter(itoa(GETADC(LCD_BTTNS), &num[0], 10), 2);
    LCD::get()->printCenter("Printer Ready", 3);
}

void LCDMainScreen::menu()
{
	currentScreen->reset();
	currentScreen = new LCDMainMenu();
}

void LCDMainScreen::makeCurrent()
{
    if (currentScreen != NULL && currentScreen != &mainScreen)
    {
        LCD_MENU* top = currentScreen;
        while (top->getParent() != NULL)
		{
        	top = top->getParent();
		}
		delete top;
    }
	currentScreen = &mainScreen;
}