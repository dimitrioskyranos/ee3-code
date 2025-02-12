#include <xc.h>
#include <stdint.h>

// Configuration bits for PIC18F57Q43
#pragma config FEXTOSC = OFF    
#pragma config RSTOSC = HFINTOSC_64MHZ  
#pragma config CLKOUTEN = OFF   
#pragma config MCLRE = EXTMCLR  
#pragma config WDTE = OFF       
#pragma config LPBOREN = OFF    
#pragma config BOREN = OFF      

// Define pins for LEDs
#define LED0 LATDbits.LATD0
#define LED1 LATDbits.LATD1
#define LED2 LATDbits.LATD2
#define LED3 LATDbits.LATD3

#define _XTAL_FREQ 64000000

// Function Prototypes
void initADC(void);
uint16_t readADC(uint8_t channel);

void main(void) {
    
    // Initialize LEDs as output
    TRISDbits.TRISD0 = 0;  // LED1 (pin RD0) as output
    TRISDbits.TRISD1 = 0;  // LED2 (pin RD1) as output
    TRISDbits.TRISD2 = 0;  // LED3 (pin RD2) as output
    TRISDbits.TRISD3 = 0;  // LED4 (pin RD3) as output

    // Turn off LEDs initially
    LED0 = 0;
    LED1 = 0;
    LED2 = 0;
    LED3 = 0;

    // Initialize ADC for joystick
    initADC();
    
    uint16_t x_neutral = readADC(0x1e); // Calibrate X-axis neutral value
    __delay_ms(20);
    uint16_t y_neutral = readADC(0x1d); // Calibrate Y-axis neutral value
    __delay_ms(20);
    
    uint16_t limit = 500;  // Tolerance limit for joystick neutrality
    
    while (1) {
        // Read joystick X and Y axes (assuming connected to AN6 and AN5)
        uint16_t y_axis = readADC(0x1e); // Joystick Y-axis connected to AN5
        __delay_ms(20);
        uint16_t x_axis = readADC(0x1d); // Joystick X-axis connected to AN6
        __delay_ms(20);
        
        
        // Handle X-axis movement (LEFT and RIGHT)
        if (x_axis > x_neutral + limit) {
            LED3 = 1; // Turn on RIGHT LED
            LED2 = 0;
        }
        else if (x_axis < x_neutral - limit) {
            LED3 = 0;
            LED2 = 1; // Turn on LEFT LED
        }
        else {
            LED3 = 0;
            LED2 = 0; // Joystick is neutral in X-axis
        }
        
        //__delay_ms(50);
        
        // Handle Y-axis movement (UP and DOWN)
        if (y_axis > y_neutral + limit) {
            LED1 = 1; // Turn on UP LED
            LED0 = 0;
        }
        else if (y_axis < y_neutral - limit) {
            LED1 = 0;
            LED0 = 1; // Turn on DOWN LED
        }
        else {
            LED1 = 0;
            LED0 = 0; // Joystick is neutral in Y-axis
        }
        
        __delay_ms(50);  // Adjust delay as needed
    }
}

// Initialize the ADC module
void initADC(void) {
    ADCON0 = 0x00;           // Turn off ADC for configuration
    ADREF = 0x00;            // VDD as positive reference, VSS as negative reference
    ADCLK = 0x3F;            // ADC clock selection (Fosc/64)
    ADCON0bits.ADFM = 1;     // Right justify the result
    ADCON0bits.ADON = 1;     // Turn on ADC module
}

// Read ADC value from the selected channel
uint16_t readADC(uint8_t channel) {
    ADPCH = channel;         // Select ADC channel
    ADCON0bits.ADCONT = 0;
    ADCON0bits.ADGO = 1;       // Start ADC conversion
    while (ADCON0bits.ADGO);   // Wait for conversion to complete
    return (uint16_t)(ADRESH << 8) | ADRESL; // Return 10-bit result
}
