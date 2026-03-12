#ifndef POPUP_H
#define POPUP_H

#include <stdint.h>
#include <sys/types.h>

#include "helper/helper.h"
#include "helper/type.h"

#define LEFT  0
#define RIGHT 1

#define WINK_COUNT 3    // nombre monté/descente sur une position

uint8_t s_popup_w_reset = 1;
uint8_t s_popup_w_run   = 0;
uint8_t s_popup_w_init  = 0;
uint8_t s_popup_w_count = 0;
uint8_t s_popup_w_stop  = 0;

///////////////////////////////////////////////////////////////////////////////
// RUN UNTIL TARGET //
//////////////////////
// ONE TARGET //
///////////////////////////////////////////////////////////////////////////////
uint8_t popup_set_to (uint8_t position, uint8_t target)
{
    uint8_t target_state   = MUX_READ (target);
    uint8_t position_state = _dmux_motor.child[position];
    if (target_state == 1)
    {
        if (position_state == 1)
        {
            _dmux_motor.child[position] = 0;
            DMUX_WRITE ();
        }
        return 1;
    }
    else // target_state == 0
    {
        if (position_state == 0)
        {
            _dmux_motor.child[position] = 1;
            DMUX_WRITE ();
        }
        return 0;
    }
}
////////////////
// TWO TARGET //
///////////////////////////////////////////////////////////////////////////////
uint8_t popup_set_two_to (uint8_t target_left, uint8_t target_right)
{
    uint8_t state_left  = popup_set_to (LEFT,  target_left);
    uint8_t state_right = popup_set_to (RIGHT, target_right);
    if (state_left == 1 && state_right == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// ANIMATION WINK //
////////////////////
uint8_t target_1, target_2, target_3, target_4, target_5, target_6;
uint8_t position_first = LEFT;
uint8_t position_last = RIGHT;
void popup_wink (uint8_t WINK_NBR)
{
    if (s_popup_w_reset == 1)
    {
        s_popup_w_reset = 0;
        s_popup_w_init = 1;
        if (position_first == LEFT)
        {
            target_1 = _mux_popup.left_up;
            target_2 = _mux_popup.left_down;
            target_3 = _mux_popup.right_up;
            target_4 = _mux_popup.left_up;      //
            target_5 = _mux_popup.right_down;   //
            target_6 = _mux_popup.right_down;
            position_last = RIGHT;
        }
        else
        {
            target_1 = _mux_popup.right_up;
            target_2 = _mux_popup.left_up;
            target_3 = _mux_popup.right_down;
            target_4 = _mux_popup.left_down;    //
            target_5 = _mux_popup.right_up;     //
            target_6 = _mux_popup.left_down;
            position_last = LEFT;
        }
    }
    if (s_popup_w_init == 1)
    {
        uint8_t state_finish = popup_set_to (position_first, target_1);
        if (state_finish == 1)
        {
            s_popup_w_init  = 0;
            s_popup_w_stop  = 1;
            s_popup_w_count = 0;
        }
    }
    else if (s_popup_w_count <= WINK_NBR)
    {
        if (s_popup_w_count % 2 == 0)
        {
            uint8_t state_finish = popup_set_two_to (target_2, target_3);
            if (state_finish == 1)
            {
                s_popup_w_count++;
            }
        }
        if (s_popup_w_count % 2 == 1)
        {
            uint8_t state_finish = popup_set_two_to (target_4, target_5);
            if (state_finish == 1)
            {
                s_popup_w_count++;
            }
        } 
    } 
    else if (s_popup_w_stop == 1)
    {
        uint8_t state_finish = popup_set_to (position_last, target_6);
        if (state_finish == 1)
        {
            s_popup_w_stop  = 0;
            s_popup_w_reset = 1;
            s_popup_w_run   = 0;
            if (position_first == LEFT)
            {
                position_first = RIGHT;
            }
            else
            {
                position_first = LEFT;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// POPUP MAIN LISTENER //
/////////////////////////
void popup_play ()
{
    uint8_t state_light = MUX_READ (_mux_popup.light);
    uint8_t state_wink  = MUX_READ (_mux_popup.wink);
    
    if (state_light == 1)
    {
        uint8_t state_finish =  popup_set_two_to (_mux_popup.left_up, _mux_popup.right_up);
        if (state_finish == 1)
        {
            s_popup_w_reset = 1;
        }
    }
    else if (state_wink == 1 || s_popup_w_run == 1)
    {
        s_popup_w_run = 1;
        popup_wink (WINK_COUNT *2);

    }
    else 
    {
        popup_set_two_to (_mux_popup.left_down, _mux_popup.right_down);
    }
}

#endif