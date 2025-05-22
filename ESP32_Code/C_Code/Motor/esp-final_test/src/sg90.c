#include "sg90.h"
#include "esp_task_wdt.h"


 void servo_init(sg90_servo_t *servo)
{
    
	ledc_timer_config_t ledc_timer = {
        .speed_mode       = servo->speed_mode,
        .timer_num        = servo->timer,
        .duty_resolution  = LEDC_TIMER_13_BIT,
        .freq_hz          = LEDC_FREQUENCY, // Standard 50Hz for servos
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = servo->speed_mode,
        .channel        = servo->channel,
        .timer_sel      = servo->timer,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = servo->gpio_num,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}


 void servo_pos(sg90_servo_t *servo, int pos)
{

	 // Example pos mapping: 1 to 20 => duty 205 to 1000
	 static const int pos_to_duty[] = {
        205, 242, 284, 326, 368,
        410, 452, 494, 536, 578,
        600, 662, 704, 746, 788,
        830, 872, 914, 956, 1000
    };

    if (pos < 1 || pos > 20) {
        ESP_LOGW("SG90", "Invalid position: %d", pos);
        return;
    }

	int duty = pos_to_duty[pos - 1];
    ledc_set_duty(servo->speed_mode, servo->channel, duty);
    ledc_update_duty(servo->speed_mode, servo->channel);

}
