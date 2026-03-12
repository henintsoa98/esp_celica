#include <stdio.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include "freertos/idf_additions.h"
#include "hal/gpio_types.h"
#include "portmacro.h"

#include "helper/type.h"
#include "helper/COUNTER.h"
#include "helper/POPUP.h"
#include "helper/STRIP.h"

////////////
// PINOUT //
////////////
#define RPM_IN 4
#define SPD_IN 8

#define DMUX_SH 5
#define DMUX_ST 6
#define DMUX_DS 7

#define MUX_SEL0 15
#define MUX_SEL1 16
#define MUX_SEL2 17
#define MUX_SEL3 18
#define MUX_OUT 8

// MUX PIN
#define MUX_PIN_POPUP_LU 0
#define MUX_PIN_POPUP_RU 1
#define MUX_PIN_POPUP_LD 2
#define MUX_PIN_POPUP_RD 3
#define MUX_PIN_POPUP_LIGHT 4
#define MUX_PIN_POPUP_WINK  5

#define MUX_PIN_STRIP_JAPAN    6
#define MUX_PIN_STRIP_DETRESSE 7
#define MUX_PIN_STRIP_ARRIERE  8
#define MUX_PIN_STRIP_GAUCHE   9
#define MUX_PIN_STRIP_DROITE   10


// VAR
#define DMUX_NBR 10
#define DMUX_PIN DMUX_NBR * 8

#define SEG_PIN 8   // FIXE
#define SEG_RPM 3
#define SEG_SPD 3

#define MOTOR_NBR 2 // FIXE

#define STRIP_NBR 64

#define T_REF_IN  100   // RPM refresh speed

#define T_REF_OUT 5     // LED RPM refresh speed

#define WHEEL_P1 175
#define WHEEL_P2 65
#define WHEEL_P3 16

#define RPE 2 // rotation par explosion

// timer
uint32_t t_ms  = 0;
uint16_t t_rpm_in = 0;
uint16_t t_spd_in = 0;
uint16_t t_led = 0;
uint16_t t_strip = 0;

uint32_t t_rpm_old = 0;
uint32_t t_rpm_new = 0;

uint32_t t_spd_old = 0;
uint32_t t_spd_new = 0;

// INTERRUPTION COMPTEUR
static void IRAM_ATTR gpio_rpm_handler (void *arg)
{
    t_rpm_old = t_rpm_new;
    t_rpm_new = t_ms;
}

static void IRAM_ATTR gpio_spd_handler (void *arg)
{
    t_spd_old = t_spd_new;
    t_spd_new = t_ms;
}

// TIMER 1MS
void timer_callback (void *param)
{
    t_ms++;
    t_rpm_in++;
    t_spd_in++;
    t_led++;
    t_strip++;
}

void app_main (void)
{
    ///////////
    // SETUP //
    ///////////

    // interruption pour compteur de fréquence
    gpio_reset_pin (RPM_IN);
    gpio_set_direction (RPM_IN, GPIO_MODE_INPUT);
    gpio_set_pull_mode (RPM_IN, GPIO_PULLUP_ONLY);
    gpio_set_intr_type (RPM_IN, GPIO_INTR_POSEDGE);
    gpio_install_isr_service (0);
    gpio_isr_handler_add (RPM_IN, gpio_rpm_handler, NULL);
    gpio_intr_enable (RPM_IN);

    // interruption pour compteur de fréquence
    gpio_reset_pin (SPD_IN);
    gpio_set_direction (SPD_IN, GPIO_MODE_INPUT);
    gpio_set_pull_mode (SPD_IN, GPIO_PULLUP_ONLY);
    gpio_set_intr_type (SPD_IN, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add (SPD_IN, gpio_spd_handler, NULL);
    gpio_intr_enable (SPD_IN);
    
    // timer pour gestion des temps 
    const esp_timer_create_args_t my_timer_args = {
        .callback = &timer_callback,
        .name = "Timer interrupt"
    };
    esp_timer_handle_t timer_handler;
    esp_timer_create (&my_timer_args, &timer_handler);
    esp_timer_start_periodic (timer_handler, 1000); // microsecond

    // GPIO pour 74DMUX595 : OUT
    gpio_reset_pin (DMUX_DS);
    gpio_reset_pin (DMUX_SH);
    gpio_reset_pin (DMUX_ST);
    gpio_set_direction (DMUX_DS, GPIO_MODE_OUTPUT);
    gpio_set_direction (DMUX_SH, GPIO_MODE_OUTPUT);
    gpio_set_direction (DMUX_ST, GPIO_MODE_OUTPUT);
    gpio_set_level (DMUX_DS, 0);
    gpio_set_level (DMUX_SH, 0);
    gpio_set_level (DMUX_ST, 0);

    // GPIO pour CD4067 : out + IN
    gpio_reset_pin (MUX_SEL0);
    gpio_reset_pin (MUX_SEL1);
    gpio_reset_pin (MUX_SEL2);
    gpio_reset_pin (MUX_SEL3);
    gpio_reset_pin (MUX_OUT);
    gpio_set_direction (MUX_SEL0, GPIO_MODE_OUTPUT);
    gpio_set_direction (MUX_SEL1, GPIO_MODE_OUTPUT);
    gpio_set_direction (MUX_SEL2, GPIO_MODE_OUTPUT);
    gpio_set_direction (MUX_SEL3, GPIO_MODE_OUTPUT);
    gpio_set_direction (MUX_OUT,  GPIO_MODE_INPUT);
    gpio_set_level (MUX_SEL0, 0);
    gpio_set_level (MUX_SEL1, 0);
    gpio_set_level (MUX_SEL2, 0);
    gpio_set_level (MUX_SEL3, 0);
    

    // 74DMUX595 PINOUT
    uint8_t *DMUX[DMUX_PIN];
    uint8_t DMUX_COUNT = 0;

    // DMUX 74HC595
    dmux_conf_t dmux = {
        .pin_ds  = DMUX_DS,
        .pin_sh  = DMUX_SH,
        .pin_st  = DMUX_ST,
        .value   = DMUX,
        .len     = DMUX_COUNT
    };

    uint8_t DMUX_SEG[SEG_PIN];
    uint8_t DMUX_SEG_RPM[SEG_RPM];
    uint8_t DMUX_SEG_SPD[SEG_SPD];
    uint8_t DMUX_MOTOR[MOTOR_NBR];
    uint8_t DMUX_STRIP[STRIP_NBR];
    if (SEG_PIN + SEG_RPM + SEG_SPD + MOTOR_NBR + STRIP_NBR > DMUX_PIN)
    {
        while (true)
        {
            printf ("Error : Nombre de pin incompatible sur 74HC595");
            vTaskDelay (1000 / portTICK_PERIOD_MS);
        }
    }
    
    dmux_child_t dmux_seg      = DMUX_ADD (DMUX, DMUX_SEG,     SEG_PIN,   &DMUX_COUNT); // 1er bloc du 74DMUX595 : 7 segment + point
    dmux_child_t dmux_rpm_ctrl = DMUX_ADD (DMUX, DMUX_SEG_RPM, SEG_RPM,   &DMUX_COUNT); // 2eme bloc du 74DMUX595 : cathode
    dmux_child_t dmux_spd_ctrl = DMUX_ADD (DMUX, DMUX_SEG_SPD, SEG_SPD,   &DMUX_COUNT); // 2eme bloc du 74DMUX595 : cathode
    dmux_child_t dmux_motor    = DMUX_ADD (DMUX, DMUX_MOTOR,   MOTOR_NBR, &DMUX_COUNT);
    dmux_child_t dmux_strip    = DMUX_ADD (DMUX, DMUX_STRIP,   STRIP_NBR, &DMUX_COUNT);
    dmux.len = DMUX_COUNT;

    // MUX 4067
    mux_select_t mux_select = {
        .sel0 = MUX_SEL0,
        .sel1 = MUX_SEL1,
        .sel2 = MUX_SEL2,
        .sel3 = MUX_SEL3,
        .out  = MUX_OUT
    };

    mux_popup_t mux_popup = {
        .left_up    = MUX_PIN_POPUP_LU,
        .right_up   = MUX_PIN_POPUP_RU,
        .left_down  = MUX_PIN_POPUP_LD,
        .right_down = MUX_PIN_POPUP_RD,
        .light      = MUX_PIN_POPUP_LIGHT,
        .wink       = MUX_PIN_POPUP_WINK
    };

    mux_strip_t mux_strip = {
        .japan_wing = MUX_PIN_STRIP_JAPAN,
        .detresse   = MUX_PIN_STRIP_DETRESSE,
        .arriere    = MUX_PIN_STRIP_ARRIERE,
        .gauche     = MUX_PIN_STRIP_GAUCHE,
        .droite     = MUX_PIN_STRIP_DROITE
    };

    input_setup (T_REF_IN);
    output_setup ( T_REF_OUT);

    dmux_setup (dmux);
    
    dmux_led_seg_setup (dmux_seg);
    dmux_led_rpm_setup (dmux_rpm_ctrl);
    dmux_led_spd_setup (dmux_spd_ctrl);
    dmux_motor_setup   (dmux_motor);
    dmux_strip_setup   (dmux_strip);

    rpm_setup (RPE);  // counter
    spd_setup (WHEEL_P1, WHEEL_P2, WHEEL_P3);  // counter
    
    mux_setup (mux_select);
    popup_setup (mux_popup);
    strip_setup (mux_strip);

    //////////
    // LOOP //
    //////////
    time_fetch_t time;
    while (true)
    {
        time.current = t_ms;

        time.new = t_rpm_new;
        time.old = t_rpm_old;
        rpm_fetch (&t_rpm_in, time);
        
        time.new = t_spd_new;
        time.old = t_spd_old;
        spd_fetch (&t_spd_in, time);
        
        led_show (&t_led);  // RPM + SPD

        strip_mode (&t_strip);

        popup_play ();
    }
}