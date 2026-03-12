#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>
#include "driver/gpio.h"

#include "helper/type.h"
#include "helper/variable.h"

///////////////////////////////////////////////////////////////////////////////
// PUISSANCE 10 //
//////////////////
uint32_t TEN_POWER (uint8_t power)
{
    uint32_t result = 1;
    for (uint8_t i = 0; i < power; i++)
    {
        result *= 10; 
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// INVERTING POSTION //
///////////////////////
uint32_t INVERT_IT (uint32_t value, uint8_t len)
{
    uint32_t output = 0;

    uint32_t power = TEN_POWER (len - 1);

    for (uint8_t i = 1, j = 1; i <= len; i++, j *= 10)
    {
        uint32_t digit = (value / j) % 10;
        output = digit * power + output;
        power /= 10; 
    }

    return output;
}

///////////////////////////////////////////////////////////////////////////////
// RISING PIN //
////////////////
void RISE_IT (uint8_t pin)
{
    gpio_set_level (pin, 1);
    gpio_set_level (pin, 0);
}

///////////////////////////////////////////////////////////////////////////////
// DMUX ADD CHILD TO PARENT //
//////////////////////////////
dmux_child_t DMUX_ADD (uint8_t **DMUX, uint8_t *DMUX_CHILD, uint8_t CHILD_LEN, uint8_t *DMUX_COUNT)
{
    dmux_child_t output;
    for (uint8_t i = 0; i < CHILD_LEN; i++)
    {
        DMUX[*DMUX_COUNT] = &DMUX_CHILD[i];
        DMUX_CHILD[i] = 0;
        (*DMUX_COUNT)++;
    }
    output.child      = DMUX_CHILD;
    output.child_len  = CHILD_LEN;
    output.dmux_start = *DMUX_COUNT - CHILD_LEN;
    return output;
}

///////////////////////////////////////////////////////////////////////////////
// MUX READ PIN //
//////////////////
uint8_t MUX_READ (uint8_t PIN)
{
    gpio_set_level (_mux_select.sel0,  (PIN >> 0) & 1);
    gpio_set_level (_mux_select.sel1,  (PIN >> 1) & 1);
    gpio_set_level (_mux_select.sel2,  (PIN >> 2) & 1);
    gpio_set_level (_mux_select.sel3,  (PIN >> 3) & 1);
    return gpio_get_level (_mux_select.out);
}

///////////////////////////////////////////////////////////////////////////////
// DMUX UPDATE OUTPUT //
////////////////////////
void DMUX_WRITE ()
{
    for (int8_t i = _dmux.len - 1; i >= 0; i--)
    {
        gpio_set_level (_dmux.pin_ds, *_dmux.value[i]);
        RISE_IT (_dmux.pin_sh);
    }
    RISE_IT (_dmux.pin_st);
}

#endif