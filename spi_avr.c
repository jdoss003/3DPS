//
// Created by Justin on 11/8/18.
//

#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi_avr.h"

// Master code
void SPI_MasterInit()
{
    // Set DDRB to have MOSI, SCK, and SS as output and MISO as input
    DDRB = (1 << DDRB4) | (1 << DDRB5) | (1 << DDRB7) | (DDRB & ~(1 << DDRB6));
    // Set SPCR register to enable SPI, enable master, and use SCK frequency of fosc/16  (pg. 168)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
    // Make sure global interrupts are enabled on SREG register (pg. 9)
    sei();
}

void SPI_MasterTransmit(unsigned char cData)
{
    // data in SPDR will be transmitted, e.g. SPDR = cData;
    SPDR = cData;
    while (!(SPSR & (1 << SPIF))); // wait for transmission to complete
}

// Servant code
void SPI_ServantInit()
{
    // set DDRB to have MISO line as output and MOSI, SCK, and SS as input
    DDRB = (1 << DDRB6) | (DDRB & ~((1 << DDRB4) | (1 << DDRB5) | (1 << DDRB7)));
    // set SPCR register to enable SPI and enable SPI interrupt (pg. 168)
    SPCR = (1 << SPE) | (1 << SPIE);
    // make sure global interrupts are enabled on SREG register (pg. 9)
    sei();
}

void SPI_Disable()
{
    SPCR &= ~(1 << SPE);
}

void SPI_SSLOW()
{
    SPCR |= (1 << MSTR);
    PORTB &= ~(1 << PORTB4);
}

void SPI_SSHIGH()
{
    PORTB |= (1 << PORTB4);
}

void SPI_FCLK_FAST()
{
    SPCR = (1 << SPI2X) | (SPCR & ~((1 << SPR1) | (1 << SPR0)));
}

void SPI_FCLK_SLOW()
{
    SPCR = (1 << SPI2X) | (1 << SPR1) | (SPCR & ~(1 << SPR0));
}

/*
ISR(SPI_STC_vect)
{
    // this is enabled in with the SPCR registers SPI Interrupt Enable
    // SPDR contains the received data, e.g. unsigned char receivedData = SPDR;
    //receivedData = SPDR;
}
*/