/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pomodoro.h"
#include "hardware/pwm.h"

bool button_press = false;
bool press = false;

/* Example code to drive a 16x2 LCD panel via a I2C bridge chip (e.g. PCF8574)

   NOTE: The panel must be capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefore I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO 4 (pin 6)-> SDA on LCD bridge board
   GPIO 5 (pin 7)-> SCL on LCD bridge board
   3.3v (pin 36) -> VCC on LCD bridge board
   GND (pin 38)  -> GND on LCD bridge board
*/

/* Quick helper function for single byte transfers */
void i2c_write_byte(uint8_t val)
{
    i2c_write_blocking(i2c1, addr, &val, 1, false);
}

void lcd_toggle_enable(uint8_t val)
{
    // Toggle enable pin on LCD display
    // We cannot do this too quickly or things don't work
#define DELAY_US 600
    sleep_us(DELAY_US);
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
    i2c_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
}

// The display is sent a byte as two separate nibble transfers
void lcd_send_byte(uint8_t val, int mode)
{
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(high);
    lcd_toggle_enable(high);
    i2c_write_byte(low);
    lcd_toggle_enable(low);
}

void lcd_clear(void)
{
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

// go to location on LCD
void lcd_set_cursor(int line, int position)
{
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, LCD_COMMAND);
}

static inline void lcd_char(char val)
{
    lcd_send_byte(val, LCD_CHARACTER);
}

void lcd_string(const char *s)
{
    while (*s)
    {
        lcd_char(*s++);
    }
}

void lcd_init()
{
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear();
}

int check_input()
{

    press = !gpio_get(BUTTON_PIN);
    if (press && !button_press)
    {
        button_press = press;
        return 1;
    }
    button_press = press;
    return 0;
}
void buzz(uint32_t wrap, uint32_t lvl, uint32_t length)
{
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(BUZZER_PIN), lvl);
    pwm_set_enabled(slice_num, true);
    sleep_ms(length);
    pwm_set_enabled(slice_num, false);
}

void time_name(int mode)
{
    char time_n[17];
    switch(mode)
    {
        case 1:
            snprintf(time_n, sizeof(time_n), "Study Time");
            break;
        case 2:
            snprintf(time_n, sizeof(time_n), "Chill Time");
            break;
        case 3:
            snprintf(time_n, sizeof(time_n), "Relax Time");
            break;
    }

    lcd_clear();
    lcd_set_cursor(0, 3);
    lcd_string(time_n);
}

void timer(int tot_sec, int mode)
{
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    uint32_t base_time = to_ms_since_boot(get_absolute_time());

    time_name(mode);

    char buf5[17];
    int min_left = tot_sec / 60;
    int sec_left = tot_sec % 60;
    snprintf(buf5, sizeof(buf5), "%2d:%02d", min_left, sec_left);
    lcd_set_cursor(1, 6);
    lcd_string(buf5);
    while (tot_sec > 0)
    {
        current_time = to_ms_since_boot(get_absolute_time());

        if ((current_time - base_time) >= 1000)
        {
            tot_sec -= 1;

            if (sec_left == 0)
            {
                sec_left = 59;
                min_left -= 1;
            }
            else
            {
                sec_left -= 1;
            }
            snprintf(buf5, sizeof(buf5), "%2d:%02d", min_left, sec_left);
            lcd_set_cursor(1, 6);
            lcd_string(buf5);

            base_time += 1000;
        }
    }
}