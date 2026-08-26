#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pomodoro.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

void intro(void)
{
    uint16_t pot_value = adc_read();
    float voltage = pot_value * (3.3 / 4095.0f); // scales it w voltage
    printf("Raw: %d | Voltage: %.2fV | LED: %d\n", pot_value, voltage, voltage > 1.6 ? 1 : 0);

    char buf1[17]; // 16 chars + null terminator
    char buf2[17]; // 16 chars + null terminator

    snprintf(buf1, sizeof(buf1), "Pomodoro Timer");
    snprintf(buf2, sizeof(buf2), "Press Button");

    lcd_clear();
    lcd_set_cursor(0, 1);
    lcd_string(buf1);
    lcd_set_cursor(1, 2);
    lcd_string(buf2);
    while (!check_input())
    {

    }
    return;
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
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);

    while (true)
    {
        intro();

        char buf[17];
        snprintf(buf, sizeof(buf), "Set Study Time");
        lcd_clear();
        lcd_set_cursor(0, 1);
        lcd_string(buf);

        uint16_t pot_value;
        int step_index;
        float set_time;
        int s_min;
        int s_sec;
        int c_min;
        int c_sec;
        int l_min;
        int l_sec;

        while (!check_input())
        {
            pot_value = adc_read();
            step_index = (pot_value * 119) / 4096;
            if (step_index > 118)
            {
                step_index = 118;
            }

            printf("raw=%d step=%d\n", pot_value, step_index);

            set_time = 1.0f + (step_index * .5f);
            s_min = (int)set_time;
            s_sec = (set_time - s_min > .1) ? 30 : 0; // condition ? if true : if false

            char buf2[17];
            snprintf(buf2, sizeof(buf2), "%2d:%02d", s_min, s_sec);
            lcd_set_cursor(1, 6);
            lcd_string(buf2);

            sleep_ms(100);
        }

        char buf3[17];
        snprintf(buf3, sizeof(buf3), "Set Chill Time");
        lcd_clear();
        lcd_set_cursor(0, 1);
        lcd_string(buf3);

        while (!check_input())
        {
            pot_value = adc_read();
            step_index = (pot_value * 59) / 4096;
            if (step_index > 58)
            {
                step_index = 58;
            }

            printf("raw=%d step=%d\n", pot_value, step_index);

            set_time = 1.0f + (step_index * .5f);
            c_min = (int)set_time;
            c_sec = (set_time - c_min > .1f) ? 30 : 0; // condition ? if true : if false

            char buf4[17];
            snprintf(buf4, sizeof(buf4), "%2d:%02d", c_min, c_sec);
            lcd_set_cursor(1, 6);
            lcd_string(buf4);

            sleep_ms(100);
        }

        char buf7[17];
        snprintf(buf7, sizeof(buf7), "Set Relax Time");
        lcd_clear();
        lcd_set_cursor(0, 1);
        lcd_string(buf7);

        while (!check_input())
        {
            pot_value = adc_read();
            step_index = (pot_value * 119) / 4096;
            if (step_index > 118)
            {
                step_index = 118;
            }

            printf("raw=%d step=%d\n", pot_value, step_index);

            set_time = 1.0f + (step_index * .5f);
            l_min = (int)set_time;
            l_sec = (set_time - l_min > .1f) ? 30 : 0; // condition ? if true : if false

            char buf4[17];
            snprintf(buf4, sizeof(buf4), "%2d:%02d", l_min, l_sec);
            lcd_set_cursor(1, 6);
            lcd_string(buf4);

            sleep_ms(100);
        }

        int s_tot_sec = (s_min * 60) + s_sec;
        int c_tot_sec = (c_min * 60) + c_sec;
        int l_tot_sec = (l_min * 60) + l_sec;

        for (int i = 0; i < 4; i++)
        {
            // study
            timer(s_tot_sec, 1);
            buzz(40000, 35000, 200);
            sleep_ms(150);
            buzz(40000, 35000, 200);

            if (i < 3)
            {
                timer(c_tot_sec, 2);
                buzz(50000, 45500, 500);
            }
            else
            {
                timer(l_tot_sec, 3);
                buzz(30000, 22500, 200);
                sleep_ms(150);
                buzz(30000, 22500, 200);
                sleep_ms(150);
                buzz(30000, 22500, 200);
            }
        }

        char buf5[17];
        char buf6[17];
        snprintf(buf5, sizeof(buf5), "Press Button");
        snprintf(buf6, sizeof(buf6), "To Reset");
        lcd_clear();
        lcd_set_cursor(0, 2);
        lcd_string(buf5);
        lcd_set_cursor(1, 4);
        lcd_string(buf6);

        while (!check_input())
        {

        }
    }
}