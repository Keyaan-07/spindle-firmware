#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "eink-driver.h"
#include "ff.h"
#include "font8x8_basic.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi1
#define PIN_UP 0
#define PIN_DOWN 0
#define PIN_RIGHT 0
#define PIN_LEFT 0
#define PIN_CENTER 0

#define MAX_FILENAME_LENGTH 32
#define MAX_FILES 16
char file_list[MAX_FILENAME_LENGTH][MAX_FILES];
int total_files = 0;
int selected_file = 0;

FILINFO fno;
FRESULT read;
FATFS fs;
DIR open_dir;

void scan_directory(char* path){
    FRESULT opened_directory = f_opendir(&open_dir, path);
    
    while(1){
        read = f_readdir(&open_dir, &fno);
        if (fno.fname[0]==0){
            break;
        }
        
        if (total_files>=MAX_FILES){
            break;
        }
        if (fno.fattrib && AM_DIR) {
            continue;
        }

        strncpy(file_list[total_files], fno.fdate, MAX_FILENAME_LENGTH);
        total_files++;
    }
    f_closedir(&opened_directory);

}

void draw_menu(){
    eink_clear();
    int y_cursor = 10;
    for (int i = 0; i<=total_files;i++){
        
    }
}



int main()
{

    gpio_init(PIN_UP);
    gpio_init(PIN_DOWN);
    gpio_init(PIN_RIGHT);
    gpio_init(PIN_LEFT);
    gpio_set_dir(PIN_LEFT, GPIO_IN);
    gpio_set_dir(PIN_RIGHT, GPIO_IN);
    gpio_set_dir(PIN_DOWN, GPIO_IN);
    gpio_set_dir(PIN_UP, GPIO_IN);


    stdio_init_all();
    eink_init();
    eink_clear();

    f_mount(&fs, "0:", 1);
    

    eink_write_data_and_display();
    f_closedir(&open_dir);


    while(1){
         sleep_ms(1000);
    }
}
