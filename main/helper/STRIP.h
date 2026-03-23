#ifndef _STRIP_H
#define _STRIP_H

#include <stdint.h>
#include "helper/helper.h"
#include "helper/variable.h"

#define STRIP_MODE_JAPAN_WING       1
#define STRIP_MODE_DETRESSE_ARRIERE 2
#define STRIP_MODE_DETRESSE         3
#define STRIP_MODE_ARRIERE          4
#define STRIP_MODE_LEFT             5
#define STRIP_MODE_RIGHT            6
#define STRIP_MODE_NORMAL           7

uint8_t strip_old_mode = 0;
uint8_t strip_jw_count = 0;
uint8_t strip_jw_flow = 1;
uint16_t t_strip_max = 500;

void strip_init (uint8_t mode)
{
    switch (mode)
    {
        case STRIP_MODE_JAPAN_WING:
            strip_jw_count = 0;
            strip_jw_flow = 1;
            break;
        case STRIP_MODE_DETRESSE_ARRIERE:
            break;
        case STRIP_MODE_DETRESSE:
            break;
        case STRIP_MODE_ARRIERE:
            break;
        case STRIP_MODE_LEFT:
            break;
        case STRIP_MODE_RIGHT:
            break;
        case STRIP_MODE_NORMAL:
            break;
        default:
            break;
    }
}

void strip_check_init (uint8_t mode)
{
    if (strip_old_mode != mode)
    {
        strip_old_mode = mode;
        strip_init (mode);
    }
}

void t_strip_max_by_rpm ()
{
    if (_RPM < 1000)
    {
        t_strip_max = 500;
    }
    else if (_RPM >= 1000 && _RPM < 1500)
    {
        t_strip_max = 400;
    }
    else if (_RPM >= 1500 && _RPM < 2000)
    {
        t_strip_max = 300;
    }
    else if (_RPM >= 2000 && _RPM < 2500)
    {
        t_strip_max = 200;
    }
    else if (_RPM >= 2500)
    {
        t_strip_max = 100;
    }
}

void t_strip_max_japan_wing()
{
    if (strip_jw_flow == 1)
    {
        strip_jw_count++;
    }
    else
    {
        strip_jw_count--;
    }
    if (strip_jw_count == 64)
    {
        strip_jw_flow = 0;
    }
    if (strip_jw_count == 0)
    {
        strip_jw_flow = 1;
    }
    if (strip_jw_count >= 0 && strip_jw_count < 28)
    {
        t_strip_max = 100;
    }
    if (strip_jw_count >= 28 && strip_jw_count < 42)
    {
        t_strip_max = 200;
    }
    if (strip_jw_count >= 42 && strip_jw_count < 51)
    {
        t_strip_max = 300;
    }
    if (strip_jw_count >= 51 && strip_jw_count < 58)
    {
        t_strip_max = 400;
    }
    if (strip_jw_count >= 58)
    {
        t_strip_max = 500;
    }

}

void strip_mode (uint16_t *t_strip)
{
    if (*t_strip >= t_strip_max)
    {
        *t_strip = 0;
        
        uint8_t state_japan    = MUX_READ(_mux_strip.japan_wing);
        uint8_t state_detresse = MUX_READ(_mux_strip.detresse);
        uint8_t state_arriere  = MUX_READ(_mux_strip.arriere);
        uint8_t state_gauche   = MUX_READ(_mux_strip.gauche);
        uint8_t state_droite   = MUX_READ(_mux_strip.droite);

        if (state_japan == 1)                                   // delay fast to low to ...
        {
            strip_check_init(STRIP_MODE_JAPAN_WING);
            t_strip_max_japan_wing();
        }
        else if (state_detresse == 1 && state_arriere == 1)     // low + (fast + fast + ...)
        {
            strip_check_init(STRIP_MODE_DETRESSE_ARRIERE);
        }
        else if (state_detresse == 1)                           // low + (fast + fast + ...)
        {
            strip_check_init(STRIP_MODE_DETRESSE);
        }
        else if (state_arriere == 1)                            // rpm
        {
            strip_check_init(STRIP_MODE_ARRIERE);
            t_strip_max_by_rpm();
        }
        else if (state_gauche == 1)                             // low + (fast + fast + ...)
        {
            strip_check_init(STRIP_MODE_LEFT);
        }
        else if (state_droite == 1)                             // low + (fast + fast + ...)
        {
            strip_check_init(STRIP_MODE_RIGHT);
        }
        else                                                    // rpm
        {
            strip_check_init(STRIP_MODE_NORMAL);
            t_strip_max_by_rpm();
        }
    }
}

#endif