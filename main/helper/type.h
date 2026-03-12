#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include <sys/types.h>

// DMUX
typedef struct
{
    uint8_t pin_ds;
    uint8_t pin_sh;
    uint8_t pin_st;
    uint8_t **value;
    uint8_t len;
} dmux_conf_t;

typedef struct
{
    uint8_t *child;
    uint8_t child_len;
    uint8_t dmux_start;
} dmux_child_t;

// MUX
typedef struct
{
    uint8_t sel0;
    uint8_t sel1;
    uint8_t sel2;
    uint8_t sel3;
    uint8_t out;
} mux_select_t;

typedef struct
{
    uint8_t left_up;
    uint8_t right_up;
    uint8_t left_down;
    uint8_t right_down;
    uint8_t light;
    uint8_t wink;
} mux_popup_t;

typedef struct
{
    uint8_t japan_wing;
    uint8_t detresse;
    uint8_t arriere;
    uint8_t gauche;
    uint8_t droite;
} mux_strip_t;

// TIMING
typedef struct
{
    uint32_t new;
    uint32_t old;
    uint32_t current;
} time_fetch_t;

#endif