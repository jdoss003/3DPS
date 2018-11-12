//
// Created by Justin on 11/8/18.
//

#ifndef AVR_SPI_H
#define AVR_SPI_H

void SPI_MasterInit();
void SPI_MasterTransmit(unsigned char cData);
void SPI_ServantInit();
void SPI_Disable();

void SPI_SS_LOW();
void SPI_SS_HIGH();

void SPI_FCLK_FAST();
void SPI_FCLK_SLOW();

#endif //AVR_SPI_H
