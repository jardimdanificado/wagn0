#include "wagner.h"

static Canvas img = {0};

void preload() {
    load_image(&img, "test.png");
}

void setup() {
    set_fps(60);
}

void draw_number(int n, int x, int y, pixel_t color) {
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
    push(); translate(x, y); fill(color); text(str); pop();
}

void draw() {
    push(); fill(rgb(30, 30, 40)); clear(); pop();
    
    if (img.pixels) {
        int ix = (320 - img.width) / 2;
        int iy = (240 - img.height) / 2;
        
        // Animated border
        float t = (sin(w_ticks * 0.005f) + 1.0f) * 0.5f;
        pixel_t border_col = lerp_color(CYAN, MAGENTA, t);
        
        push(); translate(ix - 2, iy - 2); scale(img.width + 4, img.height + 4); stroke(border_col); rect(); pop();
        push(); translate(ix - 1, iy - 1); scale(img.width + 2, img.height + 2); stroke(border_col); rect(); pop();
        
        push(); translate(ix, iy); scale(img.width, img.height); texture(&img); rect(); pop();
        
        // Dimensions
        push(); translate(10, 220); fill(GRAY); text("W:"); pop();
        draw_number(img.width, 30, 220, WHITE);
        
        push(); translate(70, 220); fill(GRAY); text("H:"); pop();
        draw_number(img.height, 90, 220, WHITE);
    } else {
        // Red X
        push(); translate(100, 60); stroke(RED); line(220 - (100), 180 - (60), 0, 0); pop();
        push(); translate(220, 60); stroke(RED); line(100 - (220), 180 - (60), 0, 0); pop();
        push(); translate(60, 200); fill(RED); text("FAILED TO LOAD test.png"); pop();
    }
}
