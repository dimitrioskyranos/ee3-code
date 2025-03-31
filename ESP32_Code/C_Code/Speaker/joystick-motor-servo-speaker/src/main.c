    #include <stdio.h>
    #include <string.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/spi_master.h"
    #include "esp_log.h"
    #include "driver/gpio.h"
    #include "nrf24.h"
    #include <stdint.h>
    #include <stddef.h>
    #include "driver/ledc.h"
    #include "esp_err.h"
    #include <math.h>

    
    #define IN1 7
    #define IN2 5
    #define ENA 6  // PWM channel 0
    // Motor B pins
    #define IN3 17
    #define IN4 15
    #define ENB 16  // PWM channel 1

    // PWM configuration
    #define PWM_FREQ 1000
    #define PWM_RES LEDC_TIMER_8_BIT
    #define MAX_DUTY_CYCLE 255


    //NRF CONFIG
    #define SPI_HOST       SPI2_HOST
    #define DMA_CHANNEL    0
    #define MOSI_PIN       13
    #define MISO_PIN       12
    #define SCLK_PIN       14
    #define CS_PIN         11
    #define CE_PIN         4 





#define SERVO_X_PIN 42       // GPIO pin for Servo X
#define SERVO_Y_PIN 41       // GPIO pin for Servo Y
#define SERVO_X_CHANNEL LEDC_CHANNEL_2  // PWM channel for Servo X
#define SERVO_Y_CHANNEL LEDC_CHANNEL_3  // PWM channel for Servo Y
#define SERVO_Z_PIN 40
#define SERVO_GRAB 39 
#define SERVO_Z_CHANNEL LEDC_CHANNEL_4 

// Servo PWM configuration
#define SERVO_FREQ 50       // 50 Hz (standard for servos)
#define SERVO_RES LEDC_TIMER_13_BIT  // 13-bit resolution for finer control
#define SERVO_MIN_DUTY_X 163  // Duty cycle for 0 degrees (1 ms pulse)
#define SERVO_MAX_DUTY_X 819  // Duty cycle for 180 degrees (2 ms pulse)
#define SERVO_MIN_DUTY_Y 163  // Duty cycle for 0 degrees (1 ms pulse)
#define SERVO_MAX_DUTY_Y 600  // Duty cycle for 180 degrees (2 ms pulse)
#define SERVO_MIN_DUTY_Z 605  // Duty cycle for 0 degrees (1 ms pulse)
#define SERVO_MAX_DUTY_Z 820  // Duty cycle for 180 degrees (2 ms pulse)
#define SERVO_NEUTRAL_DUTY_X 491  // Duty cycle for 90 degrees (1.5 ms pulse)
#define SERVO_NEUTRAL_DUTY_Y 350
#define SERVO_NEUTRAL_DUTY_Z 499  

//servoPositionsLast
int lastX = SERVO_NEUTRAL_DUTY_X;
int lastY = SERVO_NEUTRAL_DUTY_Y;
int lastZ = SERVO_NEUTRAL_DUTY_Z;

// Adjust how fast the servo moves (tune this value as needed)
#define SERVO_SPEED_SCALING 2  // Higher = faster movement


// Speaker Constants
#define SPEAKER_PIN 38
#define SWITCH_PIN 37
#define SPEAKER_RES LEDC_TIMER_10_BIT
#define SPEAKER_FREQ1 600
#define SPEAKER_FREQ2 900
#define SPEAKER_DUTY 512 
#define SPEAKER_CHANNEL LEDC_CHANNEL_5 
volatile bool siren_active = false;  // Flag to track button press state


    spi_device_handle_t spi;

    uint8_t TxAddress[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t DataTx[] = "Hello, world";
    uint8_t RxData[32];

    static const char *TAG = "SPI_MASTER";

    void spi_initialization()
    {
        // SPI bus configuration
        spi_bus_config_t buscfg = {
            .mosi_io_num = MOSI_PIN,
            .miso_io_num = MISO_PIN,
            .sclk_io_num = SCLK_PIN,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            
        };

        // Initialize the SPI bus
        ESP_ERROR_CHECK(spi_bus_initialize(SPI_HOST, &buscfg, DMA_CHANNEL));

        // SPI device configuration
        spi_device_interface_config_t devcfg = {
            .clock_speed_hz = 500 * 1000, // Start with 500 kHz
            .mode = 0,                         // SPI Mode 0 (CPOL = 0, CPHA = 0)
            .spics_io_num = CS_PIN,            // CS pin
            .queue_size = 7, 
            .flags = SPI_DEVICE_NO_DUMMY,                        // MSB first (default)
        };

        // Add the SPI device
        ESP_ERROR_CHECK(spi_bus_add_device(SPI_HOST, &devcfg, &spi));

        ESP_LOGI(TAG, "SPI Master initialized for NRF24L01");
    }

    void espNRFPins(){
         // config CS
        gpio_reset_pin(CS_PIN);
        gpio_set_direction(CS_PIN, GPIO_MODE_OUTPUT);
        // config CE
        gpio_reset_pin(CE_PIN);
        gpio_set_direction(CE_PIN, GPIO_MODE_OUTPUT);
        CS_unselect();
        CE_disable();
    }


void setup_switch() {
    gpio_config_t switch_conf = {
        .pin_bit_mask = (1ULL << SWITCH_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,  // Enable internal pull-up resistor
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&switch_conf);
}

    // Function to initialize the servo
    void setup_servo() {
        // Configure PWM timer for the servos
        ledc_timer_config_t servo_timer = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .timer_num = LEDC_TIMER_1,  // Use a different timer for the servos
            .duty_resolution = SERVO_RES,
            .freq_hz = SERVO_FREQ,
            .clk_cfg = LEDC_AUTO_CLK
        };
        ledc_timer_config(&servo_timer);

        
    
        // Configure PWM channel for Servo X
        ledc_channel_config_t servoX_channel = {
            .gpio_num = SERVO_X_PIN,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = SERVO_X_CHANNEL,
            .timer_sel = LEDC_TIMER_1,
            .duty = SERVO_NEUTRAL_DUTY_X,  // Start at neutral position
            .hpoint = 0
        };
        ledc_channel_config(&servoX_channel);

    
        // Configure PWM channel for Servo Y
        ledc_channel_config_t servoY_channel = {
            .gpio_num = SERVO_Y_PIN,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = SERVO_Y_CHANNEL,
            .timer_sel = LEDC_TIMER_1,
            .duty = SERVO_NEUTRAL_DUTY_Y,  // Start at neutral position
            .hpoint = 0
        };
        ledc_channel_config(&servoY_channel);

        ledc_channel_config_t servoZ_channel = {
            .gpio_num = SERVO_Z_PIN,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = SERVO_Z_CHANNEL,
            .timer_sel = LEDC_TIMER_1,
            .duty = SERVO_NEUTRAL_DUTY_Z,  // Start at neutral position
            .hpoint = 0
        };
        ledc_channel_config(&servoZ_channel);
    }


    void setup_pwm() {
    // Configure PWM for Motor A
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = PWM_RES,
        .freq_hz          = PWM_FREQ,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t motorA_channel = {
        .gpio_num   = ENA,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&motorA_channel);

    // Configure PWM for Motor B
    ledc_channel_config_t motorB_channel = {
        .gpio_num   = ENB,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_1,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&motorB_channel);
    
    }


    void setup_speaker(){
        ledc_timer_config_t speaker_timer = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .timer_num = LEDC_TIMER_2,  // Use a different timer for the servos
            .duty_resolution = SPEAKER_RES,
            .freq_hz = SPEAKER_FREQ1,
            .clk_cfg = LEDC_AUTO_CLK
        };
        ledc_timer_config(&speaker_timer);
        // Configure PWM channel for Servo X
        ledc_channel_config_t speaker_channel = {
            .gpio_num = SPEAKER_PIN,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = SPEAKER_CHANNEL,
            .timer_sel = LEDC_TIMER_2,
            .duty = 0,  // Start silent
            .hpoint = 0
        };
        ledc_channel_config(&speaker_channel);
    }




    void setup_gpio() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << IN1) | (1ULL << IN2) | (1ULL << IN3) | (1ULL << IN4),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    }


    void siren_task(void *arg) {
        uint32_t current_freq = SPEAKER_FREQ1;
    
        while (1) {
            if (siren_active) {
                // Toggle frequency between 2kHz and 3kHz
                current_freq = (current_freq == SPEAKER_FREQ1) ? SPEAKER_FREQ2 : SPEAKER_FREQ1;
                ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_2, current_freq);
                ledc_set_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CHANNEL, SPEAKER_DUTY);  // Enable sound
                ledc_update_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CHANNEL);
            } else {
                // Mute speaker when conditions are not met
                ledc_set_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CHANNEL, 0);
                ledc_update_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CHANNEL);
            }
            vTaskDelay(pdMS_TO_TICKS(400));  // Switch frequency every 400ms
        }
    }


    void set_motorA(bool forward, int speed) {
    gpio_set_level(IN1, forward ? 1 : 0);
    gpio_set_level(IN2, forward ? 0 : 1);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, speed);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }

    void set_motorB(bool forward, int speed) {
    gpio_set_level(IN3, forward ? 1 : 0);
    gpio_set_level(IN4, forward ? 0 : 1);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, speed);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    }



// Function to control the servo based on servoX value
void control_servo(uint8_t servo_value, ledc_channel_t servo_channel) {
    int min = 0;
    int max = 0;
    int* lastPos = NULL;  // Pointer to last position variable

    // Identify servo channel and assign corresponding parameters
    if (servo_channel == SERVO_X_CHANNEL) {
        min = SERVO_MIN_DUTY_X;
        max = SERVO_MAX_DUTY_X;
        lastPos = &lastX;
    } 
    else if (servo_channel == SERVO_Y_CHANNEL) {
        min = SERVO_MIN_DUTY_Y;
        max = SERVO_MAX_DUTY_Y;
        lastPos = &lastY;
    } 
    else if (servo_channel == SERVO_Z_CHANNEL) {
        min = SERVO_MIN_DUTY_Z;
        max = SERVO_MAX_DUTY_Z;
        lastPos = &lastZ;
    }

    // If joystick is centered, do nothing (hold last position)
    if (servo_value == 0x80 || (servo_channel == SERVO_Z_CHANNEL && servo_value == 0xFF)) {
        return;
    }

    // Calculate movement step based on joystick deviation
    int step = ((servo_value - 0x80) / SERVO_SPEED_SCALING); 

    // Adjust last position gradually
    *lastPos += step;

    // Clamp the position within allowed servo limits
    if (*lastPos < min) *lastPos = min;
    if (*lastPos > max) *lastPos = max;

    // Set the duty cycle for the servo
    ledc_set_duty(LEDC_LOW_SPEED_MODE, servo_channel, *lastPos);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, servo_channel);
}


void control_motors(uint8_t x, uint8_t y) {
    int speedA = 0;
    int speedB = 0;
    bool forwardA = true;
    bool forwardB = true;

    // Forward/Backward logic
    if (y < 0x80) {
        // Joystick Up: Both motors move forward
        speedA = (0x80 - y) * 2; // Speed increases as y decreases
        speedB = (0x80 - y) * 2;
        forwardA = true;
        forwardB = true;
    } else if (y > 0x80) {
        // Joystick Down: Both motors move backward
        speedA = (y - 0x80) * 2; // Speed increases as y increases
        speedB = (y - 0x80) * 2;
        forwardA = false;
        forwardB = false;
    }

    // Left/Right logic
    if (x < 0x80) {
        // Joystick Left: Only Motor B moves forward
        speedA = 0; // Motor A is stopped
        speedB = (0x80 - x) * 2; // Speed increases as x decreases
        forwardA = true; // Doesn't matter since speedA is 0
        forwardB = true;
    } else if (x > 0x80) {
        // Joystick Right: Only Motor A moves forward
        speedA = (x - 0x80) * 2; // Speed increases as x increases
        speedB = 0; // Motor B is stopped
        forwardA = true;
        forwardB = true; // Doesn't matter since speedB is 0
    }

    // Ensure the speeds are within the valid range
    speedA = (speedA < 0) ? 0 : (speedA > MAX_DUTY_CYCLE) ? MAX_DUTY_CYCLE : speedA;
    speedB = (speedB < 0) ? 0 : (speedB > MAX_DUTY_CYCLE) ? MAX_DUTY_CYCLE : speedB;

    // Set the motor speeds and directions
    set_motorA(forwardA, speedA);
    set_motorB(forwardB, speedB);
}

    void app_main(void)
    {
        //catch everything on the serial monitor delay
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        ESP_LOGE(TAG,"Start to control the motors over nrf24");    


        spi_initialization();
        setup_pwm();  // Initialize PWM
        setup_gpio(); //GPIO Pins init
        setup_servo();
        setup_speaker();
        setup_switch();

        xTaskCreate(siren_task, "siren_task", 2048, NULL, 5, NULL);




        espNRFPins();


        init_NRF();
        NRF24_RxMode(TxAddress, 122);

        
        uint8_t joystickX, joystickY, servoX, servoY, servoSwitch;
       


        while(1)
        {
            
            if(isDataAvailable(1) == 1)
            {
                NRF24_Receive(RxData);

                joystickY = RxData[1];
                joystickX = RxData[0];
                servoX = RxData[2];
                servoY = RxData[3];
                servoSwitch = RxData[4];

            

                control_motors(joystickX, joystickY);

                // Control servos
            control_servo(servoX, SERVO_X_CHANNEL);  // Control Servo X
            control_servo(servoY, SERVO_Y_CHANNEL);  // Control Servo Y
            control_servo(servoSwitch, SERVO_Z_CHANNEL);



            bool switch_pressed = (gpio_get_level(SWITCH_PIN) == 0);  // LOW = pressed
            bool car_moving = (joystickX != 0x80 || joystickY != 0x80);  // Check if motors are moving
            siren_active = switch_pressed && car_moving;

                ESP_LOGI(TAG, "Joystick X: %d, Y: %d", joystickX, joystickY);
                ESP_LOGI(TAG, "Servo X: %d, Y: %d", servoX, servoY);

            


                vTaskDelay(pdMS_TO_TICKS(20));
            
                
            }
                

            
        }
    }

    uint8_t SPI_ByteExchange(uint8_t data) {
        uint8_t receivedByte = 0;

        spi_transaction_t trans = {
            .length = 8,            // 8 bits to send
            .tx_buffer = &data,     // Data to transmit
            .rx_buffer = &receivedByte, // Buffer for received data
        };

        ESP_ERROR_CHECK(spi_device_transmit(spi, &trans));
        return receivedByte;
    }
    
    void CS_select(void) {
        gpio_set_level(CS_PIN, 0);
    }

    void CS_unselect(void) {
        gpio_set_level(CS_PIN, 1);
    }

    void CE_enable(void) {
        gpio_set_level(CE_PIN, 1);
    }

    void CE_disable(void) {
        gpio_set_level(CE_PIN, 0);
    }

    void nrf24_writeReg(uint8_t reg, uint8_t data) {
        uint8_t buf[2];
        buf[0] = reg | (1 << 5);  // Register + W_REGISTER command
        buf[1] = data;
        
        // Put CS pin low to select device
        CS_select();
        SPI_ByteExchange(buf[0]);
        SPI_ByteExchange(buf[1]);
        CS_unselect();
    }

    void nrf24_writeRegMulti(uint8_t reg, uint8_t *data, size_t size) {
        uint8_t buf[2];
        buf[0] = reg | (1 << 5);  // Register + W_REGISTER command
        // Put CS pin low to select device
        CS_select();
        SPI_ByteExchange(buf[0]); 
        for (int i = 0; i<size; i++){
//       while (SPI1CON2bits.BUSY);
       SPI_ByteExchange(data[i]); 
        }
        CS_unselect();
    }

    uint8_t nrf24_readReg(uint8_t reg)
    {
        uint8_t data_read = 0;
        //Put CS pin low to select device
        CS_select();
        SPI_ByteExchange(reg);
        data_read = SPI_ByteExchange(0xff);
        CS_unselect();
        
        return data_read;
    }


    void nrfsendCmd(uint8_t cmd) {
        CS_select();
        SPI_ByteExchange(cmd);
        CS_unselect();
    }

    void init_NRF(void) {
        CE_disable();

        nrf24_reset (0);
        // Config reg
        nrf24_writeReg(CONFIG, 0);
        uint8_t t = nrf24_readReg(RF_CH);
        ESP_LOGI("DEBUG", "%d", t);
        // No Auto ack
        nrf24_writeReg(EN_AA, 0);
        // Disable all the pipes
        nrf24_writeReg(EN_RXADDR, 0);
        // Set reg address length
        nrf24_writeReg(SETUP_AW, 0x03);
        // No retransmission
        nrf24_writeReg(SETUP_RETR, 0);
        // Channel num
        nrf24_writeReg(RF_CH, 0);
        // Rate
        nrf24_writeReg(RF_SETUP, 0x0E);
        
        CE_enable();
    }

    void NRF_TxMode(uint8_t *Address, uint8_t channel) {
        CE_disable();
        
        nrf24_writeReg(RF_CH, channel); // channel freq
        uint8_t chn = nrf24_readReg(RF_CH);
        ESP_LOGI(TAG, "RF_CH: %02X", chn);
        nrf24_writeRegMulti(TX_ADDR, Address, 5);

        // Power up the device
        uint8_t config = nrf24_readReg(CONFIG);
        ESP_LOGI(TAG, "CONFIG: %02X", config);
        config = config | (1 << 1); // Set PWR_UP
        // config = config & (0xF2); // write 0 in the PRIM_RX, and 1 in the PWR_UP, and all other bits are masked
        nrf24_writeReg(CONFIG, config);
        
        CE_enable();
    }
    void NRF24_RxMode (uint8_t *Address, uint8_t channel)
    {
        CE_disable();
        
        nrf24_writeReg(RF_CH, channel); // channel freq
        uint8_t en_rxaddr = nrf24_readReg(EN_RXADDR);
        en_rxaddr = en_rxaddr | (1 << 1);
        nrf24_writeReg(EN_RXADDR, en_rxaddr); //Select pipe 1
        nrf24_writeRegMulti(RX_ADDR_P1, Address, 5);
        nrf24_writeReg(RX_PW_P1, 32);

        // Power up the device
        uint8_t config = nrf24_readReg(CONFIG);
        config = config | (1 << 1) | (1<<0); // Set PWR_UP
        nrf24_writeReg(CONFIG, config);
        
        CE_enable();
    }

    uint8_t isDataAvailable(int pipenum)
    {
        uint8_t status = nrf24_readReg(STATUS);

        if ((status&(1<<6))&&(status&(pipenum<<1)))
        {
            nrf24_writeReg(STATUS, (1<<6));

            return 1;
        }

        return 0;
    }

    void NRF24_Receive (uint8_t *data)
    {
        uint8_t cmdtosend = 0;

        // select the device
        CS_select();

        // payload command
        cmdtosend = R_RX_PAYLOAD;
        SPI_ByteExchange(cmdtosend);

        for (int i = 0; i<32; i++){
       //while (SPI1CON2bits.BUSY);
       data[i] = SPI_ByteExchange(0xff); 
    }


        // Unselect the device
        CS_unselect();

        vTaskDelay(1);

        cmdtosend = FLUSH_RX;
        nrfsendCmd(cmdtosend);
    }


    

    uint8_t nrf_send_data(uint8_t *data) {
        uint8_t cmdtosend = 0;
        
        CS_select();
        
        // Payload command
        cmdtosend = W_TX_PAYLOAD;
        
       SPI_ByteExchange(cmdtosend); 

        // Send data
        
       for (int i = 0; i<32; i++){
//       while (SPI1CON2bits.BUSY);
       SPI_ByteExchange(data[i]); 
        }
        
        CS_unselect();

        vTaskDelay(1);
        
        uint8_t fifostatus = nrf24_readReg(FIFO_STATUS);
        ESP_LOGI(TAG, "FIFO_STATUS: %02X", fifostatus);
        
        if ((fifostatus&(1<<4)) && (!(fifostatus&(1<<3)))) {

            cmdtosend = FLUSH_TX;
            nrfsendCmd(cmdtosend);

            nrf24_reset (FIFO_STATUS);
            
            return 1;
        }
        return 0;
    }

    void nrf24_reset(uint8_t REG)
    {
        if (REG == STATUS)
        {
            nrf24_writeReg(STATUS, 0x00);
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
        nrf24_writeReg(STATUS, 0x00);
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
