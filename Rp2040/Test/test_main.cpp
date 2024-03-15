#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "hardware/spi.h"
#include "hardware/uart.h"
#include "pico/binary_info.h"

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

const uint32_t SPI_SLAVE_SCK_PIN = 18;
const uint32_t SPI_SLAVE_MOSI_PIN = 19;
const uint32_t SPI_SLAVE_MISO_PIN = 16;
const uint32_t SPI_SLAVE_CS_PIN = 17;

uint8_t output_buffer[(uint16_t)(sizeof(float) * 8)+2];
uint8_t input_buffer[(uint16_t)(sizeof(float) * 8)+2];

uint8_t test_buffer[32] ={0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0};
float pwm_output_values[8] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8};

float pwm_input_values[8] = {0,0,0,0,0,0,0,0};

int main()
{
    stdio_init_all();

    spi_init(spi0, 1000 * 1000);
    gpio_set_function(SPI_SLAVE_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SLAVE_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SLAVE_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SLAVE_CS_PIN, GPIO_FUNC_SPI);

    bi_decl(bi_4pins_with_func(SPI_SLAVE_MISO_PIN, SPI_SLAVE_MOSI_PIN, SPI_SLAVE_SCK_PIN, SPI_SLAVE_CS_PIN, GPIO_FUNC_SPI));

    
    gpio_init(20);
    gpio_set_dir(20,GPIO_OUT);
    gpio_put(20,false);

    for (int m = 0; m < 100; m++)
    {
        printf("%d\n", m);
        sleep_ms(100);
    }

    while (true)
    {   
        for (int i = 0; i <  8; i++)
        {
            floatToUint8(pwm_output_values[i], output_buffer + (sizeof(float) * i));
            pwm_output_values[i] = pwm_output_values[i] + 0.1;
            if (pwm_output_values[i] > 1)
            {
                pwm_output_values[i] = 0;
            }
        }
        gpio_put(20,true);
        spi_write_read_blocking(spi0, output_buffer, input_buffer, (sizeof(float) * 8)+2);
        gpio_put(20,false);
        for (int i = 0; i < 8; i++)
        {
            uint8ArrayToFloat(input_buffer, (i * sizeof(float))+2, &pwm_input_values[i]);
            printf("%5.4f,", pwm_input_values[i]);  
            //printf("%d,", input_buffer[i]);
        }
        printf("\n");
        sleep_ms(10);

    }
}
