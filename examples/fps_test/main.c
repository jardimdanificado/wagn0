#include "wagn0.h"

#define FPS_COUNT 4
static const uint32_t fps_values[FPS_COUNT] = { 60, 45, 30, 15 };
static int fps_index = 0;
static uint32_t last_switch = 0;

void setup() {
    set_fps(fps_values[0]);
    last_switch = w_ticks;
}

static void draw_number(Canvas c, int n, int x, int y, pixel_t color) {
    char str[16];
    int i = 0;
    if (n == 0) { str[i++] = '0'; }
    else {
        int tmp = n;
        while (tmp > 0) { str[i++] = '0' + (tmp % 10); tmp /= 10; }
        for (int j = 0; j < i/2; j++) {
            char t = str[j]; str[j] = str[i-1-j]; str[i-1-j] = t;
        }
    }
    str[i] = '\0';
    draw_text(c, str, x, y, color);
}

void draw() {
    clear(screen, rgb(15, 15, 15));
    
    uint32_t now = w_ticks;
    if (now - last_switch >= 2000) {
        last_switch = now;
        fps_index = (fps_index + 1) % FPS_COUNT;
        set_fps(fps_values[fps_index]);
    }
    
    // Rotating wheel
    int cx = 220, cy = 120, r = 60;
    float angle = now * 0.003f;
    
    draw_circle_outline(screen, cx, cy, r, CYAN);
    for (int i = 0; i < 8; i++) {
        float a = angle + i * (TWO_PI / 8.0f);
        int px = cx + (int)(cos(a) * r);
        int py = cy + (int)(sin(a) * r);
        draw_line(screen, cx, cy, px, py, lerp_color(BLUE, CYAN, (float)i/7.0f));
    }
    
    // Panel
    draw_rect_outline(screen, 10, 40, 140, 160, rgb(60, 60, 60));
    draw_text(screen, "FPS TEST", 45, 50, WHITE);
    draw_line(screen, 10, 70, 150, 70, rgb(60, 60, 60));
    
    draw_text(screen, "Target:", 20, 90, GRAY);
    draw_number(screen, fps_values[fps_index], 90, 90, WHITE);
    
    draw_text(screen, "Actual:", 20, 130, GRAY);
    
    // Color based on performance
    float ratio = (float)wagn0.fps / (float)fps_values[fps_index];
    if (ratio > 1.0f) ratio = 1.0f;
    pixel_t fps_color = lerp_color(RED, GREEN, ratio);
    draw_number(screen, wagn0.fps, 90, 130, fps_color);
    
    draw_text(screen, "Delta:", 20, 170, GRAY);
    draw_number(screen, (int)(wagn0.delta_time * 1000.0f), 90, 170, YELLOW);
    draw_text(screen, "ms", 115, 170, YELLOW);
}
