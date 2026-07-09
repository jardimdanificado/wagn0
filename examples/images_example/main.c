#include "wagner.h"

static Canvas img;
static int pos_x = 100, pos_y = 70;
static int scale_w = 120, scale_h = 100;

void preload() {
    load_image(&img, "sprite.png");
}

void setup() {
    set_fps(60);
}

static void draw_number(int n, int x, int y, pixel_t color) {
    char str[16];
    int i = 0, is_neg = 0;
    if (n < 0) { is_neg = 1; n = -n; }
    if (n == 0) { str[i++] = '0'; }
    else {
        int tmp = n;
        while (tmp > 0) { str[i++] = '0' + (tmp % 10); tmp /= 10; }
        if (is_neg) str[i++] = '-';
        for (int j = 0; j < i/2; j++) {
            char t = str[j]; str[j] = str[i-1-j]; str[i-1-j] = t;
        }
    }
    str[i] = '\0';
    push();
    translate(x, y);
    fill(color);
    draw_text(str);
    pop();
}

static void draw_str(const char* str, int x, int y, pixel_t color) {
    push();
    translate(x, y);
    fill(color);
    draw_text(str);
    pop();
}

void draw() {
    // Input
    if (w_gamepad_buttons & W_BTN_LEFT || wagner.keys[80])  pos_x -= 2;
    if (w_gamepad_buttons & W_BTN_RIGHT || wagner.keys[79]) pos_x += 2;
    if (w_gamepad_buttons & W_BTN_UP || wagner.keys[82])    pos_y -= 2;
    if (w_gamepad_buttons & W_BTN_DOWN || wagner.keys[81])  pos_y += 2;
    if (w_gamepad_buttons & W_BTN_L1 || wagner.keys[29])    scale_w -= 2; // z
    if (w_gamepad_buttons & W_BTN_R1 || wagner.keys[27])    scale_w += 2; // x
    if (w_gamepad_buttons & W_BTN_L2)    scale_h -= 2;
    if (w_gamepad_buttons & W_BTN_R2)    scale_h += 2;
    if (w_gamepad_buttons & W_BTN_A || wagner.keys[4]) { 
        if (img.pixels) { scale_w = img.width; scale_h = img.height; }
    }

    // Checkerboard
    for (int y = 0; y < 240; y += 20) {
        for (int x = 0; x < 320; x += 20) {
            pixel_t col = ((x / 20) + (y / 20)) % 2 == 0 ? rgb(20, 20, 30) : rgb(30, 30, 40);
            push();
            translate(x, y);
            scale(20, 20);
            fill(col);
            draw_quad();
            pop();
        }
    }

    // Sprite and highlight
    if (img.pixels) {
        // Highlight ring
        int cx = pos_x + scale_w / 2;
        int cy = pos_y + scale_h / 2;
        float r = (scale_w > scale_h ? scale_w : scale_h) * 0.6f + 10.0f;
        r += sin(w_ticks * 0.01f) * 5.0f;
        
        push();
        translate(cx, cy);
        scale(r, r);
        stroke(rgb(100, 200, 255));
        draw_circle();
        pop();
        
        push();
        translate(pos_x, pos_y);
        scale(scale_w, scale_h);
        texture(&img);
        stroke(YELLOW); // Outline
        draw_quad();
        pop();
    }

    // Info panel
    push();
    translate(210, 10);
    scale(100, 70);
    fill(rgb(0, 0, 0));
    stroke(GRAY);
    draw_quad();
    pop();
    
    draw_str("POS X:", 215, 15, GRAY); draw_number(pos_x, 260, 15, WHITE);
    draw_str("POS Y:", 215, 30, GRAY); draw_number(pos_y, 260, 30, WHITE);
    draw_str("SCL W:", 215, 45, GRAY); draw_number(scale_w, 260, 45, CYAN);
    draw_str("SCL H:", 215, 60, GRAY); draw_number(scale_h, 260, 60, CYAN);
}
