#include "wagn0.h"

static Canvas img;
static int pos_x = 100, pos_y = 70;
static int scale_w = 120, scale_h = 100;

void preload() {
    load_image(&img, "sprite.png");
}

void setup() {
    set_fps(60);
}

static void draw_number(Canvas c, int n, int x, int y, pixel_t color) {
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
    draw_text(c, str, x, y, color);
}

void draw() {
    // Input
    if (w_gamepad_buttons & W_BTN_LEFT || wagn0.keys[80])  pos_x -= 2;
    if (w_gamepad_buttons & W_BTN_RIGHT || wagn0.keys[79]) pos_x += 2;
    if (w_gamepad_buttons & W_BTN_UP || wagn0.keys[82])    pos_y -= 2;
    if (w_gamepad_buttons & W_BTN_DOWN || wagn0.keys[81])  pos_y += 2;
    if (w_gamepad_buttons & W_BTN_L1 || wagn0.keys[29])    scale_w -= 2; // z
    if (w_gamepad_buttons & W_BTN_R1 || wagn0.keys[27])    scale_w += 2; // x
    if (w_gamepad_buttons & W_BTN_L2)    scale_h -= 2;
    if (w_gamepad_buttons & W_BTN_R2)    scale_h += 2;
    if (w_gamepad_buttons & W_BTN_A || wagn0.keys[4]) { 
        if (img.pixels) { scale_w = img.width; scale_h = img.height; }
    }

    // Checkerboard
    for (int y = 0; y < 240; y += 20) {
        for (int x = 0; x < 320; x += 20) {
            pixel_t col = ((x / 20) + (y / 20)) % 2 == 0 ? rgb(20, 20, 30) : rgb(30, 30, 40);
            draw_rect(screen, x, y, 20, 20, col);
        }
    }

    // Sprite and highlight
    if (img.pixels) {
        // Highlight ring
        int cx = pos_x + scale_w / 2;
        int cy = pos_y + scale_h / 2;
        float r = (scale_w > scale_h ? scale_w : scale_h) * 0.6f + 10.0f;
        r += sin(w_ticks * 0.01f) * 5.0f;
        draw_circle_outline(screen, cx, cy, (int)r, rgb(100, 200, 255));
        
        draw_canvas_scaled(screen, img, pos_x, pos_y, scale_w, scale_h);
        draw_rect_outline(screen, pos_x, pos_y, scale_w, scale_h, YELLOW);
    }

    // Info panel
    draw_rect(screen, 210, 10, 100, 70, rgb(0, 0, 0)); // solid black for readability
    draw_rect_outline(screen, 210, 10, 100, 70, GRAY);
    draw_text(screen, "POS X:", 215, 15, GRAY); draw_number(screen, pos_x, 260, 15, WHITE);
    draw_text(screen, "POS Y:", 215, 30, GRAY); draw_number(screen, pos_y, 260, 30, WHITE);
    draw_text(screen, "SCL W:", 215, 45, GRAY); draw_number(screen, scale_w, 260, 45, CYAN);
    draw_text(screen, "SCL H:", 215, 60, GRAY); draw_number(screen, scale_h, 260, 60, CYAN);
}
