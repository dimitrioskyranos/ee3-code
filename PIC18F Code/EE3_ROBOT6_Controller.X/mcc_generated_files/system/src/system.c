/**
 * System Driver Source File
 * 
 * @file system.c
 * 
 * @ingroup systemdriver
 * 
 * @brief This file contains the API implementation for the System driver.
 *
 * @version Driver Version 2.0.3
 *
 * @version Package Version 5.3.5
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

#include "../system.h"

void PMD_Initialize(void);

void SYSTEM_Initialize(void)
{
    CLOCK_Initialize();
    PIN_MANAGER_Initialize();
    ADC_Initialize();
    CLKREF_Initialize();
    PMD_Initialize();
    WWDT_Initialize();
    INTERRUPT_Initialize();
}

void PMD_Initialize(void)
{
    //IOCMD IOC enabled; CLKRMD CLKR enabled; SCANMD SCANNER enabled; CRCMD CRC enabled; FVRMD FVR enabled; SYSCMD SYSCLK enabled; HLVDMD HLVD enabled; 
    PMD0 = 0x0;
    //TMR0MD TMR0 enabled; TMR1MD TMR1 enabled; TMR2MD TMR2 enabled; TMR3MD TMR3 enabled; TMR4MD TMR4 enabled; TMR5MD TMR5 enabled; TMR6MD TMR6 enabled; SMT1MD SMT1 enabled; 
    PMD1 = 0x0;
    //ZCDMD ZCD enabled; CM1MD CM1 enabled; CM2MD CM2 enabled; ACTMD ACT enabled; ADCMD ADC enabled; DAC1MD DAC1 enabled; 
    PMD3 = 0x0;
    //CWG1MD CWG1 enabled; CWG2MD CWG2 enabled; CWG3MD CWG3 enabled; NCO1MD NCO1 enabled; NCO2MD NCO2 enabled; NCO3MD NCO3 enabled; DSM1MD DSM1 enabled; 
    PMD4 = 0x0;
    //CCP1MD CCP1 enabled; CCP2MD CCP2 enabled; CCP3MD CCP3 enabled; PWM1MD PWM1 enabled; PWM2MD PWM2 enabled; PWM3MD PWM3 enabled; 
    PMD5 = 0x0;
    //U1MD UART1 enabled; U2MD UART2 enabled; U3MD UART3 enabled; U4MD UART4 enabled; U5MD UART5 enabled; SPI1MD SPI1 enabled; SPI2MD SPI2 enabled; I2C1MD I2C1 enabled; 
    PMD6 = 0x0;
    //CLC1MD CLC1 enabled; CLC2MD CLC2 enabled; CLC3MD CLC3 enabled; CLC4MD CLC4 enabled; CLC5MD CLC5 enabled; CLC6MD CLC6 enabled; CLC7MD CLC7 enabled; CLC8MD CLC8 enabled; 
    PMD7 = 0x0;
    //DMA1MD DMA1 enabled; DMA2MD DMA2 enabled; DMA6MD DMA6 enabled; DMA3MD DMA3 enabled; DMA4MD DMA4 enabled; DMA5MD DMA5 enabled; 
    PMD8 = 0x0;
}

