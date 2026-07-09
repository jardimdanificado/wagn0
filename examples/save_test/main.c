#include "wagner.h"

static int counter = 0;
static uint32_t last_save_time = 0;

void preload() {
}

void setup() {
    set_fps(60);
    counter = 0;
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
    push(); translate(x, y); fill(color); text(str); pop();
}

void draw() {
    clear(rgb(15, 20, 25));
    
    counter++;
    
    push(); translate(10, 10); fill(WHITE); text("Save System Test"); pop();
    push(); translate(10, 25); stroke(rgb(50, 50, 60)); line(140, 0, 0, 0); pop();
    
    push(); translate(10, 50); fill(GRAY); text("Counter:"); pop();
    draw_number(counter, 80, 50, CYAN);
    
    push(); translate(10, 80); fill(WHITE); text("[SPACE] to Save"); pop();
    
    // Status indicator
    int cx = 160, cy = 160;
    
    if (file_is_saving()) {
        push(); translate(cx - 30, cy + 30); fill(YELLOW); text("Saving..."); pop();
        
        // Spinner
        float t = w_ticks * 0.01f;
        for (int i = 0; i < 8; i++) {
            float angle = i * (TWO_PI / 8.0f);
            float alpha = sin(t - angle) * 0.5f + 0.5f;
            pixel_t col = lerp_color(rgb(40, 40, 0), YELLOW, alpha);
            int px = cx + (int)(cos(angle) * 20.0f);
            int py = cy + (int)(sin(angle) * 20.0f);
            push(); translate(px, py); scale(3, 3); fill(col); circle(); pop();
        }
    } else {
        if (last_save_time > 0 && w_ticks - last_save_time < 2000) {
            // Green checkmark
            push(); translate(cx, cy); scale(20, 20); stroke(GREEN); circle(); pop();
            push(); translate(cx - 10, cy); stroke(GREEN); line(cx - 3 - (cx - 10), cy + 8 - (cy), 0, 0); pop();
            push(); translate(cx - 3, cy + 8); stroke(GREEN); line(cx + 12 - (cx - 3), cy - 10 - (cy + 8), 0, 0); pop();
            
            push(); translate(cx - 70, cy + 30); fill(GREEN); text("Saved successfully!"); pop();
        } else {
            push(); translate(cx, cy); scale(20, 20); stroke(rgb(50, 50, 50)); circle(); pop();
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
