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
© [2024] Microchip Technology Inc. and its subsidiaries.

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




/*
    Main application
*/
 //static volatile uint8_t test = 0;
 

//void SPI_TransmitCompleteHandler()
//{
//    complete = true;  
//}

//void SPI_ReceiveCompleteHandler()
//{
//    test = SPI1_ByteRead();
//    PORTD = test;
//}

uint8_t TxAddress[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
uint8_t DataTx[32] = "Hello esp from pic\n";


int main(void)
{
   
    SYSTEM_Initialize();
    

    // Enable the Global High Interrupts 
    //INTERRUPT_GlobalInterruptHighEnable(); 
    TMR0_Start();
    TMR0_TMRInterruptEnable();
    SPI1_Initialize();
    SPI1_Open(HOST_CONFIG);
    
    TRISDbits.TRISD3 = 0;//CS pin
    LATD3 = 1;
    
    TRISDbits.TRISD2 = 0;//CE pin
    LATD2 = 0;
    
    TRISFbits.TRISF0 = 0;//3.3V PWR
    LATFbits.LATF0 = 1; 
    
    TRISFbits.TRISF3 = 0;//LED Toggle
    TRISFbits.TRISF2 = 0;//LED Toggle
    
    init_NRF();
    NRF_TxMode(TxAddress, 122);


    while(1)
    {
        LATFbits.LATF3 =0;
     if(nrf_send_data(DataTx) == 1)
        {
            LATFbits.LATF3 =1;
            __delay_ms(1000);
            
            
            
        }
     
     
         //LATFbits.LATF2 = ~ LATFbits.LATF2;
     
        
    }    
}