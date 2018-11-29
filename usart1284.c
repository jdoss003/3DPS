

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

unsigned char USART_isSendReady(unsigned char usartNum)
{
    return !usartNum ? (UCSR0A & (1 << UDRE0))  && !bit_is_set(UCSR0B, TXCIE0) : (UCSR1A & (1 << UDRE1)) && !bit_is_set(UCSR1B, TXCIE1);
}

unsigned char USART_hasTransmitted(unsigned char usartNum)
{
    return !usartNum ? bit_is_set(UCSR0A, TXC0) : bit_is_set(UCSR1A, TXC1);
}

inline unsigned char USART_hasTransmittedLine(unsigned char usartNum)
{
	return !usartNum ? !bit_is_set(UCSR0B, UDRIE0) : !bit_is_set(UCSR1B, UDRIE1);
}

/// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! **** ///
unsigned char USART_hasReceived(unsigned char usartNum)
{
    return !usartNum? bit_is_set(UCSR0A, RXC0) : bit_is_set(UCSR1A, RXC1);
}

/// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! **** ///
void USART_send(unsigned char toSend, unsigned char usartNum)
{
    if (!usartNum) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = toSend;
    }
    else {
        loop_until_bit_is_set(UCSR1A, UDRE1);
        UDR1 = toSend;
    }
}

/// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! **** ///
unsigned char USART_receive(unsigned char usartNum)
{
    if (!usartNum) {
        loop_until_bit_is_set(UCSR0A, RXC0);     // Wait for data to be received
        return UDR0;                             // Get and return received data from buffer
    }
    else {
        loop_until_bit_is_set(UCSR1A, RXC1);
        return UDR1;
    }
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
}

inline unsigned char USART_hasLine(unsigned char usartNum)
{
// 	char i = !usartNum ? head0 : head1;
// 	char* c = !usartNum ? &readBuf0[0] : &readBuf1[0];
// 	char t = !usartNum ? index0 : index1;
// 	
// 	while (i != t)
// 	{
// 		if (*(c + i) == '\n')
// 			return 1;
// 		if (++i == MAX_BUF)
// 			i = 0;
// 	}
// 	
//     return 0;

	if (!usartNum)
	{
		return index0[curBufRead0] > 0 && readBuf0[curBufRead0][index0[curBufRead0] - 1] == '\n';
	}
	return 0;
	//return !usartNum ? index0 > 0 && readBuf0[index0 - 1] == '\n' : index1 > 0 && readBuf1[index1 - 1] == '\n';
}

char* USART_getLine(unsigned char usartNum)
{
// 	char i = !usartNum ? head0 : head1;
// 	char* c = !usartNum ? &readBuf0[0] : &readBuf1[0];
// 	char t = !usartNum ? index0 : index1;
// 	char s = 0;
// 	
// 	while (i != t)
// 	{
// 		++s;
// 		if (*(c + i) == '\n')
// 			break;
// 		if (++i == MAX_BUF)
// 			i = 0;
// 	}
// 	
// 	if (i == t)
// 		return NULL;
// 	
// 	i = !usartNum ? head0 : head1;
// 	char* l = malloc(sizeof(char) * (s + 1));
// 	memset(l, 0, sizeof(char) * (s + 1));
// 	
// 	while (s-- > 0)
// 	{
// 		*(l++) = *(c + i);
// 		if (++i == MAX_BUF)
// 			i = 0;
// 	}
// 	
// 	if (!usartNum)
// 	{
// 		head0 = i;
// 	}
// 	else
// 	{
// 		head1 = i;
// 	}
// 	
// 	return l;
	char* temp = &readBuf0[curBufRead0][0];
	index0[curBufRead0] = 0;
	curBufRead0 = (curBufRead0 + 1) % MAX_BUFS;
	return temp;
	//return !curBuf ? &readBuf0[0] : &readBuf1[0];
}

void USART_clearBuf(unsigned char usartNum)
{
    if (!usartNum)
    {
//         memset(&readBuf0[0], 0, MAX_BUF);
//         index0 = 0;
    }
    else
    {
//         memset(&readBuf1[0], 0, MAX_BUF);
//         index1 = 0;
    }
}

inline void USART_sendLine(char* l, unsigned char usartNum)
{
	//char c;
    if (!usartNum)
    {
//		while ((c = *(l++)))
//			USART_send(c, usartNum);
//		USART_send(c, usartNum);
        out0 = l;
		//outHead0 = l;
        UCSR0B |= (1  << UDRIE0);
    }
    else
    {
        //out1 = l;
		//outHead1 = l;
        //UCSR1B |= (1  << UDRIE1);
    }
// 	USART_autoRecieve(0, usartNum);
// 	USART_send(*l, usartNum);
}

ISR(USART0_UDRE_vect)
{
    UDR0 = *out0;
    ++out0;
    if (*out0 == 0)
	{
		UCSR0B = UCSR0B & ~(1 << UDRIE0);
		//free(outHead0);
	}
}

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
// 	if (index0 > 0 && readBuf0[index0 - 1] == '\n')
//         trash = UDR0;
// 	if (index0 >= MAX_BUF)
//     {
// 	    USART_clearBuf(0);
//         readBuf0[index0++] = UDR0;
//     }
//     else
//         readBuf0[index0++] = UDR0;
		
// 	if (index0 == MAX_BUF)
// 	{
// 		USART_clearBuf(0);
// 		index0 = 0;
// 	}
}

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
