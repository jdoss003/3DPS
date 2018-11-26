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
#include "mov_controller.h"
#include "string.h"

#define WITHIN(val, low, high) (val >= low && val <= high)

void listRootDir();

LCD_MENU* currentScreen;
LCDMainScreen mainScreen;
LCDMainMenu mainMenu;
LCD_MENU actionMenu;
//LCD_FILE_MENU fileMenu;
LCD_MOVE_MOTOR_MENU xMotorMenu(X_AXIS, "Move X");
LCD_MOVE_MOTOR_MENU yMotorMenu(Y_AXIS, "Move Y");
LCD_MOVE_MOTOR_MENU zMotorMenu(Z_AXIS, "Move Z");

LCD_MENU_ITEM_MENU itemActionMenu("Actions", &actionMenu);
LCD_MENU_ITEM_ACTION itemListRootDir("Print file", &listRootDir);

LCD_MENU_ITEM_MENU itemXMotorMenu("Move X", &xMotorMenu);
LCD_MENU_ITEM_MENU itemYMotorMenu("Move Y", &yMotorMenu);
LCD_MENU_ITEM_MENU itemZMotorMenu("Move Z", &zMotorMenu);

LCD_MENU_ITEM_ACTION itemHomeAll("Home All", &MovController::goHomeAll);
LCD_MENU_ITEM_ACTION itemDisableSteppers("Disable Steppers", &MovController::disableSteppers);
LCD_MENU_ITEM_ACTION itemEnableSteppers("Enable Steppers", &MovController::enableSteppers);
LCD_MENU_ITEM_ACTION itemPreHeat("Preheat", &Extruder_preHeat);

volatile unsigned char driveMounted = 0;

unsigned char strEndsWith(char* toTest, char* toMatch)
{
    unsigned char len1 = strlen(toTest);
    unsigned char len2 = strlen(toMatch);

    if (len1 < len2)
        return 0;

    toTest += len1 - len2 - 1;
    while (--len2 > 0)
	{
		if (*toTest != *toMatch)
		{
			return 0;
		}
		++toTest;
		++toMatch;
	}
    return 1;
}

void listFiles(char* path)
{
    if (FIO_changeDir(path))
    {
        FILINFO* finfo;
        LCD_MENU* fmenu = new LCD_FILE_MENU();

        fmenu->addMenuItem(new LCD_MENU_ITEM_FILE("..", 1));

        unsigned char count = 0;
        while ((finfo = FIO_readDirNext()))
        {
            if (!(finfo->fattrib & AM_HID) && count++ < MAX_MENU_ITEMS)
            {
                char isDir = finfo->fattrib & AM_DIR;
                //if (isDir)// || strEndsWith(&finfo->fname[0], ".gcode"))
                    fmenu->addMenuItem(new LCD_MENU_ITEM_FILE(strdup(&finfo->fname[0]), isDir));
            }
        }

        fmenu->setParent(currentScreen);
        currentScreen = fmenu;
    }
}

void listRootDir()
{
    if (driveMounted || FIO_mountDrive())
    {
        driveMounted = 1;
        listFiles("/..");
    }
}

void LCDMenu_init()
{
    actionMenu.addMenuItem(&itemHomeAll);
    actionMenu.addMenuItem(&itemEnableSteppers);
    actionMenu.addMenuItem(&itemDisableSteppers);
    actionMenu.addMenuItem(&itemPreHeat);

    xMotorMenu.setParent(&actionMenu);
    yMotorMenu.setParent(&actionMenu);
    zMotorMenu.setParent(&actionMenu);
    actionMenu.addMenuItem(&itemXMotorMenu);
    actionMenu.addMenuItem(&itemYMotorMenu);
    actionMenu.addMenuItem(&itemZMotorMenu);
    actionMenu.setParent(&mainMenu);

    mainMenu.addMenuItem(&itemActionMenu);
    mainMenu.addMenuItem(&itemListRootDir);

    //fileMenu.setParent(&mainMenu);
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
	_delay_us(20);
}

/*
 *
 */
LCD_MENU_ITEM_FILE::LCD_MENU_ITEM_FILE(char* name, char isDir)
{
    this->name = name;
    this->isDir = isDir;
}

void LCD_MENU_ITEM_FILE::onSelect()
{
    if (this->isDir)
    {
        listFiles(this->name);
    }
    else
    {
		if (FIO_openFile(this->name))
		{
			setSystemPrinting(0);
		}
        //todo error msg
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
    this->parent = 0;
    this->index = 0;
    this->count = 0;
    this->btnEnabled = 1;
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
    LCD_blinkOn();
    LCD_clear();

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
            LCD_printAt(this->items[i]->getName() + this->nameIndex, line++, 1);
        }
        else
        {
            LCD_printAt(this->items[i]->getName(), line++, 1);
        }
    }

    LCD_blinkOn();
    LCD_setCursor(this->index >= 3 ? 3 : this->index, 0);
}

void LCD_MENU::updateButtons()
{
    if (!this->btnEnabled)
        return;

	//cli();
    unsigned short xADC = GETADC(LCD_BTTNS);
// 	xADC += GETADC(LCD_BTTNS);
// 	xADC += GETADC(LCD_BTTNS);
// 	xADC /= 3;
	//sei();
	
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
    else if (xADC > 850)
    {
        this->curBtn = 0;
    }
}

void LCD_MENU::enableButtons()
{
    this->btnEnabled = 1;
}

unsigned char LCD_MENU::disableButtons()
{
    unsigned char c = this->btnEnabled;
	this->btnEnabled = 0;
	return c;
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
    this->btnEnabled = 1;
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
LCD_MOVE_MOTOR_MENU::LCD_MOVE_MOTOR_MENU(_axis ax, char* name)
{
    this->ax = ax;
    this->name = name;
}

LCD_MOVE_MOTOR_MENU::~LCD_MOVE_MOTOR_MENU()
{
    delete this->name;
}

void LCD_MOVE_MOTOR_MENU::update()
{
    LCD_blinkOff();
    LCD_clear();

    char temp1[] = "   .   mm";
    dtostrf(MovController::getMovController(this->ax)->getPosition(), 6, 2, temp1);
	*(temp1 + 6) = ' ';
    LCD_printRight(temp1);
    LCD_printCenter(this->name, 1);
}

void LCD_MOVE_MOTOR_MENU::updateButtons()
{
    if (!MovController::areAnyMotorsMoving())
    {
        LCD_MENU::updateButtons();
    }
}

void LCD_MOVE_MOTOR_MENU::next() {}

void LCD_MOVE_MOTOR_MENU::prev() {}

void LCD_MOVE_MOTOR_MENU::select()
{
    MovController* mc = MovController::getMovController(ax);
    if (mc->getPosition() < (float)MovController::getMaxPos(this->ax) - 0.1)
        mc->moveTo(MovController::getStepsPerMM(this->ax) * 0.1, 5);
}

void LCD_MOVE_MOTOR_MENU::back()
{
    MovController* mc = MovController::getMovController(ax);
    if (mc->getPosition() > 0.1)
        mc->moveTo(MovController::getStepsPerMM(this->ax) * -0.1, 5);
}

void LCD_MOVE_MOTOR_MENU::menu()
{
    LCD_MENU::back();
}

/*
 *
 */
//LCDReadFileMenu::LCDReadFileMenu()
//{
//    this->line = (TCHAR*)malloc(sizeof(TCHAR) * 100);
//}
//
//LCDReadFileMenu::~LCDReadFileMenu()
//{
//    delete this->line;
//}
//
//void LCDReadFileMenu::update()
//{
//    if (!f_eof(&this->file))
//    {
//        memset(this->line, NULL, 100);
//        f_gets(this->line, 100, &this->file);
//        LCD_blinkOff();
//        LCD_clear();
//        LCD_print(this->line);
//    }
//    else
//    {
//        this->back();
//    }
//}
//
//void LCDReadFileMenu::back()
//{
//    f_close(&this->file);
//    LCD_MENU::back();
//}
//
//void LCDReadFileMenu::menu()
//{
//    f_close(&this->file);
//    LCD_MENU::menu();
//}
//
//FIL* LCDReadFileMenu::getFile()
//{
//    return &this->file;
//}

/*
 *
 */
LCDMainMenu::LCDMainMenu() {}

/*
 *
 */
LCDMainScreen::LCDMainScreen()
{
    this->setMessage("Printer Ready");
}

LCDMainScreen::~LCDMainScreen() {}

void LCDMainScreen::update()
{
    LCD_blinkOff();
    LCD_clear();
    char temp1[6];
    dtostrf(Extruder_getTemp(), 5, 1, &temp1[0]);
    LCD_print(&temp1[0]);
    LCD_print("/");
    char temp1d[4];
    utoa(Extruder_getDesiredTemp(), &temp1d[0], 10);
    LCD_print(&temp1d[0]);
    LCD_print("C\xDF");
    LCD_printRight("00.0/00C\xDF");

	unsigned short xADC = GETADC(LCD_BTTNS);
// 	xADC += GETADC(LCD_BTTNS);
// 	xADC += GETADC(LCD_BTTNS);
// 	xADC /= 3;
	
	char temp2[6];
	utoa(xADC, &temp2[0], 10);
	LCD_printCenter(&temp2[0], 2);

	if (this->msg)
	{
		LCD_printCenter(this->msg, 3);
 		//this->msg[0] = 0;
	}
}

void LCDMainScreen::menu()
{
    currentScreen->reset();
    currentScreen = &mainMenu;
}

void LCDMainScreen::makeCurrent()
{
    currentScreen = &mainScreen;
}

void LCDMainScreen::setMessage(char* m)
{
	unsigned char i = 0;
    while (i < 20 && *m)
		mainScreen.msg[i++] = *(m++);
	mainScreen.msg[i] = 0;
}