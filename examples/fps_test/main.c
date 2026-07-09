#include "wagner.h"

#define FPS_COUNT 4
static const uint32_t fps_values[FPS_COUNT] = { 60, 45, 30, 15 };
static int fps_index = 0;
static uint32_t last_switch = 0;

void setup() {
    set_fps(fps_values[0]);
    last_switch = w_ticks;
}

static void draw_number(int n, int x, int y, pixel_t color) {
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
    push();
    translate(x, y);
    fill(color);
    text(str);
    pop();
}

static void draw_str(const char* str, int x, int y, pixel_t color) {
    push();
    translate(x, y);
    fill(color);
    text(str);
    pop();
}

void draw() {
    clear(rgb(15, 15, 15));
    
    uint32_t now = w_ticks;
    if (now - last_switch >= 2000) {
        last_switch = now;
        fps_index = (fps_index + 1) % FPS_COUNT;
        set_fps(fps_values[fps_index]);
    }
    
    // Rotating wheel
    int cx = 220, cy = 120, r = 60;
    float angle = now * 0.003f;
    
    push();
    translate(cx, cy);
    scale(r, r);
    stroke(CYAN);
    circle();
    pop();
    
    for (int i = 0; i < 8; i++) {
        float a = angle + i * (TWO_PI / 8.0f);
        int px = cx + (int)(cos(a) * r);
        int py = cy + (int)(sin(a) * r);
        push();
        translate(cx, cy);
        stroke(lerp_color(BLUE, CYAN, (float)i/7.0f));
        line(px - cx, py - cy, 0, 0); // Wait, old line took x1,y1,x2,y2. In state machine line takes x1,y1,x2,y2, but transformed. Wait, my line definition is void line(float x1, float y1, float x2, float y2);
        pop();
    }
    
    // Panel
    push();
    translate(10, 40);
    scale(140, 160);
    stroke(rgb(60, 60, 60));
    quad();
    pop();
    
    draw_str("FPS TEST", 45, 50, WHITE);
    
    push();
    translate(0, 0);
    stroke(rgb(60, 60, 60));
    line(10, 70, 150, 70);
    pop();
    
    draw_str("Target:", 20, 90, GRAY);
    draw_number(fps_values[fps_index], 90, 90, WHITE);
    
    draw_str("Actual:", 20, 130, GRAY);
    
    // Color based on performance
    float ratio = (float)wagner.fps / (float)fps_values[fps_index];
    if (ratio > 1.0f) ratio = 1.0f;
    pixel_t fps_color = lerp_color(RED, GREEN, ratio);
    draw_number(wagner.fps, 90, 130, fps_color);
    
    draw_str("Delta:", 20, 170, GRAY);
    draw_number((int)(wagner.delta_time * 1000.0f), 90, 170, YELLOW);
    draw_str("ms", 115, 170, YELLOW);
}
