

#ifndef SG90_H_
#define SG90_H_

#include <stdio.h>
#include <sys/_intsup.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "hal/ledc_types.h"
#include "portmacro.h"
#include "esp_log.h"

//PWM settings
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (5) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (2457) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          (50) // Frequency in Hertz. 


typedef struct {
    ledc_channel_t channel;
    ledc_timer_t timer;
    ledc_mode_t speed_mode;
    int gpio_num;
} sg90_servo_t;


void servo_init(sg90_servo_t *servo);
void servo_pos(sg90_servo_t *servo, int pos);


#endif 
