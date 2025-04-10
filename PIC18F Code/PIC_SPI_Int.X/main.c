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
// Configuration bits for PIC18F57Q43
//#pragma config FEXTOSC = OFF    
//#pragma config RSTOSC = HFINTOSC_64MHZ  
//#pragma config CLKOUTEN = OFF   
//#pragma config MCLRE = EXTMCLR  
//#pragma config WDTE = OFF       
//#pragma config LPBOREN = OFF    
//#pragma config BOREN = OFF     


void initADC(void);
uint16_t readADC(uint8_t channel);
uint16_t readRelativeADC(uint8_t channel);
int delta_y_motor;
int delta_x_motor;
int delta_x_servo;
int delta_y_servo;
 uint16_t x_neutral; // Calibrate X-axis neutral value
//    __delay_ms(20);
    uint16_t y_neutral; // Calibrate Y-axis neutral value
//    __delay_ms(20);
    
    uint16_t x_neutral_servo; // Calibrate X-axis neutral value
//    __delay_ms(20);
    uint16_t y_neutral_servo; 
    uint16_t x_axis; // Joystick X-axis connected to D7
//        __delay_ms(20);
        uint16_t y_axis; // Joystick Y-axis connected to D2
//        __delay_ms(20);
        
         uint16_t x_servo; // Servo Joystick X-Axis connected to D6
//        __delay_ms(20);
        uint16_t y_servo; // Servo Joystick Y-Axis connected to D5
//        __delay_ms(20);
        uint16_t y_switch_servo; // Servo Joystick Switch connected to D4
//        __delay_ms(20);
        
        uint16_t grab_switch; 
    int limit1 = 500;  // Small tilt (Low speed)
    int limit2 = 1500;  // Moderate tilt (Medium speed)
    int limit3 = 2000; // Extreme tilt (High speed)
    
      //Y axis limits
    int limit1x = 300;  // Small tilt (Low speed)
    int limit2x = 700;  // Moderate tilt (Medium speed)
    int limit3x = 1000; // Extreme tilt (High speed)
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
uint8_t DataTx[32];
uint16_t baselineValue = 0;

    

void sample_and_send(){
         x_axis = readADC(0x1F); // Joystick X-axis connected to D7
//        __delay_ms(20);
         y_axis = readADC(0x1a); // Joystick Y-axis connected to D2
//        __delay_ms(20);
        
          x_servo = readADC(0x1e); // Servo Joystick X-Axis connected to D6
//        __delay_ms(20);
         y_servo = readADC(0x1d); // Servo Joystick Y-Axis connected to D5
//        __delay_ms(20);
         y_switch_servo = readADC(0x1c); // Servo Joystick Switch connected to D4
//        __delay_ms(20);
        
         grab_switch = readADC(0x19); // Motor   Joystick Switch connected to D1
//        __delay_ms(20);
        
        
        if (grab_switch!= 0x00){
             DataTx[5] = 0xFF;
        }
        else{
             DataTx[5] = 0x00;
        }
        
        
        if (y_switch_servo!= 0x00){
             DataTx[4] = 0xFF;
        }
        else{
             DataTx[4] = 0x00;
        }
       
        
//Motors     
        
delta_y_motor = (int)y_axis - (int)y_neutral;   

// Handle Y-axis movement (UP and DOWN)
if (delta_y_motor > limit3) {  
    DataTx[1] = 0xFF;  // High speed (right)
}  
else if (delta_y_motor > limit2) {  
    DataTx[1] = 0xE0;  // Medium speed (right)
}  
else if (delta_y_motor > limit1) {  
    DataTx[1] = 0xC0;  // Low speed (right)
}  
else if (delta_y_motor <  -limit3) {  
    DataTx[1] = 0x00;  // High speed (left)
}  
else if (delta_y_motor <  -limit2) {  
    DataTx[1] = 0x20;  // Medium speed (left)
}  
else if (delta_y_motor < -limit1) {  
    DataTx[1] = 0x40;  // Low speed (left)
}  
else {  
    DataTx[1] = 0x80;  // Neutral (no movement)
}  

delta_x_motor = (int)x_axis - (int)x_neutral;  
// Handle X-axis movement (LEFT and RIGHT)
if (delta_x_motor > limit3x) {  
    DataTx[0] = 0xFF;  // High speed (up)
}  
else if (delta_x_motor > limit2x) {  
    DataTx[0] = 0xE0;  // Medium speed (up)
}  
else if (delta_x_motor > limit1x) {  
    DataTx[0] = 0xC0;  // Low speed (up)
}  
else if (delta_x_motor < -limit3x) {  
    DataTx[0] = 0x00;  // High speed (down)
}  
else if (delta_x_motor < -limit2x) {  
    DataTx[0] = 0x20;  // Medium speed (down)
}  
else if (delta_x_motor < -limit1x) {  
    DataTx[0] = 0x40;  // Low speed (down)
}  
else {  
    DataTx[0] = 0x80;  // Neutral (no movement)
}
        
        
//Servo
delta_y_servo = (int)y_servo - (int)y_neutral_servo;    

// Handle Y-axis movement (UP and DOWN)
if (delta_y_servo> limit3) {  
    DataTx[3] = 0xFF;  // High speed (right)
}  
else if (delta_y_servo > limit2) {  
    DataTx[3] = 0xE0;  // Medium speed (right)
}  
else if (delta_y_servo > limit1) {  
    DataTx[3] = 0xC0;  // Low speed (right)
}  
else if (delta_y_servo < -limit3) {  
    DataTx[3] = 0x00;  // High speed (left)
}  
else if (delta_y_servo < -limit2) {  
    DataTx[3] = 0x20;  // Medium speed (left)
}  
else if (delta_y_servo < -limit1) {  
    DataTx[3] = 0x40;  // Low speed (left)
}  
else {  
    DataTx[3] = 0x80;  // Neutral (no movement)
}  


delta_x_servo = (int)x_servo - (int)x_neutral_servo; 

// Handle X-axis movement (LEFT and RIGHT)
if (delta_x_servo > limit3x) {  
    DataTx[2] = 0xFF;  // High speed (up)
}  
else if (delta_x_servo > limit2x) {  
    DataTx[2] = 0xE0;  // Medium speed (up)
}  
else if (delta_x_servo > limit1x) {  
    DataTx[2] = 0xC0;  // Low speed (up)
}  
else if (delta_x_servo < -limit3x) {  
    DataTx[2] = 0x00;  // High speed (down)
}  
else if (delta_x_servo < -limit2x) {  
    DataTx[2] = 0x20;  // Medium speed (down)
}  
else if (delta_x_servo < -limit1x) {  
    DataTx[2] = 0x40;  // Low speed (down)
}  
else {  
    DataTx[2] = 0x80;  // Neutral (no movement)
}
          


        
        //__delay_ms(50);  // Adjust delay as needed
//        uint8_t Dtx[4] = {0x80,0x60,0x40,0x50};
        
        
     if(nrf_send_data(DataTx) == 1)
        {
            LATFbits.LATF3 =1;
            
            
            
            
        }
     
     
}

int main(void)
{
   
    SYSTEM_Initialize();
     INTERRUPT_GlobalInterruptHighEnable(); 
    TRISCbits.TRISC0 = 0;
    LATC0 = 1;
    // Enable the Global High Interrupts 
    //INTERRUPT_GlobalInterruptHighEnable(); 
//    TMR0_Start();
//    TMR0_TMRInterruptEnable();
    SPI1_Initialize();
    SPI1_Open(HOST_CONFIG);
    
    TRISBbits.TRISB1 = 0;//CE pin
    LATB1 = 1;
    
    TRISBbits.TRISB2 = 0;//CS pin
    LATB2 = 0;
    
    TRISFbits.TRISF0 = 0;//3.3V PWR
    LATFbits.LATF0 = 1; 
    
    TRISFbits.TRISF3 = 0;//LED Toggle
    TRISFbits.TRISF2 = 0;//LED Togglex  
    
//    WPUBbits.WPUB = 1;  // Enable global weak pull-ups
TRISBbits.TRISB0 = 1;  // Set RB0 as an input
WPUBbits.WPUB0 = 1;    // Enable pull-up on RB0
TRISFbits.TRISF4 = 1;
WPUFbits.WPUF4 = 0;
//TRISDbits.TRISD2 = 1;
//TRISDbits.TRISD3 = 1;
//TRISDbits.TRISD5 = 1;
//TRISDbits.TRISD6 = 1;

    
    init_NRF();
    NRF_TxMode(TxAddress, 122);
    initADC();
    
   
    x_neutral = readADC(0x1F); // Calibrate X-axis neutral value
//    __delay_ms(20);
    y_neutral = readADC(0x1A); // Calibrate Y-axis neutral value
//    __delay_ms(20);
    
    x_neutral_servo = readADC(0x1E); // Calibrate X-axis neutral value
//    __delay_ms(20);
    y_neutral_servo = readADC(0x1D); // Calibrate Y-axis neutral value
//    __delay_ms(20);
    
    //Y axis limits
    
    
   
//    TMR0_Start();
//    TMR0_TMRInterruptEnable();

/*
 Motor Joystick: SW = D1 , VRy = D2 , VRx = D3
 * 
 * Servo Joystick: SW = D4, VRy = D5, VRx = D6
 * 
 * 
 * NRF: CE = B1, CSN = B2, SCK = C3, MISO = C4, MOSI = C5
 
 
 */
    //
   
    
    
    while(1)
    {
//        if(time_to_sample){
        sample_and_send();
//        time_to_sample = false;
//        }
        
    }



    
}

void initADC(void) {
    ADCON0 = 0x00;           // Turn off ADC for configuration
    ADREF = 0x00;            // VDD as positive reference, VSS as negative reference
    ADCLK = 0x3F;            // ADC clock selection (Fosc/64)
    ADCON0bits.ADFM = 1;     // Right justify the result
    ADCON0bits.ADON = 1;     // Turn on ADC module

    // Take an initial baseline reading
    baselineValue = readADC(0x1e);
}


// Read ADC value from the selected channel
uint16_t readADC(uint8_t channel) {
    ADPCH = channel;         // Select ADC channel
    ADCON0bits.ADCONT = 0;
    ADCON0bits.ADGO = 1;       // Start ADC conversion
//    while (ADCON0bits.ADGO);   // Wait for conversion to complete
    return (uint16_t)(ADRESH << 8) | ADRESL; // Return 10-bit result
}


