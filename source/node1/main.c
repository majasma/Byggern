#define F_CPU 4915200
#define FOSC 4915200
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

#include "avr/io.h"
#include "stdio.h"
#include <stdlib.h>
#include <avr/interrupt.h>

#include "uart_driver.h"
#include "sram.h"
#include "menu.h"
#include "can_driver.h"


int main(){
    slider_t s;
    joystick_t j;
    SRAM_init();
    adc_init();
    string_init(MYUBRR);
    can_init();
    can_msg_t receive;
    can_msg_t msg;
    msg.id = 3;
    while(1){
        multifunc_joy_get(&j);
        multifunc_joy_get_dir(&j);
        msg.data[0] = j.dir_x;
        msg.data[1] = j.dir_y;
        msg.data[2] = j.val_x;
        msg.data[3] = j.val_y;
        msg.length = 4;
       can_send(&msg);
//    msg.id = 3;
//    msg.data[0] = 'a';
//    msg.data[1] = 'b';
//    msg.data[2] = 'c';
//    msg.length = 3;
//    while(1) {
//    can_send(&msg);
    }
}

//Putte inn riktig interrupt-vektor i denne funksjonen her.
ISR() {
    can_receive(&msg);
}