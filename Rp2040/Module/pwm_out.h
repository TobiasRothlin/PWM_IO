#ifndef PWM_OUT_H
#define PWM_OUT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"

class PwmOut
{
public:
    // Constructor
    PwmOut(uint *pin_list, uint num_of_pin,u_int8_t frequency_mode = 0);

    // Destructor
    ~PwmOut();

    // Member functions
    void setDutyCycle(uint idx,float duty_cycle);
    void setPulseWidth(uint idx,float pulse_width);


private:
    uint pwm_pins [8];
    uint16_t pwmCounter = 25000;
    float pwmFrequency = 50.0;
};

#endif