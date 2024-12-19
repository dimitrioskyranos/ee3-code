/* SPI library for 16F886*/
#ifndef SPILibrary16F886_h
#define SPILibrary16F886_h

#include <stdio.h>
#include <stdlib.h>
#include "SPILibrary16F886.h"

//turn on SPI module (really the MSSP)
void SPIOn();
//init SPI with the mode choice (1 -4), clock division (4,16,64 - not used for slave), master mode or slave mode ('m'.'s')
int initSPI(int mode, int clockDivide, char masterSlave);
//8 bit SPI write
uint8_t writeSPIByte (unsigned char data);
//8 bit SPI read
unsigned char readSPIByte (void);
//16 bit SPI write
int writeSPIWord(uint8_t  setting);
//24 bit SPI write
int writeSPI24bits(uint32_t  setting);
//16 bit SPI read
int readSPIWord();
//turn off SPI module
void SPIOff();

#endif