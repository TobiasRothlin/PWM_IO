#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "pwm_in.h"
#include "pwm_out.h"

#include "hardware/spi.h"
#include "hardware/uart.h"


/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------
 _    _ __          __   _____                __  _        
 | |  | |\ \        / /  / ____|              / _|(_)       
 | |__| | \ \  /\  / /  | |      ___   _ __  | |_  _   __ _ 
 |  __  |  \ \/  \/ /   | |     / _ \ | '_ \ |  _|| | / _` |
 | |  | |   \  /\  /    | |____| (_) || | | || |  | || (_| |
 |_|  |_|    \/  \/      \_____|\___/ |_| |_||_|  |_| \__, |
                                                       __/ |
                                                      |___/
-----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#define NUMBER_OF_INPUT_PINS 8
#define NUMBER_OF_OUTPUT_PINS 8

uint input_pin_list[NUMBER_OF_INPUT_PINS] = {16, 17, 18, 19, 20, 21, 22, 28};
uint output_pin_list[NUMBER_OF_OUTPUT_PINS] = {2, 3, 4, 5, 6, 7, 8, 9};

const int LOOP_THROUGH_ENABLE_PIN = 14;
const int CONFIG_ENABLE_PIN = 15;

const int OUTPUT_FREQUENCY_SELECTOR_A_PIN = 26;
const int OUTPUT_FREQUENCY_SELECTOR_B_PIN = 27;

const int SPI_SLAVE_SCK_PIN = 10;
const int SPI_SLAVE_MOSI_PIN = 12;
const int SPI_SLAVE_MISO_PIN = 11;
const int SPI_SLAVE_CS_PIN = 13;

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------
  _    _  _    _  _  _  _            ______                    _    _                    
 | |  | || |  (_)| |(_)| |          |  ____|                  | |  (_)                   
 | |  | || |_  _ | | _ | |_  _   _  | |__  _   _  _ __    ___ | |_  _   ___   _ __   ___ 
 | |  | || __|| || || || __|| | | | |  __|| | | || '_ \  / __|| __|| | / _ \ | '_ \ / __|
 | |__| || |_ | || || || |_ | |_| | | |   | |_| || | | || (__ | |_ | || (_) || | | |\__ \
  \____/  \__||_||_||_| \__| \__, | |_|    \__,_||_| |_| \___| \__||_| \___/ |_| |_||___/
                              __/ |                                                      
                             |___/                                                       
-----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void uint8ArrayToFloat(uint8_t *input, int startIndex, float *output)
{
    uint8_t *bytePointer = (uint8_t *)output;
    for (size_t i = startIndex; i < startIndex + sizeof(float); i++)
    {
        *bytePointer++ = input[i];
    }
}


/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------
   _____  _         _             _  __      __           _         _      _            
  / ____|| |       | |           | | \ \    / /          (_)       | |    | |           
 | |  __ | |  ___  | |__    __ _ | |  \ \  / /__ _  _ __  _   __ _ | |__  | |  ___  ___ 
 | | |_ || | / _ \ | '_ \  / _` || |   \ \/ // _` || '__|| | / _` || '_ \ | | / _ \/ __|
 | |__| || || (_) || |_) || (_| || |    \  /| (_| || |   | || (_| || |_) || ||  __/\__ \
  \_____||_| \___/ |_.__/  \__,_||_|     \/  \__,_||_|   |_| \__,_||_.__/ |_| \___||___/                                                                                            
-----------------------------------------------------------------------------------------------------------------------------------------------------------------*/


bool has_new_value = false;

uint8_t inputBuffer[(uint16_t)(sizeof(float) * 8)];

float pwm_output_values[8] = {0, 0, 0, 0, 0, 0, 0, 0};

float loop_through_matrix[8][8] = {
    {1,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,0},
    {0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0},
    {0,0,0,0,0,1,0,0},
    {0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,1}
};



int main()
{
    stdio_init_all();

    gpio_init(1);
    gpio_set_dir(1, GPIO_OUT);

    gpio_init(OUTPUT_FREQUENCY_SELECTOR_A_PIN);
    gpio_set_dir(OUTPUT_FREQUENCY_SELECTOR_A_PIN, GPIO_IN);
    gpio_init(OUTPUT_FREQUENCY_SELECTOR_B_PIN);
    gpio_set_dir(OUTPUT_FREQUENCY_SELECTOR_B_PIN, GPIO_IN);

    gpio_init(LOOP_THROUGH_ENABLE_PIN);
    gpio_set_dir(LOOP_THROUGH_ENABLE_PIN, GPIO_IN);
    gpio_init(CONFIG_ENABLE_PIN);
    gpio_set_dir(CONFIG_ENABLE_PIN, GPIO_IN);

    spi_init(spi1, 50 * 1000);
    spi_set_slave(spi1, true);
    gpio_set_function(SPI_SLAVE_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SLAVE_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SLAVE_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SLAVE_CS_PIN, GPIO_FUNC_SPI);

    bool selector_a = gpio_get(OUTPUT_FREQUENCY_SELECTOR_A_PIN);
    bool selector_b = gpio_get(OUTPUT_FREQUENCY_SELECTOR_B_PIN);

    int frequency_selector_state = 0;

    if (selector_a && !selector_b)
    {
        frequency_selector_state = 1;
    }
    else if (!selector_a && selector_b)
    {
        frequency_selector_state = 2;
    }

    sleep_ms(10);



    PwmIn pwmIn(input_pin_list, NUMBER_OF_INPUT_PINS);
    PwmOut pwmOut(output_pin_list, NUMBER_OF_OUTPUT_PINS, frequency_selector_state);

    while (true)
    {
        gpio_put(1, 1);
        if (spi_is_readable(spi1))
        {
            if (gpio_get(CONFIG_ENABLE_PIN))
            {
                // Config Mode
            }
            else
            {
                 // Drive Output Mode
                spi_read_blocking(spi1, 0, inputBuffer, (uint16_t)(sizeof(float) * 8));
                for (int i = 0; i < 8; i++)
                {
                    uint8ArrayToFloat(inputBuffer, (sizeof(float) * i), &pwm_output_values[i]);
                }
                has_new_value = true;
            }
        }
        else
        {
            has_new_value = false;
        }

        for (int idx = 0; idx < 8; idx++)
        {
            if (gpio_get(LOOP_THROUGH_ENABLE_PIN))
            {
                float new_read_values[3];
                pwmIn.read_PWM(new_read_values, idx);
                if (new_read_values[2] > 0)
                {
                    pwm_output_values[idx] = new_read_values[2];
                    has_new_value = true;
                }
                else
                {
                    has_new_value = false;
                }
            }

            if (has_new_value)
            {
                printf("%.3f \n", pwm_output_values[idx]);
                pwmOut.setPWM(idx, pwm_output_values[idx]);
            }
        }
        
        gpio_put(1, 0);
    }
}
