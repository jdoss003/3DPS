/*
 * File: fileio.h
 * Author : Justin Doss
 *
 * This contains utility functions for FATFS operations
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef FILEIO_H
#define FILEIO_H

#ifdef __cplusplus
//extern "C" {
#endif

#include "defs.h"

unsigned char FIO_mountDrive();
unsigned char FIO_changeDir(char* p);

FILINFO* FIO_readDirNext();

unsigned char FIO_openFile(char* n);
unsigned char FIO_closeFile();
char* FIO_fileGetLine();

#ifdef __cplusplus
//}
#endif


#endif //FILEIO_H
