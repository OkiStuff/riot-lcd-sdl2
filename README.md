# riot-lcd-sdl2

Simple SDL2 Implementation for [lcd.h](https://github.com/RIOT-OS/RIOT/blob/master/drivers/include/lcd.h)

## Purpose
I needed a way to test code written using [lcd.h](https://github.com/RIOT-OS/RIOT/blob/master/drivers/include/lcd.h) so I decided to write this simple implementation that allows for testing on modern devices

## Dependencies
- SDL2

## How to use
Simply include the `riot_lcd_sdl2.h` and either define `RIOT_LCD_SDL2_IMPLEMENTATION` before the inclusion or compile `riot_lcd_sdl2.c` alongside your other source files.

The only implemented functions are the high level
 - lcd_init
 - lcd_fill
 - lcd_pixmap
 - lcd_invert_on
 - lcd_invert_off

## How to build
TODO
