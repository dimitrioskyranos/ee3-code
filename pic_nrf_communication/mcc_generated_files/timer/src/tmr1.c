/**
  * TMR1 Generated Driver File
  *
  * @file tmr1.c
  *
  * @ingroup tmr1
  *
  * @brief Driver implementation for the TMR1 driver
  *
  * @version TMR1 Driver Version 4.0.0
  * 
  * @version Package Version 5.0.0
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

/**
 * Section: Included Files
*/

#include <xc.h>
#include "../tmr1.h"
#include "../../system/interrupt.h"
#include "C:\Users\dimk2\Desktop\EE3\pic_nrf_communication\nrf24l01.h"
 
static volatile uint16_t timer1ReloadVal;
static void (*TMR1_OverflowCallback)(void);
static void (*TMR1_GateCallback)(void);
static void TMR1_DefaultOverflowCallback(void);
static void TMR1_DefaultGateCallback(void);

void TMR1_Initialize(void)
{
    T1CONbits.TMR1ON = 0;             // TMRON disabled

    T1GCON = (0 << _T1GCON_T1GGO_POSN)   // T1GGO done
        | (0 << _T1GCON_T1GSPM_POSN)   // T1GSPM disabled
        | (0 << _T1GCON_T1GTM_POSN)   // T1GTM disabled
        | (0 << _T1GCON_T1GPOL_POSN)   // T1GPOL low
        | (0 << _T1GCON_T1GE_POSN);  // T1GE disabled

    T1GATE = (0 << _T1GATE_GSS_POSN);  // GSS T1G_pin

    T1CLK = (3 << _T1CLK_CS_POSN);  // CS HFINTOSC

    TMR1H = 0xF3;              // Period 50.01563us; Timer clock 64000000 Hz;
    TMR1L = 0x7F;

    timer1ReloadVal=((uint16_t)TMR1H << 8) | TMR1L;
    TMR1_OverflowCallback = TMR1_DefaultOverflowCallback;
    TMR1_GateCallback = TMR1_DefaultGateCallback;
    
    PIR3bits.TMR1IF = 0;   
    PIE3bits.TMR1IE = 1;
  
    T1CON = (1 << _T1CON_TMR1ON_POSN)   // TMR1ON enabled
        | (0 << _T1CON_T1RD16_POSN)   // T1RD16 disabled
        | (1 << _T1CON_nT1SYNC_POSN)   // nT1SYNC do_not_synchronize
        | (0 << _T1CON_CKPS_POSN);  // CKPS 1:1
}

void TMR1_Deinitialize(void)
{
    T1CONbits.TMR1ON = 0;             // TMRON disabled
    
    T1CON = 0x0;
    T1GCON = 0x0;
    T1GATE = 0x0;
    T1CLK = 0x0;
    TMR1H = 0x0;
    TMR1L = 0x0;

    PIR3bits.TMR1IF = 0;
    PIE3bits.TMR1IE = 0;
    PIR3bits.TMR1GIF = 0;
    PIE3bits.TMR1GIE = 0;
}

void TMR1_Start(void)
{   
    T1CONbits.TMR1ON = 1;
}

void TMR1_Stop(void)
{ 
    T1CONbits.TMR1ON = 0;
}

uint16_t TMR1_CounterGet(void)
{
    uint16_t readVal;
    uint8_t readValHigh;
    uint8_t readValLow;
    	
    readValLow = TMR1L;
    readValHigh = TMR1H;
    
    readVal = ((uint16_t)readValHigh << 8) | readValLow;

    return readVal;
}

void TMR1_CounterSet(uint16_t timerVal)
{
    if (1U == T1CONbits.nT1SYNC)
    {
        bool onState = T1CONbits.TMR1ON;
      
        TMR1H = (uint8_t)(timerVal >> 8);
        TMR1L = (uint8_t)timerVal;
       
        T1CONbits.TMR1ON = onState;
    }
    else
    {      
        TMR1H = (uint8_t)(timerVal >> 8);
        TMR1L = (uint8_t)timerVal;
    }
}

void TMR1_PeriodSet(uint16_t periodVal)
{
    timer1ReloadVal = periodVal;
}

uint16_t TMR1_PeriodGet(void)
{
    return timer1ReloadVal;
}

void TMR1_Reload(void)
{
    /* cppcheck-suppress misra-c2012-8.7 */
    TMR1_CounterSet(timer1ReloadVal);
}

uint16_t TMR1_MaxCountGet(void)
{
    return TMR1_MAX_COUNT;
}

void TMR1_SinglePulseAcquisitionStart(void)
{
    T1GCONbits.T1GGO = 1;
}

uint8_t TMR1_GateStateGet(void)
{
    return (T1GCONbits.T1GVAL);
}

void TMR1_TMRInterruptEnable(void)
{
    PIE3bits.TMR1IE = 1;
}

void TMR1_TMRInterruptDisable(void)
{
    PIE3bits.TMR1IE = 0;
}

void __interrupt(irq(TMR1),base(8)) TMR1_OverflowISR()
{
    /* cppcheck-suppress misra-c2012-8.7 */
    TMR1_CounterSet(timer1ReloadVal);

    // The ticker is set to 1 -> The callback function gets called every time this ISR executes.
    if(NULL != TMR1_OverflowCallback)
    {
        TMR1_OverflowCallback();
    }
    PIR3bits.TMR1IF = 0;
}

void TMR1_OverflowCallbackRegister(void (*CallbackHandler)(void))
{
    TMR1_OverflowCallback = CallbackHandler;
}

void TMR1_GateCallbackRegister(void (*CallbackHandler)(void))
{
    TMR1_GateCallback = CallbackHandler;
}

static void TMR1_DefaultOverflowCallback(void)
{
    // Default overflow callback
}

static void TMR1_DefaultGateCallback(void)
{
    // Default Gate callback
    if (test == 1){
    LATA1 = 1;
    TMR1_Stop();
    }
    
    else if (test == 2){
        LATA1 = 0;
        TMR1_Stop();
        
    }
    
    else if (test == 3){
        nrf24l01_flush_rx();
        TMR1_Stop();
    }
    
    
}


/**
  End of File
*/