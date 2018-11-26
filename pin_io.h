/*
 * pin_io.h
 * Author : Justin Doss
 *
 * This file allows for easy control of all input and output signals from the microcontroller.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#ifndef PIN_IO_H_
#define PIN_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum io_mode { INPUT, OUTPUT } _io_mode;
typedef enum io_hl { LOW, HIGH } _io_hl;
typedef enum io_pin
{
    PA_0, PA_1, PA_2, PA_3, PA_4, PA_5, PA_6, PA_7,
    PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7,
    PC_0, PC_1, PC_2, PC_3, PC_4, PC_5, PC_6, PC_7,
    PD_0, PD_1, PD_2, PD_3, PD_4, PD_5, PD_6, PD_7, NONE
} _io_pin;

void INITPIN(_io_pin pin, _io_mode mode, char _default);
void INITADC();

void SETPIN(_io_pin pin, char out);

unsigned char GETPIN(_io_pin pin, char invert);

unsigned short GETADC(_io_pin pin);
unsigned short GETMAXADC();

#ifdef __cplusplus
}
#endif

#endif /* PIN_IO_H_ */