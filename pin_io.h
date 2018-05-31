/*
 * pin_io.h
 *
 * Created: 5/19/2018 4:05:20 PM
 *  Author: Justin
 */


#ifndef PIN_IO_H_
#define PIN_IO_H_

typedef enum io_mode { INPUT, OUTPUT } _io_mode;
typedef enum io_hl { LOW, HIGH } _io_hl;
typedef enum io_pin
{
    PA_0, PA_1, PA_2, PA_3, PA_4, PA_5, PA_6, PA_7,
    PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7,
    PC_0, PC_1, PC_2, PC_3, PC_4, PC_5, PC_6, PC_7,
    PD_0, PD_1, PD_2, PD_3, PD_4, PD_5, PD_6, PD_7, NONE
} _io_pin;

void INITPIN(_io_pin pin, _io_mode mode, _io_hl _default);
void INITADC(_io_pin pin);

void SETPIN(_io_pin pin, _io_hl out);

unsigned char GETPIN(_io_pin pin, unsigned char invert);

unsigned short GETADC();
unsigned short GETMAXADC();

#endif /* PIN_IO_H_ */