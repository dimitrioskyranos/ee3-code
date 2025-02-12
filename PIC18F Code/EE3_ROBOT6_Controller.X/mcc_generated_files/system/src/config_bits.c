// Configuration bits for PIC18F57Q43

// CONFIG1L
#pragma config FEXTOSC = OFF      // External Oscillator Mode Selection (Oscillator not enabled)
#pragma config RSTOSC = HFINTOSC_64MHZ  // Reset Oscillator Selection (HFINTOSC with HFFRQ = 64 MHz)

// CONFIG1H
#pragma config CLKOUTEN = OFF     // Clock Out Enable bit (CLKOUT function is disabled)

// CONFIG2L
#pragma config WDTE = OFF         // Watchdog Timer Enable (WDT disabled)

// CONFIG2H
#pragma config MCLRE = EXTMCLR    // MCLR Pin Function Select bit (MCLR pin function is MCLR)

// CONFIG3L
#pragma config BOREN = OFF        // Brown-out Reset Enable (Brown-out Reset disabled)

// CONFIG4L
//#pragma config PWRTE = OFF        // Power-up Timer Enable (PWRT disabled)
