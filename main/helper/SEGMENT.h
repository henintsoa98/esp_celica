#ifndef _SEGMENT_H
#define _SEGMENT_H

#include <stdint.h>
#include <inttypes.h>
#include "helper/type.h"
#include "helper/variable.h"
#include "helper/helper.h"

const uint8_t led_pattern[11][8] =
{
    {1, 1, 1, 1, 1, 1, 0, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1, 0}, // 2
    {1, 1, 1, 1, 0, 0, 1, 0}, // 3
    {0, 1, 1, 0, 0, 1, 1, 0}, // 4
    {1, 0, 1, 1, 0, 1, 1, 0}, // 5
    {1, 0, 1, 1, 1, 1, 1, 0}, // 6
    {1, 1, 1, 0, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1, 0}, // 8
    {1, 1, 1, 1, 0, 1, 1, 0}, // 9
    {0, 0, 0, 0, 0, 0, 0 ,0} //10
};

// SEG // RPM // SPD //
//  8  //  4  //  3  //

uint8_t s_rpm_led = 0;
uint8_t s_rpm_ref = 0;

///////////////////////////////////////////////////////////////////////////////
// SHOW ONE DIGIT //
////////////////////
void led_show_digit (uint8_t position, uint32_t digit, uint16_t t_led)
{
    uint8_t pos_by_dmux = _dmux_seg.dmux_start + _dmux_seg.child_len + position; // ctrl pin start
    if (digit != 10)
    {
        //printf ("[%" PRIu16 "]pos = %" PRIu8 "; digit = %" PRIu32 "\n", t_rpm_clock, position, digit);
    }

    if (digit == 10)
    {
        *_dmux.value[pos_by_dmux] = 1;
    }
    else
    {
        *_dmux.value[pos_by_dmux] = 0;
    }

    for (uint8_t i = 0; i < _dmux_seg.child_len; i++)
    {
        _dmux_seg.child[i] = led_pattern[digit][i];
    }
    if (position == 1) // allume le point k sur rpm
    {
        _dmux_seg.child[_dmux_seg.child_len - 1] = 1;
    }
    DMUX_WRITE ();
}

///////////////////////////////////////////////////////////////////////////////
// CALCUL DIGIT = f (T_OUT) //
/////////////////////////////
void led_show_all (uint32_t input, uint16_t *t_led)
{
    uint8_t t_low, t_high;
    uint8_t SEG_NBR = _dmux_rpm_ctrl.child_len + _dmux_spd_ctrl.child_len;
    if (*t_led >= _t_out_max * SEG_NBR) // mise a jour chaque 4 * t_rpm_refresh T_RPM_OUT
    {
        *t_led = 0;
        printf ("[SEGMENT.h : led_show_all] input = %" PRIu32 "\n", input);        
    }
    for (uint32_t position = 1, j = 1; position <= SEG_NBR; position++, j *= 10) // generation du digit pour chaque 7 segment
    {
        uint32_t digit = (input / j) % 10;
        
        t_low = (position - 1) * _t_out_max;
        t_high = position * _t_out_max;

        if (*t_led >= t_low && *t_led < t_high) // minimisation des mise a jour
        {
            if (s_rpm_led != position)  // clear
            {
                s_rpm_led = position;
                uint8_t clear = position - 1;
                if (clear == 0)
                {
                    clear = SEG_NBR;
                }
                led_show_digit (clear, 10, *t_led); // mise a jour anti flash
                s_rpm_ref = 1;
            }
            
            if (s_rpm_ref == 1)         // update
            {
                s_rpm_ref = 0;
                if (input > 0)
                {
                    printf ("[SEGMENT.h : led_show_all] pos[%" PRIu32 "] digit = %" PRIu32 "\n", position, digit);
                }
                led_show_digit (position, digit, *t_led); // mise a jour reel
            }
        }
    }
}
#endif