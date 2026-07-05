#include "wagn0.h"

static Canvas img = {0};

void preload() {
    load_image(&img, "test.png");
}

void setup() {
    set_fps(60);
}

void draw_number(Canvas c, int n, int x, int y, pixel_t color) {
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
    clear(screen, rgb(30, 30, 40));
    
    if (img.pixels) {
        int ix = (320 - img.width) / 2;
        int iy = (240 - img.height) / 2;
        
        // Animated border
        float t = (sin(w_ticks * 0.005f) + 1.0f) * 0.5f;
        pixel_t border_col = lerp_color(CYAN, MAGENTA, t);
        
        draw_rect_outline(screen, ix - 2, iy - 2, img.width + 4, img.height + 4, border_col);
        draw_rect_outline(screen, ix - 1, iy - 1, img.width + 2, img.height + 2, border_col);
        
        draw_canvas(screen, img, ix, iy);
        
        // Dimensions
        draw_text(screen, "W:", 10, 220, GRAY);
        draw_number(screen, img.width, 30, 220, WHITE);
        
        draw_text(screen, "H:", 70, 220, GRAY);
        draw_number(screen, img.height, 90, 220, WHITE);
    } else {
        // Red X
        draw_line(screen, 100, 60, 220, 180, RED);
        draw_line(screen, 220, 60, 100, 180, RED);
        draw_text(screen, "FAILED TO LOAD test.png", 60, 200, RED);
    }
}
