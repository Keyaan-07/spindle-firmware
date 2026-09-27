#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "eink-driver.h"
#include "ff.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi1


FILINFO fno;
FRESULT read;

int main()
{
    stdio_init_all();
    FATFS fs;

    DIR open_dir;
    TCHAR path = "/";
    FRESULT opened_directory = f_opendir(&open_dir, path);

    
        while(1){
            read = f_readdir(&open_dir, &fno);
        if (read != FR_OK){
            break;
        }
        
        print("File Name: %S\n", fno.fname);
    }

    
}
