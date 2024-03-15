#include "pwm_out.h"

#include "hardware/pwm.h"

// Constructor definition
PwmOut::PwmOut(uint *pin_list, uint num_of_pin,u_int8_t frequency_mode)
{
    pwm_config c = pwm_get_default_config();
    pwm_config_set_clkdiv(&c, 100.0);

    printf("Init PWM Frequency at ");
    if( frequency_mode == 1)
    {
        pwmCounter = 10000;
        pwmFrequency = 125.0;
        printf("125Hz\n");
        
    }
    else if (frequency_mode == 2)
    {
        pwmCounter = 5000;
        pwmFrequency = 250.0;
        printf("250Hz\n");
    }
    else
    {
        pwmCounter = 25000;
        pwmFrequency = 50.0;
        printf("50Hz\n");
    }
    

    for (uint i = 0; i < num_of_pin; i++)
    {
        pwm_pins[i] = pin_list[i];
        gpio_set_function(pwm_pins[i], GPIO_FUNC_PWM);
        uint slice_num = pwm_gpio_to_slice_num(pwm_pins[i]);
        uint chan_num = pwm_gpio_to_channel(pwm_pins[i]);

        printf("Setting up idx:%d,pin:%d,slice:%d,channel:%d \n",i,pwm_pins[i],slice_num,chan_num);

        pwm_init(slice_num, &c, true);
        pwm_set_wrap(slice_num, pwmCounter);
        pwm_set_enabled(slice_num, true);
        pwm_set_chan_level(slice_num,PWM_CHAN_A,(uint16_t)(0));
        pwm_set_chan_level(slice_num,PWM_CHAN_B,(uint16_t)(0));

    }

    
}

// Destructor definition
PwmOut::~PwmOut()
{
    // Perform cleanup if necessary
}

void PwmOut::setDutyCycle(uint idx,float duty_cycle)
{
    pwm_set_gpio_level(pwm_pins[idx],(uint16_t)(pwmCounter * duty_cycle));
}

void PwmOut::setPulseWidth(uint idx,float pulse_width)
{
    float duty_cycle = pulse_width / (1.0/pwmFrequency);
    setDutyCycle(idx,duty_cycle);
}
