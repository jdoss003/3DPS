

#include "defs.h"
#include "string.h"

#ifndef F_CPU
#define F_CPU 8000000UL // Assume uC operates at 8MHz
#endif

#ifndef BAUD_RATE
#define BAUD_RATE 9600
#endif

#define BAUD_PRESCALE(br) (((F_CPU / (br * 16UL))) - 1)
#define MAX_BUF 80
#define MAX_BUFS 5

unsigned char readBuf0[MAX_BUFS][MAX_BUF];
unsigned char index0[MAX_BUFS];
unsigned char curBufRead0 = 0;
unsigned char curBufWrite0 = 0;


char* out0;
char* out1;

void USART_initBaud(unsigned char usartNum, unsigned short baudRate)
{
    if (usartNum != 1) {
        UCSR0B |= (1 << RXEN0)  | (1 << TXEN0);            // Turn on receiver and transmitter
        UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);            // Use 8-bit character sizes
        UBRR0L = BAUD_PRESCALE(baudRate);
        UBRR0H = (BAUD_PRESCALE(baudRate) >> 8);
    }
    else {
        UCSR1B |= (1 << RXEN1)  | (1 << TXEN1);            // Turn on receiver and transmitter
        UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);            // Use 8-bit character sizes
        UBRR1L = BAUD_PRESCALE(baudRate);
        UBRR1H = (BAUD_PRESCALE(baudRate) >> 8);
    }
}

void USART_init(unsigned char usartNum)
{
	USART_initBaud(usartNum, BAUD_RATE);
}

inline unsigned char USART_hasTransmittedLine(unsigned char usartNum)
{
	return !usartNum ? !bit_is_set(UCSR0B, UDRIE0) : !bit_is_set(UCSR1B, UDRIE1);
}

void USART_autoRecieve(unsigned char b, unsigned char usartNum)
{
    if (!usartNum)
    {
		if (b)
			for (unsigned char i = 0; i < MAX_BUFS; ++i)
			{
				index0[i] = 0;
				memset(&readBuf0[i][0], 0, MAX_BUF);
			}
		UCSR0B = b ? UCSR0B | (1 << RXCIE0) : UCSR0B & ~(1 << RXCIE0);
	}
    else
        UCSR1B = b ? UCSR1B | (1 << RXCIE1) : UCSR1B & ~(1 << RXCIE1);
        // TODO USART 1
}

inline unsigned char USART_hasLine(unsigned char usartNum)
{
	if (!usartNum)
	{
		return index0[curBufRead0] > 0 && readBuf0[curBufRead0][index0[curBufRead0] - 1] == '\n';
	}
	return 0;
}

char* USART_getLine(unsigned char usartNum)
{
    // TODO USART 1
    char* temp = &readBuf0[curBufRead0][0];
	index0[curBufRead0] = 0;
	curBufRead0 = (curBufRead0 + 1) % MAX_BUFS;
	return temp;
}

void USART_clearBuf(unsigned char usartNum)
{
    if (!usartNum)
    {
        // TODO USART 0
    }
    else
    {
        // TODO USART 1
    }
}

inline void USART_sendLine(char* l, unsigned char usartNum)
{
    if (!usartNum)
    {
        out0 = l;
        UCSR0B |= (1  << UDRIE0);
    }
    else
    {
        // TODO USART 1
        //out1 = l;
        //UCSR1B |= (1  << UDRIE1);
    }
}

ISR(USART0_UDRE_vect)
{
    UDR0 = *out0;
    ++out0;
    if (*out0 == 0)
        UCSR0B = UCSR0B & ~(1 << UDRIE0);
}

// TODO USART 1
// ISR(USART1_UDRE_vect)
// {
//     UDR1 = *out1;
//     ++out1;
//     if (!(*out1))
// 	{
// 		UCSR1B = UCSR1B & ~(1 << UDRIE1);
// 		//free(outHead1);
// 	}
// }

ISR(USART0_RX_vect)
{
    char c = UDR0;
	if (c == '\n' && index0[curBufWrite0] != 0)
	{
		readBuf0[curBufWrite0][index0[curBufWrite0]++] = c;
		readBuf0[curBufWrite0][index0[curBufWrite0]] = 0;
		curBufWrite0 = (curBufWrite0 + 1) % MAX_BUFS;
		memset(&readBuf0[curBufWrite0][0], 0, MAX_BUF);
		index0[curBufWrite0] = 0;
	}
	else
	{
		readBuf0[curBufWrite0][index0[curBufWrite0]++] = c;
		if (index0[curBufWrite0] == MAX_BUF)
		{
			memset(&readBuf0[curBufWrite0][0], 0, MAX_BUF);
			index0[curBufWrite0] = 0;
		}
	}
}

// TODO USART 1
// ISR(USART1_RX_vect)
// {
//     if (index1 > 0 && readBuf1[index1 - 1] == '\n')
//         trash = UDR1;
//     else if (index1 > MAX_BUF)
//     {
//         USART_clearBuf(1);
//         readBuf1[index1++] = UDR1;
//     }
//     else
//         readBuf1[index1++] = UDR1;
// }
