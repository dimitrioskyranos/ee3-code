#ifndef NRF24L01_H
#define NRF24L01_H

#include <xc.h>
#include <stdint.h>

// Define SPI communication pins
#define NRF_CE LATBbits.LATB0  // Chip Enable
#define NRF_CSN LATBbits.LATB1 // Chip Select Not
#define NRF_SCK LATCbits.LATC3 // Clock
#define NRF_MOSI LATCbits.LATC5 // Master Out Slave In
#define NRF_MISO PORTCbits.RC4  // Master In Slave Out
#define NRF_IRQ PORTBbits.RB2  // Interrupt

// nRF24L01 Instruction Set
#define R_REGISTER      0x00
#define W_REGISTER      0x20
#define R_RX_PAYLOAD    0x61
#define W_TX_PAYLOAD    0xA0
#define FLUSH_TX        0xE1
#define FLUSH_RX        0xE2
#define REUSE_TX_PL     0xE3
#define NOP             0xFF

// Registers
#define CONFIG          0x00
#define EN_AA           0x01
#define EN_RXADDR       0x02
#define SETUP_AW        0x03
#define SETUP_RETR      0x04
#define RF_CH           0x05
#define RF_SETUP        0x06
#define STATUS          0x07
#define OBSERVE_TX      0x08
#define RX_ADDR_P0      0x0A
#define TX_ADDR         0x10
#define RX_PW_P0        0x11

// Function prototypes
void nRF24_init();
void nRF24_config(uint8_t channel, uint8_t pay_length);
void nRF24_tx_address(uint8_t* addr);
void nRF24_rx_address(uint8_t* addr);
void nRF24_send(uint8_t* value);
uint8_t nRF24_dataReady();
void nRF24_getData(uint8_t* data);
uint8_t nRF24_isSending();
uint8_t nRF24_getStatus();
void nRF24_powerUpTx();
void nRF24_powerUpRx();
void nRF24_powerDown();

#endif
