#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lcd_1602_i2c.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"


void intro(void)
{
        uint16_t pot_value = adc_read();
        float voltage = pot_value * (3.3 / 4095.0f); // scales it w voltage
        printf("Raw: %d | Voltage: %.2fV | LED: %d\n", pot_value, voltage, voltage > 1.6 ? 1 : 0);

        char buf1[17]; // 16 chars + null terminator
        char buf2[17]; // 16 chars + null terminator

        snprintf(buf1, sizeof(buf1), "Pomodoro Timer", pot_value);
        snprintf(buf2, sizeof(buf2), "Press Button");

        lcd_clear();
        lcd_set_cursor(0, 1);
        lcd_string(buf1);
        lcd_set_cursor(1, 2);
        lcd_string(buf2);
    while(true)
    {
        if (check_input())
        {
            return;
        }
    }
}

int main()
{
    // initialize everything
    stdio_init_all();
    sleep_ms(2000);
    adc_init();
    adc_gpio_init(READER_PIN);
    adc_select_input(2);
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(PICO_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_I2C_SDA_PIN);
    gpio_pull_up(PICO_I2C_SCL_PIN);
    lcd_init(); // lcd after i2c function set
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN); // in lets me just read the V on the pin it becomes passive
    gpio_pull_up(BUTTON_PIN);

    intro();

    char buf[17];
    snprintf(buf, sizeof(buf), "Button Pressed");
    lcd_clear();
    lcd_set_cursor(0,0);
    lcd_string(buf);
}
