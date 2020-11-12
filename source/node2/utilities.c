#include "utilities.h"
#include "hardware.h"
#include "msg_handler.h"
#include "pid.h"

#define MOTOR_MAX 0xFFF
#define VAL_MAX 128

volatile uint8_t solenodi_on;

volatile uint16_t ms_gone = 0;

void util_servo_driver() {
    //printf("%d \n\r", util_data.servo_data);
    servo_set_duty_cycle(util_data.servo_data);
}

void util_solenoid_driver() {
    //printf("%d \n\r", util_data.solenoid);
    if((util_data.solenoid != 0) && (solenodi_on == 0)){
        solenodi_on = 1;
        soleniod_init();
        solenoid_on();
        timer_systick_wait(500);
        solenoid_off();
        util_data.solenoid = 0;
    }
    else if(util_data.solenoid == 0)
    {
        solenodi_on = 0;
    }
    
}

uint16_t util_encoder_read() {
    uint16_t data;
    uint8_t high_byte;
    uint8_t low_byte;
    

    encoder_set_oe(LOW);
    encoder_set_sel(LOW);

    timer_systick_wait(200);

    high_byte = encoder_get_data();

    encoder_set_sel(HIGH);

    timer_systick_wait(200);

    low_byte = encoder_get_data();

    encoder_tgl_rst();
    encoder_set_oe(HIGH);

    data = ((high_byte << 8) | low_byte);
    if (data & (1 << 15)) {
        return ((uint16_t) (~data) + 1);
    }
    return data;
}



void util_motor_driver(PID_DATA * regulator) {

    int16_t reference = slider_to_encoder(util_data.motor_data);

    int16_t measurement = util_encoder_read();

    int16_t u = pid_controller(regulator, reference, measurement, ms_gone);

    if ((regulator -> cur_error) > 0) {
        motor_direction(RIGHT);
    }
    else
    {
        motor_direction(LEFT);
    }
    
     
    //u = (u/0xFFFF)*0xFFF;

    //printf("%d %d %d \n\r", reference, measurement, u );


    //dac_send(u);

    ms_gone = 0;
}