#include "mcp2515.h"
#include "can_driver.h"
#include "util/delay.h"

#include <avr/interrupt.h>

#define CAN_BRP 0x03


void can_init() {
    mcp2515_init();

    //Setting BRP = 3 and SJW = 4. Th
    mcp2515_write(MCP_CNF1, CAN_BRP);
    
    //Setting BTLMODE to 1 -> the length of PS2 is determined by CNF3. PHSEG1 = 3. PRSEG = 1.
    mcp2515_write(MCP_CNF2, 0b10110001);

    //Setting PHSEG2 = 6.
    mcp2515_write(MCP_CNF3, 0b00000101);

    //Changing the mode of the MCP from config to normal.
    mcp2515_bit_modify(MCP_CANCTRL, MODE_MASK, MODE_NORMAL);

    //Checking if th mode is right. 
    uint8_t value;
    value = mcp2515_read(MCP_CANSTAT);
    if ((value & MODE_MASK) != MODE_NORMAL) {
        printf("MCP2515 is NOT in normal mode after reset!\n");
        return 1;
    }

    //interrupt_enable();
    //masker inn et ett-tall for å sette opp et flagg
    //mcp2515_bit_modify(MCP_CANINTF, 0b00000001, 1);

    

}

void can_send(can_msg_t * msg) {

    //Putting the high bits of the ID into the ID high register.
    mcp2515_write(MCP_TXB0SIDH, 0);
    
    //Putting the low bits of the ID intor the ID low register.
    //Also leftshifting the bits due to the setup of a CAN message. 
    mcp2515_write(MCP_TXB0SIDL, msg->id << CAN_ID_OFFSET);

    //Putting the length of the CAN into the length register.
    mcp2515_write(MCP_TXB0DLC, msg->length);
    
    //Putting the data that are to be transmitted into the buffers of the MCP.
    for(int i = 0; i < msg->length; i++) {
        mcp2515_write(MCP_TXB0D0 + i, msg->data[i]);
    }

    //Requesting the MCP to send the message. 
    mcp2515_rts(0); 
}


void can_receive(can_msg_t *msg) {
    //Extracting the relevant values from the receive registers of the MCP. 
    msg->id = mcp2515_read(MCP_RXB0SIDH) >> 3 +  mcp2515_read(MCP_RXB0SIDL) >> CAN_ID_OFFSET ;
    msg->length = mcp2515_read(MCP_RXB0DLC);
    for (int i = 0; i < msg->length; i++) {
        msg->data[i] = mcp2515_read(MCP_RXB0D0 + i);
    }

    //Flag for interrupt
    mcp2515_bit_modify(MCP_CANINTF, 0b01, 0);
    
}


void print_message(can_msg_t *msg) {
    printf("%d \n", msg->id);
    for(int i = 0; i < msg->length; i++) {
        printf("%c \n", msg->data[i]);
    }
}

void can_IRS_enable(){
    DDRD &= ~(1 << PD2);
    cli();

    GICR |= (1 << INT0);
    MCUCR |= ( 1 << ISC01);
    MCUCR &= ~(1 << ISC00);

    sei();
}


/**
 * @brief Interrupt service routine for the BADISR interrupt vector
 * 
 * @param BADISR_vect BAD interrupt service routine vector.
 */
ISR(BADISR_vect){}
