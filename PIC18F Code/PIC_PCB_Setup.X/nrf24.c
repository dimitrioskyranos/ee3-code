#include "mcc_generated_files/system/system.h"

void CS_unselect(void)
{
    LATBbits.LATB2 = 1;
}

void CS_select(void)
{
    LATBbits.LATB2 = 0;
}

void CE_disable(void)
{
    LATBbits.LATB1 = 0;
}

void CE_enable(void)
{
    LATBbits.LATB1 = 1;
}

void nrf24_writeReg(uint8_t reg, uint8_t data)
{
    uint8_t buf[2];
    buf[0] = reg | (1 << 5);
    buf[1] = data;
    
    //Put CS pin low to select device
    CS_select();
    SPI1_ByteExchange(buf[0]);
//    while (SPI1CON2bits.BUSY);
    SPI1_ByteExchange(buf[1]);
    CS_unselect();
}
void nrf24_writeRegMulti(uint8_t reg, uint8_t *data, size_t size)
{
    uint8_t buf[2];
    buf[0] = reg | (1 << 5);
//    buf[1] = data;
    //Put CS pin low to select device
    CS_select();
    SPI1_ByteExchange(buf[0]);
    for (int i = 0; i<size; i++){
//       while (SPI1CON2bits.BUSY);
       SPI1_ByteExchange(data[i]); 
    }
    
    CS_unselect();
}

uint8_t nrf24_readReg(uint8_t reg)
{
    uint8_t data_read = 0;
    //Put CS pin low to select device
    CS_select();
    SPI1_ByteExchange(reg);
//    while (SPI1CON2bits.BUSY);
    data_read = SPI1_ByteExchange(NOP_NRF);
    CS_unselect();
    
    return data_read;
}

void nrfsendCmd(uint8_t cmd)
{
    CS_select();
    SPI1_ByteExchange(cmd);
    CS_unselect();
}

void init_NRF(void)
{
    // disable the chip before configuring the device
	CE_disable();
    

	// reset everything
	//nrf24_reset(0);

	nrf24_writeReg(CONFIG, 0);  // will be configured later
    uint8_t t = nrf24_readReg(RF_CH);
    t = t + 1;
	nrf24_writeReg(EN_AA, 0);  // No Auto ACK
    t = nrf24_readReg(CONFIG);
	nrf24_writeReg(EN_RXADDR, 0);  // Not Enabling any data pipe right now

	nrf24_writeReg(SETUP_AW, 0x03);  // 5 Bytes for the TX/RX address

	nrf24_writeReg(SETUP_RETR, 0);   // No retransmission

	nrf24_writeReg(RF_CH, 0);  // will be setup during Tx or RX

	nrf24_writeReg(RF_SETUP, 0x0E);   // Power= 0db, data rate = 2Mbps

	// Enable the chip after configuring the device
	CE_enable();
    

}

void NRF_TxMode(uint8_t *Address, uint8_t channel)
{
    // disable the chip before configuring the device
	CE_disable();
     

	nrf24_writeReg(RF_CH, channel);  // select the channel

	nrf24_writeRegMulti(TX_ADDR, Address, 5);  // Write the TX address
    

	// power up the device
	uint8_t config = nrf24_readReg(CONFIG);
    config = config | (1<<1);
	nrf24_writeReg(CONFIG, config);
    nrf24_readReg(RF_CH);
	// Enable the chip after configuring the device
	CE_enable();
}

uint8_t nrf_send_data(uint8_t *data)
{
    uint8_t cmdtosend = 0;
    
    CS_select();
    
    //payload command
    cmdtosend = W_TX_PAYLOAD;
    SPI1_ByteExchange(cmdtosend);

     
     for (int i = 0; i<32; i++){
       //while (SPI1CON2bits.BUSY);
       SPI1_ByteExchange(data[i]); 
    }

    CS_unselect();
    
//    __delay_us(1);
    
    uint8_t fifostatus = nrf24_readReg(FIFO_STATUS);
    
    if ((fifostatus&(1<<4)) && (!(fifostatus&(1<<3))))
    {
        cmdtosend = FLUSH_TX;
        nrfsendCmd(cmdtosend);
        
        return 1;
    }
    return 0;
}

void nrf24_reset(uint8_t REG)
    {
        if (REG == STATUS_NRF)
        {
            nrf24_writeReg(STATUS_NRF, 0x00);
        }

        else if (REG == FIFO_STATUS)
        {
            nrf24_writeReg(FIFO_STATUS, 0x11);
        }

        else {
        nrf24_writeReg(CONFIG, 0x08);
        nrf24_writeReg(EN_AA, 0x3F);
        nrf24_writeReg(EN_RXADDR, 0x03);
        nrf24_writeReg(SETUP_AW, 0x03);
        nrf24_writeReg(SETUP_RETR, 0x03);
        nrf24_writeReg(RF_CH, 0x02);
        nrf24_writeReg(RF_SETUP, 0x0E);
        nrf24_writeReg(STATUS_NRF, 0x00);
        nrf24_writeReg(OBSERVE_TX, 0x00);
        nrf24_writeReg(CD, 0x00);
        uint8_t rx_addr_p0_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
        nrf24_writeRegMulti(RX_ADDR_P0, rx_addr_p0_def, 5);
        uint8_t rx_addr_p1_def[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
        nrf24_writeRegMulti(RX_ADDR_P1, rx_addr_p1_def, 5);
        nrf24_writeReg(RX_ADDR_P2, 0xC3);
        nrf24_writeReg(RX_ADDR_P3, 0xC4);
        nrf24_writeReg(RX_ADDR_P4, 0xC5);
        nrf24_writeReg(RX_ADDR_P5, 0xC6);
        uint8_t tx_addr_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
        nrf24_writeRegMulti(TX_ADDR, tx_addr_def, 5);
        nrf24_writeReg(RX_PW_P0, 0);
        nrf24_writeReg(RX_PW_P1, 0);
        nrf24_writeReg(RX_PW_P2, 0);
        nrf24_writeReg(RX_PW_P3, 0);
        nrf24_writeReg(RX_PW_P4, 0);
        nrf24_writeReg(RX_PW_P5, 0);
        nrf24_writeReg(FIFO_STATUS, 0x11);
        nrf24_writeReg(DYNPD, 0);
        nrf24_writeReg(FEATURE, 0);
	    }
    }