#include "wagn0.h"

#define FPS_COUNT 4
static const uint32_t fps_values[FPS_COUNT] = { 60, 45, 30, 15 };
static int fps_index = 0;
static uint32_t last_switch = 0;

void draw() {
    static int _init = 0;
    if (!_init) { _init = 1;
        set_fps(fps_values[0]);
        last_switch = w_ticks;
    }

    clear(screen, BLACK);

    // Switch FPS every second
    uint32_t now = w_ticks;
    if (now - last_switch >= 1000) {
        last_switch = now;
        fps_index = (fps_index + 1) % FPS_COUNT;
        set_fps(fps_values[fps_index]);
    }

    // Build status strings
    char target_str[32], actual_str[32], dt_str[32];
    int n;

    n = fps_values[fps_index];
    int ti = 0;
    if (n >= 100) target_str[ti++] = '0' + (n / 100);
    if (n >= 10)  target_str[ti++] = '0' + ((n / 10) % 10);
    target_str[ti++] = '0' + (n % 10);
    target_str[ti] = 0;

    n = (int)wagn0.fps;
    ti = 0;
    if (n >= 100) target_str[ti++] = '0' + (n / 100);
    /* reuse target_str as temp — rewrite actual_str properly */
    char buf_actual[32];
    ti = 0;
    if (n >= 100) buf_actual[ti++] = '0' + (n / 100);
    if (n >= 10)  buf_actual[ti++] = '0' + ((n / 10) % 10);
    buf_actual[ti++] = '0' + (n % 10);
    buf_actual[ti] = 0;

    // Frame time in ms
    int dt_ms = (int)(wagn0.delta_time * 1000.0f);
    char buf_dt[32];
    ti = 0;
    if (dt_ms >= 100) buf_dt[ti++] = '0' + (dt_ms / 100);
    if (dt_ms >= 10)  buf_dt[ti++] = '0' + ((dt_ms / 10) % 10);
    buf_dt[ti++] = '0' + (dt_ms % 10);
    buf_dt[ti] = 0;

    draw_text(screen, "Target FPS:", 10, 20, WHITE);
    draw_text(screen, target_str, 140, 20, YELLOW);

    draw_text(screen, "Actual FPS:", 10, 50, WHITE);
    draw_text(screen, buf_actual, 140, 50, buf_actual[0] != '0' ? GREEN : RED);

    draw_text(screen, "Frame time:", 10, 80, WHITE);
    draw_text(screen, buf_dt, 140, 80, CYAN);
    draw_text(screen, "ms", 180, 80, CYAN);
}
