#ifndef _STRIP_H
#define _STRIP_H

#include <stdint.h>
#include "helper/helper.h"
#include "helper/variable.h"

#define STRIP_MODE_JAPAN_WING       1
#define STRIP_MODE_DETRESSE_ARRIERE 2
#define STRIP_MODE_DETRESSE         3
#define STRIP_MODE_ARRIERE          4
#define STRIP_MODE_GAUCHE           5
#define STRIP_MODE_DROITE           6
#define STRIP_MODE_NORMAL           7

uint8_t strip_old_mode = 0;
uint16_t t_strip_max = 500;

void strip_init (uint8_t mode)
{
    switch (mode)
    {
        case STRIP_MODE_JAPAN_WING:
            break;
        case STRIP_MODE_DETRESSE_ARRIERE:
            break;
        case STRIP_MODE_DETRESSE:
            break;
        case STRIP_MODE_ARRIERE:
            break;
        case STRIP_MODE_GAUCHE:
            break;
        case STRIP_MODE_DROITE:
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

void strip_mode (uint16_t *t_strip)
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

    if (*t_strip >= t_strip_max)
    {
        *t_strip = 0;
        
        uint8_t state_japan    = MUX_READ(_mux_strip.japan_wing);
        uint8_t state_detresse = MUX_READ(_mux_strip.detresse);
        uint8_t state_arriere  = MUX_READ(_mux_strip.arriere);
        uint8_t state_gauche   = MUX_READ(_mux_strip.gauche);
        uint8_t state_droite   = MUX_READ(_mux_strip.droite);

        if (state_japan == 1)
        {
            strip_check_init(STRIP_MODE_JAPAN_WING);
        }
        else if (state_detresse == 1 && state_arriere == 1)
        {
            strip_check_init(STRIP_MODE_DETRESSE_ARRIERE);
        }
        else if (state_detresse == 1)
        {
            strip_check_init(STRIP_MODE_DETRESSE);
        }
        else if (state_arriere == 1)
        {
            strip_check_init(STRIP_MODE_ARRIERE);
        }
        else if (state_gauche == 1)
        {
            strip_check_init(STRIP_MODE_GAUCHE);
        }
        else if (state_droite == 1)
        {
            strip_check_init(STRIP_MODE_DROITE);
        }
        else
        {
            strip_check_init(STRIP_MODE_NORMAL);
        }
    }
}

#endif