 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.2
 *
 * @version Package Version: 3.1.2
*/

/*
? [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"
#include "C:\Users\dimk2\Desktop\EE3\pic_nrf_communication\nrf24l01.h"
#include <string.h>



#define NRF24L01_TX_EX  0
#define NRF24L01_RX_EX  !NRF24L01_TX_EX

//static volatile uint8_t test = 0;

#define NRF24L01_TX_MODE  0
#define NRF24L01_RX_MODE  1

// Buffer for received data
//unsigned char rx_payload[32];
//
//// SPI Receive Complete Callback
//void SPI_ReceiveCompleteHandler()
//{
//     PORTD = SPI1_ByteRead();
//   
//}


int main(void)
{
   
    SYSTEM_Initialize();
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts 
    // Use the following macros to: 
    TRISF3 = 0;
    LATF3 = 1;// SPI power 3.3V 
    
    TRISCbits.TRISC6 = 0; // SCK1 as output
    TRISCbits.TRISC5 = 1; // SDI1 as input
    TRISCbits.TRISC4 = 0; // SDO1 as output
    SPI1CON0bits.EN = 1;
    SPI1CON0 = 0x82;   // EN=1, MST=1 (Master mode)
    SPI1CON1 = 0x40;   // CKE=1, CKP=0 (Mode 0)
    SPI1CON2 = 0x00;   // Default settings
    SPI1BAUD = 31;   // Set SPI speed as needed
    
    


    // Clear any existing flags
   // PIR3bits.SPI1RXIF = 0;
   // PIR3bits.SPI1TXIF = 0;
    
    SPI1_Initialize();
    SPI1_Open(HOST_CONFIG);
    
    TRISA0 = 0 ; //CSN pin
    TRISA1 = 0; //CE pin
    LATA0 = 1; //IDLE CSN HIGH - pull low to select slave when communication starts 
    LATA1 = 0; //IDLE CE LOW - pull high when communication starts
    //SPI1_RxCompleteCallbackRegister(SPI_ReceiveCompleteHandler);
    
    

        // Initialize parameters for nrf24l01_initialize
    // CONFIG without PRIM_RX bit set means TX mode
    unsigned char config = nrf24l01_CONFIG_PWR_UP | nrf24l01_CONFIG_EN_CRC;
    unsigned char opt_rx_standby_mode = 1;  // This option won't force RX if PRIM_RX=0
    unsigned char en_aa = nrf24l01_EN_AA_ENAA_ALL;
    unsigned char en_rxaddr = nrf24l01_EN_RXADDR_ERX_ALL;
    unsigned char setup_aw = nrf24l01_SETUP_AW_5BYTES;
    unsigned char setup_retr = nrf24l01_SETUP_RETR_ARD_500 | nrf24l01_SETUP_RETR_ARC_10;

    // Set the RF channel to 2.522 GHz
    // Fo = 2400 + RF_CH -> 2522 = 2400 + 122
    unsigned char rf_ch = 122;  
    unsigned char rf_setup = nrf24l01_RF_SETUP_RF_PWR_0 | nrf24l01_RF_SETUP_LNA_HCURR;

    unsigned char rx_addr_p0[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    unsigned char rx_addr_p1[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
    unsigned char tx_addr[5]    = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    unsigned char rx_addr_p2    = 0xC3;
    unsigned char rx_addr_p3    = 0xC4;
    unsigned char rx_addr_p4    = 0xC5;
    unsigned char rx_addr_p5    = 0xC6;

    unsigned char rx_pw_p0 = 32;  // Payload width for pipe 0
    unsigned char rx_pw_p1 = 32;  // Payload width for pipe 1
    unsigned char rx_pw_p2 = 0;
    unsigned char rx_pw_p3 = 0;
    unsigned char rx_pw_p4 = 0;
    unsigned char rx_pw_p5 = 0;

    // Initialize the nRF24L01 with the given parameters
    nrf24l01_initialize(config,
                        opt_rx_standby_mode,
                        en_aa,
                        en_rxaddr,
                        setup_aw,
                        setup_retr,
                        rf_ch,
                        rf_setup,
                        rx_addr_p0,
                        rx_addr_p1,
                        rx_addr_p2,
                        rx_addr_p3,
                        rx_addr_p4,
                        rx_addr_p5,
                        tx_addr,
                        rx_pw_p0,
                        rx_pw_p1,
                        rx_pw_p2,
                        rx_pw_p3,
                        rx_pw_p4,
                        rx_pw_p5);





        nrf24l01_irq_clear_all();
        nrf24l01_flush_tx();
        //test = 3; 
        //TMR1_Start();


    
    INTERRUPT_GlobalInterruptHighEnable(); 
    //TMR0_Start();
    //TMR0_TMRInterruptEnable();
    //TMR1_TMRInterruptEnable();
    
    TRISD = 0x00;  // Set all pins of PORTB as output
    PORTD = 0x00;  // Initialize PORTB to 0
    
   
    

    while(1)
    {
        
       //SPI1_BufferRead()..
       // set a flag on the spi interrupt and check with a while loop when the data are ready to be read. the reading of vfthe portb out is updated on the interrupt.
       // the buffer is already reading the buffer before i ask it to read
       // PORTD = 0;
        
        unsigned char payload[32] = "Hello, NRF24L01!";
        nrf24l01_write_tx_payload(payload, sizeof(payload));
        __delay_ms(100);
        
    }    
}

