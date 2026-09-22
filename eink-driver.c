#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <stdint.h>

#define SPI_PORT spi0

//add actual pin numbers here
#define PIN_MISO 1
#define PIN_MOSI 2
#define PIN_CS 3
#define PIN_SCK 4
#define PIN_DC 5
#define PIN_RSTN 6
#define PIN_BUSY 7 // if BUSY = 1, pause sending commands, as the device is BUSY; if BUSY = 0, no interruption needed


int eink_init(){

    // setting the pins: 
    stdio_init_all();

    spi_init(spi0, 1000000); // 1 MHz init
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);

    gpio_set_function(PIN_CS, GPIO_OUT);
    gpio_init(PIN_CS);
    gpio_put(PIN_CS, 1);

    gpio_init(PIN_DC);
    gpio_set_function(PIN_DC, GPIO_OUT);
    gpio_put(PIN_DC, 1);

    gpio_init(PIN_RSTN);
    gpio_set_function(PIN_RSTN, GPIO_OUT);
    gpio_put(PIN_RSTN, 1);

    gpio_init(PIN_BUSY);
    gpio_set_function(PIN_BUSY, GPIO_IN);


    // physical hardware reset
    gpio_put(PIN_RSTN, 0);
    sleep_ms(5);
    gpio_put(PIN_RSTN, 1);


    //software reset
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 0);

    uint8_t cmd = 0x12;
    spi_write_blocking(SPI_PORT, &cmd, 1);
    gpio_put(PIN_CS, 1);
    while(gpio_get(PIN_BUSY)){
        sleep_ms(1);
    }

    // 10ms sleep
    sleep_ms(10);


    
    
}
