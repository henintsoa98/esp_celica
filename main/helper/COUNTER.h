#ifndef COUNTER_H
#define COUNTER_H

#include <stdint.h>
//#include <stdio.h>
//#include <inttypes.h>

#include "helper/helper.h"
#include "helper/type.h"
#include "helper/variable.h"
#include "helper/SEGMENT.h"

///////////////////////////////////////////////////////////////////////////////
// CALCULATEUR //
/////////////////
uint32_t fetch (uint16_t *t_in, time_fetch_t time, uint32_t CONST)
{
    uint32_t output;
    if (*t_in >= _t_in_max) // mis a jour RPM chaque t_rpm_refresh T_RPM_IN
    {
        *t_in = 0;

        uint32_t t_diff = time.new - time.old;
        
        if (time.current - time.new > 1000)
        {
            t_diff = time.current - time.new;
        
        }
        if (t_diff == 0)
        {
            output = 0;
        }
        else 
        {
            output = CONST / t_diff;
        }
        return output;
    }
    return UINT32_MAX;
}
/////////
// RPM //
///////////////////////////////////////////////////////////////////////////////
void rpm_fetch (uint16_t *t_rpm, time_fetch_t time)
{
    uint32_t TEMP = fetch (t_rpm, time, _RPM_CONST);
    if (TEMP != UINT32_MAX)
    {
        _RPM = TEMP;
        //printf ("[COUNTER.h : rpm_fetch] RPM = %" PRIu32 "\n", _RPM);
    }
}
/////////
// SPD //
///////////////////////////////////////////////////////////////////////////////
void spd_fetch (uint16_t *t_spd, time_fetch_t time)
{
    uint32_t TEMP = fetch (t_spd, time, _SPD_CONST);
    if (TEMP != UINT32_MAX)
    {
        _SPD = TEMP;
        //printf ("[COUNTER.h : spd_fetch] SPD = %" PRIu32 "\n", _SPD);
    }
}

///////////////////////////////////////////////////////////////////////////////
// CALL LED //
//////////////
void led_show (uint16_t *t_led)     // RPM // SPD //
{
    uint32_t shift_spd = TEN_POWER (_dmux_spd_ctrl.child_len);

    uint32_t rpm_spd_inv = INVERT_IT (_RPM, _dmux_rpm_ctrl.child_len) * shift_spd +
                           INVERT_IT (_SPD, _dmux_spd_ctrl.child_len);
    led_show_all (rpm_spd_inv, t_led);
    //printf ("[COUNTER.h : led_show] RPM_SPD = %" PRIu32 "\n", rpm_spd_inv);

}

#endif