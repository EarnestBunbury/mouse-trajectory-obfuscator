/**
 * @file main.c
 * @brief RP2040 USB HID Mouse Trajectory Obfuscator & Jitter Injector
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "bsp/board.h"
#include "tusb.h"

#define JITTER_AMPLITUDE_MAX 1     
#define LATENCY_JITTER_MS_MIN 2    
#define LATENCY_JITTER_MS_MAX 8    
#define BURST_THRESHOLD_COUNT 15   

static int8_t last_dx = 0;
static int8_t last_dy = 0;
static uint32_t movement_counter = 0;

static uint32_t lcg_seed = 133799;
static inline uint32_t fast_rand(void) {
    lcg_seed = (lcg_seed * 1103515245 + 12345);
    return lcg_seed;
}

int main(void) {
    board_init();
    stdio_init_all();
    
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 1);

    tusb_init();

    while (1) {
        tud_task();
    }

    return 0;
}

void obfuscate_and_forward_mouse(uint8_t report_id, uint8_t button, int8_t x, int8_t y, int8_t wheel, int8_t pan) {
    if (x == 0 && y == 0 && wheel == 0) {
        return;
    }

    movement_counter++;

    int8_t jitter_x = 0;
    int8_t jitter_y = 0;
    
    if ((fast_rand() % 100) < 35) {
        jitter_x = (int8_t)((fast_rand() % (2 * JITTER_AMPLITUDE_MAX + 1)) - JITTER_AMPLITUDE_MAX);
        jitter_y = (int8_t)((fast_rand() % (2 * JITTER_AMPLITUDE_MAX + 1)) - JITTER_AMPLITUDE_MAX);
    }

    uint32_t artificial_delay = LATENCY_JITTER_MS_MIN + (fast_rand() % (LATENCY_JITTER_MS_MAX - LATENCY_JITTER_MS_MIN + 1));
    
    if (movement_counter > BURST_THRESHOLD_COUNT) {
        x = (x + last_dx) / 2;
        y = (y + last_dy) / 2;
        movement_counter = 0;
    }

    last_dx = x;
    last_dy = y;

    int8_t final_x = x + jitter_x;
    int8_t final_y = y + jitter_y;

    if (tud_hid_ready()) {
        sleep_ms(artificial_delay / 2);
        tud_hid_mouse_report(report_id, button, final_x, final_y, wheel, pan);
        sleep_ms(artificial_delay / 2);
    }
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    (void)instance; (void)report_id; (void)report_type; (void)buffer; (void)reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    (void)instance; (void)report_id; (void)report_type; (void)buffer; (void)bufsize;
}
