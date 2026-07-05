#include "wagn0.h"

static int counter = 0;
static uint32_t last_save_time = 0;

void preload() {
}

void setup() {
    set_fps(60);
    counter = 0;
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
    clear(screen, rgb(15, 20, 25));
    
    counter++;
    
    draw_text(screen, "Save System Test", 10, 10, WHITE);
    draw_line(screen, 10, 25, 150, 25, rgb(50, 50, 60));
    
    draw_text(screen, "Counter:", 10, 50, GRAY);
    draw_number(screen, counter, 80, 50, CYAN);
    
    draw_text(screen, "[SPACE] to Save", 10, 80, WHITE);
    
    // Status indicator
    int cx = 160, cy = 160;
    
    if (file_is_saving()) {
        draw_text(screen, "Saving...", cx - 30, cy + 30, YELLOW);
        
        // Spinner
        float t = w_ticks * 0.01f;
        for (int i = 0; i < 8; i++) {
            float angle = i * (TWO_PI / 8.0f);
            float alpha = sin(t - angle) * 0.5f + 0.5f;
            pixel_t col = lerp_color(rgb(40, 40, 0), YELLOW, alpha);
            int px = cx + (int)(cos(angle) * 20.0f);
            int py = cy + (int)(sin(angle) * 20.0f);
            draw_circle(screen, px, py, 3, col);
        }
    } else {
        if (last_save_time > 0 && w_ticks - last_save_time < 2000) {
            // Green checkmark
            draw_circle_outline(screen, cx, cy, 20, GREEN);
            draw_line(screen, cx - 10, cy, cx - 3, cy + 8, GREEN);
            draw_line(screen, cx - 3, cy + 8, cx + 12, cy - 10, GREEN);
            
            draw_text(screen, "Saved successfully!", cx - 70, cy + 30, GREEN);
        } else {
            draw_circle_outline(screen, cx, cy, 20, rgb(50, 50, 50));
        }
    }
}

void key_pressed(int key) {
    if (key == 44) { // Space
        if (!file_is_saving()) {
            char save_data[32];
            int n = counter;
            int i = 0;
            if (n == 0) { save_data[i++] = '0'; }
            else {
                int tmp = n;
                while (tmp > 0) { save_data[i++] = '0' + (tmp % 10); tmp /= 10; }
                for (int j = 0; j < i/2; j++) {
                    char t = save_data[j]; save_data[j] = save_data[i-1-j]; save_data[i-1-j] = t;
                }
            }
            save_data[i] = '\0';
            
            if (file_save("save.txt", save_data, i)) {
                last_save_time = w_ticks;
            }
        }
    }
}
