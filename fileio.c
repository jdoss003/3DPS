/*
 * File: fileio.c
 * Author : Justin Doss
 *
 * This contains utility functions for FATFS operations
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "defs.h"

FATFS fatfs;
FRESULT fr;
DIR dir;
FILINFO finfo;
FIL file;

char line[100];

unsigned char FIO_mountDrive()
{
    char* vol = "0:";
    char i = 0;

	while (i < 4)
	{
		*vol = '0' + i;
		fr = f_mount(&fatfs, vol, 1);
		if (fr == FR_OK)
		{
			fr = f_chdir("/..");
			if (fr == FR_OK)
				break;
		}
		++i;
	}  

    return fr == FR_OK;
}

unsigned char FIO_changeDir(char* p)
{
    fr = f_chdir(p);
    return fr == FR_OK;
}

FILINFO* FIO_readDirNext()
{
    if (!dir.obj.fs)
    {
        fr = f_opendir(&dir, "");
        if (fr != FR_OK)
            return NULL;
    }

    fr = f_readdir(&dir, &finfo);
    if (fr != FR_OK || !finfo.fname[0])
    {
        f_closedir(&dir);
        return NULL;
    }

    return &finfo;
}

unsigned char FIO_openFile(char* n)
{
    if (!FIO_closeFile())
        return 0;

    fr = f_open(&file, n, FA_READ | FA_OPEN_EXISTING);
    return fr == FR_OK;
}

unsigned char FIO_closeFile()
{
    if (file.obj.fs)
    {
        fr = f_close(&file);
        if (fr != FR_OK)
            return 0;
    }
    return 1;
}

char* FIO_fileGetLine()
{
    if (file.obj.fs)
    {
        char* r = f_gets(line, 100, &file);
        if (!r)
            line[0] = 0;
		return line;
    }
	line[0] = 0;
    return line;
}