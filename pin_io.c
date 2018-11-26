/*
 * pin_io.c
 * Author : Justin Doss
 *
 * This file allows for easy control of all input and output signals from the microcontroller.
 *
 * I acknowledge all content contained herein, excluding template or example code,
 * is my own work.
 */

#include "defs.h"

void INITPIN(_io_pin pin, _io_mode mode, char _default)
{
    char mask;

    switch (pin)
    {
        case PA_0:
        case PA_1:
        case PA_2:
        case PA_3:
        case PA_4:
        case PA_5:
        case PA_6:
        case PA_7:
            mask = (1 << (pin - PA_0));
            DDRA = (mode ? DDRA | mask : DDRA & ~mask);
            PORTA = (_default ? PORTA | mask : PORTA & ~mask);
            break;
        case PB_0:
        case PB_1:
        case PB_2:
        case PB_3:
        case PB_4:
        case PB_5:
        case PB_6:
        case PB_7:
            mask = (1 << (pin - PB_0));
            DDRB = (mode ? DDRB | mask : DDRB & ~mask);
            PORTB = (_default ? PORTB | mask : PORTB & ~mask);
            break;
        case PC_0:
        case PC_1:
        case PC_2:
        case PC_3:
        case PC_4:
        case PC_5:
        case PC_6:
        case PC_7:
            mask = (1 << (pin - PC_0));
            DDRC = (mode ? DDRC | mask : DDRC & ~mask);
            PORTC = (_default ? PORTC | mask : PORTC & ~mask);
            break;
        case PD_0:
        case PD_1:
        case PD_2:
        case PD_3:
        case PD_4:
        case PD_5:
        case PD_6:
        case PD_7:
            mask = (1 << (pin - PD_0));
            DDRD = (mode ? DDRD | mask : DDRD & ~mask);
            PORTD = (_default ? PORTD | mask : PORTD & ~mask);
            break;
        default:
            systemFailure("Init pin");
            break;
    }
}

void INITADC()
{
    ADCSRA = (1 << ADEN);
}

void SETPIN(_io_pin pin, char out)
{
    char mask;

    switch (pin)
    {
        case PA_0:
        case PA_1:
        case PA_2:
        case PA_3:
        case PA_4:
        case PA_5:
        case PA_6:
        case PA_7:
            mask = (1 << (pin - PA_0));
            PORTA = (out ? PORTA | mask : PORTA & ~mask);
            break;
        case PB_0:
        case PB_1:
        case PB_2:
        case PB_3:
        case PB_4:
        case PB_5:
        case PB_6:
        case PB_7:
            mask = (1 << (pin - PB_0));
            PORTB = (out ? PORTB | mask : PORTB & ~mask);
            break;
        case PC_0:
        case PC_1:
        case PC_2:
        case PC_3:
        case PC_4:
        case PC_5:
        case PC_6:
        case PC_7:
            mask = (1 << (pin - PC_0));
            PORTC = (out ? PORTC | mask : PORTC & ~mask);
            break;
        case PD_0:
        case PD_1:
        case PD_2:
        case PD_3:
        case PD_4:
        case PD_5:
        case PD_6:
        case PD_7:
            mask = (1 << (pin - PD_0));
            PORTD = (out ? PORTD | mask : PORTD & ~mask);
            break;
        default:
            systemFailure("Set pin");
            break;
    }
}

unsigned char GETPIN(_io_pin pin, char invert)
{
    char mask;
    char ret = 0;

    switch (pin)
    {
        case PA_0:
        case PA_1:
        case PA_2:
        case PA_3:
        case PA_4:
        case PA_5:
        case PA_6:
        case PA_7:
            mask = (1 << (pin - PA_0));
            ret = mask & (invert ? ~PINA : PINA);
            return ret >> (pin - PA_0);
        case PB_0:
        case PB_1:
        case PB_2:
        case PB_3:
        case PB_4:
        case PB_5:
        case PB_6:
        case PB_7:
            mask = (1 << (pin - PB_0));
            ret = mask & (invert ? ~PINB : PINB);
            return ret >> (pin - PB_0);
        case PC_0:
        case PC_1:
        case PC_2:
        case PC_3:
        case PC_4:
        case PC_5:
        case PC_6:
        case PC_7:
            mask = (1 << (pin - PC_0));
            ret = mask & (invert ? ~PINC : PINC);
            return ret >> (pin - PC_0);
        case PD_0:
        case PD_1:
        case PD_2:
        case PD_3:
        case PD_4:
        case PD_5:
        case PD_6:
        case PD_7:
            mask = (1 << (pin - PD_0));
            ret = mask & (invert ? ~PIND : PIND);
            return ret >> (pin - PD_0);
        default:
            systemFailure("Get pin");
            return 0;
    }
}

unsigned short GETADC(_io_pin pin)
{
    if (pin >= PA_0 && pin <= PA_7)
    {
        ADMUX = (pin - PA_0);
        ADCSRA |= (1 << ADSC);
    }
    
    while(ADCSRA & (1 << ADSC));
    return ADC;
}

unsigned short GETMAXADC()
{
    return 1023;
}