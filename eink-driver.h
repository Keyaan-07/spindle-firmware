#ifndef EINK_DRIVER_H
#define EINK_DRIVER_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdint.h>

#define PIN_MISO 1
#define PIN_MOSI 11
#define PIN_CS 9
#define PIN_SCK 10
#define PIN_DC 8
#define PIN_RSTN 12
#define PIN_BUSY 13 

int eink_init();
int eink_ext_temp();
int eink_int_temp();
int load_waveform_lut();
int eink_write_data_and_display(uint8_t imagearray[], size_t array_size);
int eink_softstart();
int eink_sleep();

#endif