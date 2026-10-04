# Spindle-firmware

A custom firmware written in C for [Spindle](https://github.com/Ahad732/Spindle)!! It uses the RPi Pico SDK to initialise, and a custom eink driver written in C by me! I have also used the [FatFS library by Carlk3](https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico) for the file management part of this project. It supports only txt files as of now, and i'll try to add epub files soon! 

There is a simple state machine called ui_state, when ui_state = 1, it is in read mode, and when ui_state = 0, it is in the menu, which helps select what to read! There is also a scroll feature, which helps scroll the long text. This firmware can be written much simple, but idk why i wrote it so complex. 

The pin out for the SD is this:  
MISO: 3  
MOSI: 0  
SCK: 2  
CSn: 1  
  
The pin out for the eink is this:  
MISO: 12  
MOSI: 11  
SCK: 10  
CSn: 9  
BUSY: 13  
D/C#: 8  
RSTN: 12  


New additions for firmware include Wi-Fi Compatibility, tho it may be buggy.  ui_state = 2 means it is in download mode where it will download everything from keyaan.spindle.me(the go api backend is in [/book-api](https://github.com/Keyaan-07/spindle-firmware/tree/main/book-api))  

-------------

That is all!!  
Made with <3 by Keyaan  

# Licensing
Licensed under MIT License.  