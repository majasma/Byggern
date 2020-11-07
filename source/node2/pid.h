#ifndef PID_H
#define PID_H
#include <stdint.h>

#define ENCODER_MAX 9000
#define MOTOR_INPUT_MAX 4096
#define SLIDER_MAX 225
#define K_p 0.45
#define K_i 0.10

typedef struct pid_data_t{

    int16_t cur_error;
    int16_t prev_error;

    int16_t integral;
    int16_t derivat;

} PID_DATA;

volatile PID_DATA pid_regulator;

void pid_init();

int16_t slider_to_encoder(int16_t slider_val);


int16_t pid_controller(int16_t reference, int16_t measurment);

#endif