#include <xc.h>
#include "C:\Users\dimk2\Desktop\EE3\PIC_Assignment_Dimitrios_Kyranos.X\mcc_generated_files\spi\spi1.h"

// Example function: Turn SPI On
//void SPIOn(){
//    // Enable SPI1
//    SPI1CON0bits.EN = 1;
//    // Enable SPI1 receive interrupt (if desired)
//    PIE3bits.SPI1RXIE = 1;
//}

// Example SPI initialization function for PIC18F57Q43
// mode: 1,2,3,4 as defined previously (SPI mode)
// clockDivide: user-defined, you must convert this into SPI1BAUD value
// masterSlave: 'm' for master, 's' for slave
//int initSPI(int mode, int clockDivide, char masterSlave){
//    // Disable SPI before configuring
//    SPI1CON0bits.EN = 0;
//
//    // Set Master/Slave
//    if (masterSlave == 'm') {
//        SPI1CON0bits.MST = 1; // Master mode
//        // Set up baud rate. Convert desired divide into SPI1BAUD.
//        // For example, if Fosc = 64 MHz and we want F_SCK = Fosc/4 = 16 MHz:
//        // BAUD = (Fosc/(2*F_SCK))-1 = (64/(2*16))-1= (64/32)-1=2-1=1
//        // Adjust this calculation for your actual clock.
//        if(clockDivide == 4) {
//            SPI1BAUD = 1;  // example for division by 4
//        } else if(clockDivide == 16) {
//            // e.g. BAUD = (Fosc/(2*(Fosc/16)))-1 = 7 (this is just example)
//            SPI1BAUD = 7; 
//        } else if(clockDivide == 64) {
//            // Another example calculation
//            SPI1BAUD = 31; 
//        } else {
//            return 1; //error code 1: unsupported clock divider
//        }
//        // In Master mode, you might want SMP = 1 for high speed:
//        // SMP in this device sets where the input data is sampled.
//        // Typically, SMP = 0: Input sampled middle of data output time
//        // SMP = 1: Input sampled at end.
//        // Adjust as desired:
//        SPI1CON1bits.SMP = 0; 
//    } else if (masterSlave == 's') {
//        SPI1CON0bits.MST = 0; // Slave mode
//        SPI1CON1bits.SMP = 0;
//        // In slave mode, BAUD is not used.
//    } else {
//        return 1; // invalid masterSlave character
//    }
//
//    // Set SPI mode based on mode number:
//    // mode 1: CKE=1, CKP=0
//    // mode 2: CKE=0, CKP=0
//    // mode 3: CKE=1, CKP=1
//    // mode 4: CKE=0, CKP=1
//    switch(mode) {
//        case 1:
//            SPI1CON1bits.CKP = 0;
//            SPI1CON1bits.CKE = 1;
//            break;
//        case 2:
//            SPI1CON1bits.CKP = 0;
//            SPI1CON1bits.CKE = 0;
//            break;
//        case 3:
//            SPI1CON1bits.CKP = 1;
//            SPI1CON1bits.CKE = 1;
//            break;
//        case 4:
//            SPI1CON1bits.CKP = 1;
//            SPI1CON1bits.CKE = 0;
//            break;
//        default:
//            return 2; //error code 2: invalid mode
//    }
//
//    // Configure other SPI settings:
//    // Default to standard bit ordering (MSB first), etc.
//    SPI1CON0bits.BMODE = 0; // 8-bit mode
//    SPI1CON2 = 0x00; // Default, adjust as needed
//
//    // Enable the SPI interrupt if needed:
//    PIE3bits.SPI1RXIE = 1;
//
//    // Enable SPI
//    SPI1CON0bits.EN = 1;
//    return 0;
//}

uint8_t writeSPIByte(uint8_t data)
{
    // Write data to TX buffer'
       
    if(SPI1_IsTxReady())
    {
        
       SPI1_ByteWrite(data);
    }
    while (SPI1CON2bits.BUSY);
    // Read the received data
    uint8_t received = SPI1_ByteRead(); 
    
    return received;
}

//unsigned char readSPIByte(){
//    // To read, send dummy byte
//    return writeSPIByte(0x00);
//}

//int writeSPIWord(unsigned short setting)
//{
//    unsigned char b1, b2;
//    // Send high byte
//    b1 = writeSPIByte((unsigned char)(setting >> 8));
//    // Send low byte
//    b2 = writeSPIByte((unsigned char)(setting & 0xFF));
//    return ((int)b1 << 8) | b2;
//}
//
//int writeSPI24bits(unsigned long setting)
//{
//    unsigned char b1, b2, b3;
//    uint32_t data;
//
//    b1 = writeSPIByte((unsigned char)(setting >> 16));
//    b2 = writeSPIByte((unsigned char)(setting >> 8));
//    b3 = writeSPIByte((unsigned char)setting);
//
//    // Combine the three bytes into a 32-bit value
//    data = ((uint32_t)b1 << 16) | ((uint32_t)b2 << 8) | (uint32_t)b3;
//
//    // Return as int (may lose high bits if int is 16-bit)
//    return (int)data;
//}

//int readSPIWord(){
//    unsigned char b1, b2;
//    b1 = writeSPIByte(0x00);
//    b2 = writeSPIByte(0x00);
//    return ((int)b1 << 8) | b2;
//}
////
//void SPIOff(){
//    // Disable SPI module
//    SPI1CON0bits.EN = 0;
//}
