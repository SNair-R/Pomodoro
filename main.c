#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lcd_1602_i2c.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"

#define PICO_I2C_SDA_PIN 14
#define PICO_I2C_SCL_PIN 15
#define READER_PIN 27


int main() {
    
    stdio_init_all();
    sleep_ms(2000);
    adc_init();
    adc_gpio_init(READER_PIN);
    adc_select_input(1);
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(PICO_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_I2C_SDA_PIN);
    gpio_pull_up(PICO_I2C_SCL_PIN);
    gpio_pull_up(BUTTON_PIN)
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_I2C_SDA_PIN, PICO_I2C_SCL_PIN, GPIO_FUNC_I2C));

    lcd_init();

    while(true)
    {
        uint16_t pot_value = adc_read();
    
        char buf[17]; // 16 chars + null terminator
        snprintf(buf, sizeof(buf), "Value: %d", pot_value);
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_string(buf);
        sleep_ms(200);
    }

}
