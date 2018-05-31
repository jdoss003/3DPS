/*
 * pin_io.cpp
 *
 * Created: 5/19/2018 4:05:01 PM
 *  Author: Justin
 */

#include "defs.h"

unsigned char xDDRA = 0;
unsigned char xDDRB = 0x03; // TODO
unsigned char xDDRC = 0;
unsigned char xDDRD = 0xFF; // TODO

unsigned char xPINA = 0;
unsigned char xPINB = 0;
unsigned char xPINC = 0;
unsigned char xPIND = 0;

unsigned char xPORTA = 0;
unsigned char xPORTB = 0;
unsigned char xPORTC = 0;
unsigned char xPORTD = 0;

void INITPIN(_io_pin pin, _io_mode mode, _io_hl _default)
{
    unsigned char mask;

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
            xDDRA = (mode ? xDDRA | mask : xDDRA & ~mask);
            xPORTA = (_default ? xPORTA | mask : xPORTA & ~mask);

            DDRA = xDDRA;
            PORTA = xPORTA;
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
            xDDRB = (mode ? xDDRB | mask : xDDRB & ~mask);
            xPORTB = (_default ? xPORTB | mask : xPORTB & ~mask);

            DDRB = xDDRB;
            PORTB = xPORTB;
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
            xDDRC = (mode ? xDDRC | mask : xDDRC & ~mask);
            xPORTC = (_default ? xPORTC | mask : xPORTC & ~mask);

            DDRC = xDDRC;
            PORTC = xPORTC;
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
            xDDRD = (mode ? xDDRD | mask : xDDRD & ~mask);
            xPORTD = (_default ? xPORTD | mask : xPORTD & ~mask);

            DDRD = xDDRD;
            PORTD = xPORTD;
            break;
        default:
            break;
    }
}

void INITADC(_io_pin pin)
{
	if (pin >= PA_0 && pin <= PA_7)
	{
		ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (pin - PA_0);
	}
}

void SETPIN(_io_pin pin, _io_hl out)
{
    unsigned char mask;

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
            xPORTA = (out ? xPORTA | mask : xPORTA & ~mask);

            PORTA = xPORTA;
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
            xPORTB = (out ? xPORTB | mask : xPORTB & ~mask);

            PORTB = xPORTB;
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
            xPORTC = (out ? xPORTC | mask : xPORTC & ~mask);

            PORTC = xPORTC;
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
            xPORTD = (out ? xPORTD | mask : xPORTD & ~mask);

            PORTD = xPORTD;
            break;
        default:
            break;
    }
}

unsigned char GETPIN(_io_pin pin, unsigned char invert)
{
    unsigned char mask;
    unsigned char ret = 0;

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
            return 0;
    }
}

unsigned short GETADC()
{
    unsigned short xADC = ADC;
    return xADC;
}

unsigned short GETMAXADC()
{
    return 1023;
}