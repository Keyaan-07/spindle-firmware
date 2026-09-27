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
#define MAX_PAGES 500

#define MAX_FILENAME_LENGTH 32
#define MAX_FILES 16
char file_list[MAX_FILENAME_LENGTH][MAX_FILES];
int total_files = 0;
int selected_file = 0;
int ui_state = 0;
int current_off = 0;
int next_off = 0;

UINT page_history[MAX_PAGES];
int current_page = 0;


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
        if (fno.fattrib & AM_DIR) {
            continue;
        }

        strncpy(file_list[total_files], fno.fname, MAX_FILENAME_LENGTH);
        total_files++;
    }
    f_closedir(&opened_directory);

}

void draw_menu(){
    eink_clear();
    int y_cursor = 10;
    for (int i = 0; i<total_files;i++){
        if (selected_file == i){
            eink_draw_char(2, y_cursor, '>', 1);
        }
        eink_write_string(16, y_cursor, file_list[i], 1);
        y_cursor += 12;

    }
    eink_write_data_and_display();
}

void read_book(char* filename){
    FIL file;
    FRESULT res;
    UINT bytes_read;
    char char_buf;

    eink_clear();

    res = f_open(&file, filename, FA_READ);
    f_lseek(&file, current_off);

    int y_cursor = 10;
    int x_cursor = 2;
    UINT total_bytes_tis_page = 0;


    // 176 is the width of the screen
    while (y_cursor<176){
        f_read(&file, &char_buf, 1, &bytes_read);
        if (bytes_read == 0) break;
        total_bytes_tis_page++;

        if (char_buf == '\n'){
            x_cursor = 2;
            y_cursor += 12;
            continue;
        }
        eink_draw_char(x_cursor, y_cursor, char_buf, 1);
        x_cursor += 8;

        if (x_cursor>256){
            x_cursor = 2;
            y_cursor += 12;
        }
    }


    next_off = current_off + total_bytes_tis_page;
    eink_write_data_and_display();
    f_close(&file);
}



int main()
{

    gpio_init(PIN_UP);
    gpio_init(PIN_DOWN);
    gpio_init(PIN_RIGHT);
    gpio_init(PIN_LEFT);
    gpio_init(PIN_CENTER);
    gpio_set_dir(PIN_LEFT, GPIO_IN);
    gpio_set_dir(PIN_RIGHT, GPIO_IN);
    gpio_set_dir(PIN_DOWN, GPIO_IN);
    gpio_set_dir(PIN_UP, GPIO_IN);
    gpio_set_dir(PIN_CENTER, GPIO_IN);


    stdio_init_all();
    eink_init();
    eink_clear();
    eink_write_data_and_display();

    f_mount(&fs, "0:", 1);
    scan_directory("0:");

    draw_menu();

    while(1){
        if(ui_state == 0){
            if(gpio_get(PIN_CENTER)==0){
                current_off = 0;
                page_history[0] = 0;
                current_page = 0;
                ui_state =1;

                read_book(file_list[selected_file]);
                sleep_ms(200);
            
            }

            if(gpio_get(PIN_DOWN) == 0){
                if (selected_file < (total_files-1)){
                    selected_file++;
                    draw_menu();
                }
                sleep_ms(100);
            }
            if (gpio_get(PIN_UP) == 0){
                if(selected_file>0){
                    selected_file--;
                    draw_menu();
                }
                sleep_ms(100);
            }
        }
        else if (ui_state == 1){
            if(gpio_get(PIN_LEFT)==0){
                ui_state = 0;
                draw_menu();
                sleep_ms(100);
            }

            if(gpio_get(PIN_DOWN)==0){

                if (current_page < (MAX_PAGES - 1)){
                    current_page++;
                    page_history[current_page] = next_off;
                    current_off = next_off;
                    read_book(file_list[selected_file]);
                }
                sleep_ms(100);
            }

            if(gpio_get(PIN_UP) == 0){

                if (current_page>0){
                    current_page--;
                    current_off = page_history[current_page];
                    read_book(file_list[selected_file]);
                }
                sleep_ms(100);
            }
        }
        sleep_ms(10);
    }
}
