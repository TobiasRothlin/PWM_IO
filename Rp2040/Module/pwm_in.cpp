#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"

#include "pwm_in.h"
#include "pio_pwm_in.pio.h"

// class that reads PWM pulses from up to 4 pins
PwmIn::PwmIn(uint *pin_list, uint num_of_pins)
{
    _num_of_pins = num_of_pins;
    // load the pio program into the pio memory
    uint offset = pio_add_program(pio_0, &PwmIn_program);
    // start num_of_pins state machines
    for (uint i = 0; i < 4; i++)
    {
        // prepare state machine i
        pulsewidth[i] = 0;
        period[i] = 0;

        // configure the used pins (pull down, controlled by PIO)
        gpio_pull_down(pin_list[i]);
        pio_gpio_init(pio_0, pin_list[i]);
        // make a sm config
        pio_sm_config c = PwmIn_program_get_default_config(offset);
        // set the 'jmp' pin
        sm_config_set_jmp_pin(&c, pin_list[i]);
        // set the 'wait' pin (uses 'in' pins)
        sm_config_set_in_pins(&c, pin_list[i]);
        // set shift direction
        sm_config_set_in_shift(&c, false, false, 0);
        // init the pio sm with the config
        pio_sm_init(pio_0, i, offset, &c);
        // enable the sm
        pio_sm_set_enabled(pio_0, i, true);
    }
    // set the IRQ handler
    irq_set_exclusive_handler(PIO0_IRQ_0, pio_irq_handler_0);
    // enable the IRQ
    irq_set_enabled(PIO0_IRQ_0, true);
    // allow irqs from the low 4 state machines
    pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS | PIO_IRQ0_INTE_SM2_BITS | PIO_IRQ0_INTE_SM3_BITS ;

    offset = pio_add_program(pio_1, &PwmIn_program);
    // start num_of_pins state machines
    for (uint i = 4; i < 8; i++)
    {
        // prepare state machine i
        pulsewidth[i] = 0;
        period[i] = 0;

        // configure the used pins (pull down, controlled by PIO)
        gpio_pull_down(pin_list[i]);
        pio_gpio_init(pio_1, pin_list[i]);
        // make a sm config
        pio_sm_config c = PwmIn_program_get_default_config(offset);
        // set the 'jmp' pin
        sm_config_set_jmp_pin(&c, pin_list[i]);
        // set the 'wait' pin (uses 'in' pins)
        sm_config_set_in_pins(&c, pin_list[i]);
        // set shift direction
        sm_config_set_in_shift(&c, false, false, 0);
        // init the pio sm with the config
        pio_sm_init(pio_1, i-4, offset, &c);
        // enable the sm
        pio_sm_set_enabled(pio_1, i-4, true);
    }
    // set the IRQ handler
    irq_set_exclusive_handler(PIO1_IRQ_0, pio_irq_handler_1);
    // enable the IRQ
    irq_set_enabled(PIO1_IRQ_0, true);
    // allow irqs from the low 4 state machines
    pio1_hw->inte0 = PIO_IRQ1_INTE_SM0_BITS | PIO_IRQ1_INTE_SM1_BITS | PIO_IRQ1_INTE_SM2_BITS | PIO_IRQ1_INTE_SM3_BITS ;
};

// read the period and pulsewidth
void PwmIn::read_PWM(float *readings, uint pin)
{
    if (pin < _num_of_pins)
    {
        if(period[pin] > 0)
        {
        // determine whole period
        period[pin] += pulsewidth[pin];
        // the measurements are taken with 2 clock cycles per timer tick
        // hence, it is 2*0.000000008
        *(readings + 0) = (float)pulsewidth[pin] * 2 * 0.000000008;
        *(readings + 1) = (float)period[pin] * 2 * 0.000000008;
        *(readings + 2) = ((float)pulsewidth[pin] / (float)period[pin]);
        pulsewidth[pin] = 0;
        period[pin] = 0;
        }
        else{
            *(readings + 0) = -1.0F;
            *(readings + 1) = -1.0F;
            *(readings + 2) = -1.0F;
        }
    }
};

// read only the duty cycle
float PwmIn::read_DC(uint pin)
{
    return ((float)pulsewidth[pin] / (float)period[pin]);
}

// read only the period
float PwmIn::read_P(uint pin)
{
    // the measurements are taken with 2 clock cycles per timer tick
    // hence, it is 2*0.000000008
    return ((float)period[pin] * 0.000000016);
}

float PwmIn::read_PW(uint pin)
{
    // the measurements are taken with 2 clock cycles per timer tick
    // hence, it is 2*0.000000008
    return ((float)pulsewidth[pin] * 0.000000016);
};

uint32_t PwmIn::pulsewidth[8];
uint32_t PwmIn::period[8];
PIO PwmIn::pio_0 = pio0;
PIO PwmIn::pio_1 = pio1;