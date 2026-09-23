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

// for all arrays, the first value is the command, and the following value(s) are the data values. 


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


    // setting gate driver output
    uint8_t gate_driver_config[] = {0x01, 0x27, 0x01, 0x00};
    gpio_put(PIN_DC,0);
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &gate_driver_config[0], 1);
    gpio_put(PIN_DC, 1);
    spi_write_blocking(SPI_PORT, &gate_driver_config[1], 3);
    gpio_put(PIN_CS, 1);
    sleep_ms(1);


    // setting display RAM size
    uint8_t data_entry_mode_setting[] = {0x11, 0x03};
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &data_entry_mode_setting[0], 1);
    gpio_put(PIN_DC, 1);
    spi_write_blocking(SPI_PORT, &data_entry_mode_setting[1], 1);
    gpio_put(PIN_CS, 1);
    sleep_ms(1);


    // setting RAM X address start and end position
    uint8_t ram_x_start_end[] = {0x44, 0x00, 0x15};
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &ram_x_start_end[0], 1);
    gpio_put(PIN_DC, 1);
    spi_write_blocking(SPI_PORT, &ram_x_start_end[1], 2);
    gpio_put(PIN_CS, 1);
    sleep_ms(1);


    // setting RAM Y address start and end position
    uint8_t ram_y_start_end[] = {0x45, 0x00, 0x00, 0x27, 0x01};
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &ram_y_start_end[0], 1);
    gpio_put(PIN_DC, 1);
    spi_write_blocking(SPI_PORT, &ram_y_start_end[1], 4);
    gpio_put(PIN_CS, 1);
    sleep_ms(1);


    // border waveform control
    uint8_t border_waveform_control[] = {0x3C, 0xC0};
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &border_waveform_control[0], 1);
    gpio_put(PIN_DC, 1);
    spi_write_blocking(SPI_PORT, &border_waveform_control[1], 1);
    gpio_put(PIN_CS, 1);
    sleep_ms(1);

    return 0;
    //initialisation complete. 
}


int eink_ext_temp(){
    gpio_put(PIN_DC, 0); 
    gpio_put(PIN_CS, 0);
    
    uint8_t ext_ts_control [] = {0x18, 0x80};
    spi_write_blocking(SPI_PORT, &ext_ts_control[0], 1);
    gpio_put(PIN_DC, 1);
    spi_write_blocking(SPI_PORT, &ext_ts_control[1], 1);
    gpio_put(PIN_CS, 1);

    return 0;
}

int eink_int_temp(){
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 0);
    uint8_t int_ts_control [] = {0x18, 0x48};
    spi_write_blocking(SPI_PORT, &int_ts_control[0], 1);
    gpio_put(PIN_DC, 1);
    spi_write_blocking(SPI_PORT, &int_ts_control[1], 1);
    gpio_put(PIN_CS, 1);

    return 0;
}

int load_waveform_lut(){
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 0); 

    uint8_t temp_with_lut_mode1 [] = {0x22, 0xB1}; // MODE 1 is slow/full refresh 
    spi_write_blocking(SPI_PORT, &temp_with_lut_mode1[0], 1);
    gpio_put(PIN_DC, 1);
    spi_write_blocking(SPI_PORT, &temp_with_lut_mode1[1], 1);
    gpio_put(PIN_CS, 1);
    sleep_ms(1);

    // LUT loaded, now 0x20, in the same command

    gpio_put(PIN_DC, 0); 
    gpio_put(PIN_CS, 0);
    uint8_t master_activation = 0x20;
    spi_write_blocking(SPI_PORT, &master_activation, 1);
    gpio_put(PIN_DC, 1);
    gpio_put(PIN_CS, 1);

    while (gpio_get(PIN_BUSY)){
        sleep_ms(1);
    }

    return 0;
    // completed
}

int write_data_and_display(uint8_t imagearray[], size_t array_size){
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_DC, 1);

    uint8_t ram_x_addr_counter [] = {0x4e, 0x00};
    spi_write_blocking(SPI_PORT, &ram_x_addr_counter[0], 1);
    gpio_put(PIN_DC, 1);
    spi_write_blocking(SPI_PORT, &ram_x_addr_counter[1], 1);
    gpio_put(PIN_CS, 1);

    uint8_t ram_y_addr_counter [] = {0x4f, 0x00, 0x00};
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &ram_y_addr_counter[0], 1);
    gpio_put(PIN_DC, 1);
    spi_write_blocking(SPI_PORT, &ram_y_addr_counter[1], 2);
    gpio_put(PIN_CS, 1);

    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 0);
    uint8_t imagearraycommand = 0x24;
    spi_write_blocking(SPI_PORT, &imagearraycommand, 1);
    gpio_put(PIN_DC, 1);
    spi_write_blocking(SPI_PORT, &imagearray[0], array_size);
    gpio_put(PIN_DC, 0);
    uint8_t nop = 0x7f;
    spi_write_blocking(SPI_PORT, &nop, 1);
    gpio_put(PIN_CS, 1);
    gpio_put(PIN_DC, 1);
    //writing data part done, now displaying it. 

    eink_softstart();

    //writing more stuff soon
    

}

int eink_softstart(){
    uint8_t softstart[] = {0x0c, 0x8b, 0x9c, 0x96, 0x0f};

    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &softstart[0], 1);
    gpio_put(PIN_DC, 1);
    spi_write_blocking(SPI_PORT, &softstart[1], 4);
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_CS, 1);

    return 0;
}

