# EE3 Project Repository
(By Dimitrios Kyranos)
This is a list with all the credits from various sources that I consulted in the making of this project code. At 22/05/2025 all links were still accessible.

## References

- Robotic Arm Code and Motor Code (can be found on this folder under esp-final_motor_code)

These sources were consulted in the making of lines 169-260, 320-509

[2] Melek Cherif, PWM motor control using ESP32 & ESP-IDF
https://medium.com/@melek_cherif/pwm-motor-control-using-esp32-esp-idf-6c0e9bec4604
[3] ESP-IDF Programming guide, LED Control (LEDC) 
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/ledc.html


- NRF Code on ESP(can be found on this folder under esp-final_motor_code)

These sources were consulted in the making of lines 115-154, 511-553, 683-940 as well as the file nrf24.h

[1] Controllers Tech, How to Write Basic Library for NRF24L01 - PART 1,2,3
https://www.youtube.com/watch?v=X5XDSWQYYvU&list=PLfIJKC1ud8giTKW0nzHN71hud_238d-JO&ab_channel=ControllersTech


- NRF Code on PIC(can be found on this folder under PIC_Completedv2.X)

These sources were consulted in the making the files nrf24.h and nrf24.c

[1] Controllers Tech, How to Write Basic Library for NRF24L01 - PART 1,2,3
https://www.youtube.com/watch?v=X5XDSWQYYvU&list=PLfIJKC1ud8giTKW0nzHN71hud_238d-JO&ab_channel=ControllersTech


-Http Server and Uart Transmission Code on ESP(can be found on this folder under esp-final_camera)

These sources were consulted in the making of the file main.c 

[5] Esp32 Tutorials, ESP32 Web Server with ESP-IDF
https://esp32tutorials.com/esp32-web-server-esp-idf/
[6] Sajia96 and nachiketkukade, WiFi station example
https://github.com/espressif/esp-idf/blob/master/examples/wifi/getting_started/station/main/station_example_main.c
[7] kostaond, Static IP Example
https://github.com/espressif/esp-idf/blob/master/examples/protocols/static_ip/main/static_ip_example_main.c
[8] ESP-IDF Programming guide, Universal Asynchronous Receiver/Transmitter (UART)
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/uart.html
[9] Esp32 Tutorials, ESP32 Timer Interrupt using ESP-IDF
https://esp32tutorials.com/esp32-timer-interrupt-esp-idf/#more-1700

-General: FreeRTOS Tasks on ESP

This source was consulted for the making of the various event and task implementations in all ESP code. 
[4] Esp32 Tutorials, ESP32 FreeRTOS Event Groups with ESP-IDF
 https://esp32tutorials.com/esp32-freertos-event-groups-esp-idf/#more-1815


## Notes

NRF code for both the PIC and ESP was implemented in collaboration with Mykyta Chunikhin from Robot 1. 
Part of the code was refined using ChatGPT-4o (OpenAI, 2025) to debug issues during development.
