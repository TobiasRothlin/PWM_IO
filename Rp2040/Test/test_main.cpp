#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "hardware/spi.h"
#include "hardware/uart.h"

#include "PWM_IO.h"

void floatToUint8(float input, u_int8_t *desination)
{
    uint8_t *bytePointer = (uint8_t *)&input;

    for (size_t i = 0; i < sizeof(float); i++)
    {
        desination[i] = *bytePointer;
        bytePointer++;
    }
}


void uint8ArrayToFloat(uint8_t *input, int startIndex, float *output)
{
    uint8_t *bytePointer = (uint8_t *)output;
    for (size_t i = startIndex; i < startIndex + sizeof(float); i++)
    {
        *bytePointer++ = input[i];
    }
}

const int SPI_SLAVE_SCK_PIN = 18;
const int SPI_SLAVE_MOSI_PIN = 19;
const int SPI_SLAVE_MISO_PIN = 16;
const int SPI_SLAVE_CS_PIN = 17;

uint8_t output_buffer[(uint16_t)(sizeof(float) * 8)];
uint8_t input_buffer[(uint16_t)(sizeof(float) * 8)];

uint8_t test_buffer[32] ={1,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0};
float pwm_output_values[8] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8};

float pwm_output_values_decode[8] = {0,0,0,0,0,0,0,0};

int main()
{
    stdio_init_all();

    spi_init(spi0, 1000 * 1000);
    gpio_set_function(SPI_SLAVE_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SLAVE_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SLAVE_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SLAVE_CS_PIN, GPIO_FUNC_SPI);
    
    gpio_init(20);
    gpio_set_dir(20,GPIO_OUT);
    gpio_put(20,true);

    for (int m = 0; m < 100; m++)
    {
        printf("%d\n", m);
        sleep_ms(100);
    }

    int itter = 0;
    while (true)
    {   
        for (int i = 0; i <  8; i++)
        {
            floatToUint8(pwm_output_values[i], output_buffer + (sizeof(float) * i));
            printf("%5.4f,", pwm_output_values[i]);
            pwm_output_values[i] = pwm_output_values[i] + 0.1;
            if (pwm_output_values[i] > 1)
            {
                pwm_output_values[i] = 0;
            }
        }
        printf("\n");
        gpio_put(20,false);
        spi_write_blocking(spi0, output_buffer, sizeof(float) * 8);
        gpio_put(20,true);
        sleep_ms(100);

        itter++;
    }
}
