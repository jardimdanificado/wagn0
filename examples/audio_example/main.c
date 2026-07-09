#include "wagn0.h"

static int active_count(void) {
    int n = 0;
    for (int i = 0; i < WAGN0_MAX_TONES; i++) {
        if (_wagn0_tones[i].active) n++;
    }
    return n;
}

void setup() {
    set_fps(60);
}

void draw() {
    clear(screen, rgb(20, 20, 30));

    // Sound wave visualization
    int count = active_count();
    if (count > 0) {
        float r_base = 60.0f + sin(w_ticks * 0.01f) * 20.0f;
        for (int i = 0; i < count; i++) {
            pixel_t col = lerp_color(CYAN, MAGENTA, (float)i / (float)(count > 1 ? count - 1 : 1));
            push(); translate(160, 100); scale((int)r_base + i * 15, (int)r_base + i * 15); stroke(col); draw_circle(); pop();
        }
    }

    push(); translate(100, 20); fill(WHITE); draw_text("Synth Piano Demo"); pop();

    char buf[32];
    buf[0] = 'A'; buf[1] = 'c'; buf[2] = 't'; buf[3] = 'i'; buf[4] = 'v'; buf[5] = 'e'; buf[6] = ':'; buf[7] = ' ';
    buf[8] = '0' + count; buf[9] = 0;
    push(); translate(130, 40); fill(GRAY); draw_text(buf); pop();

    // Keyboard UI
    const char* labels[] = {"A4", "C5", "E5", "G5", "Nse"};
    const char* keys[] = {"1", "2", "3", "4", "5"};
    for (int i = 0; i < 5; i++) {
        int kx = 35 + i * 50;
        int ky = 160;
        // Check if key is pressed for highlight
        int is_pressed = wagn0.keys[30 + i]; // 1=30, 2=31, etc.
        
        pixel_t fill = is_pressed ? rgb(100, 200, 100) : rgb(40, 40, 50);
        pixel_t outline = is_pressed ? WHITE : GRAY;
        
        push(); translate(kx, ky); scale(40, 60); fill(fill); draw_quad(); pop();
        push(); translate(kx, ky); scale(40, 60); stroke(outline); draw_quad(); pop();
        
        push(); translate(kx + 10, ky + 10); fill(outline); draw_text(labels[i]); pop();
        push(); translate(kx + 16, ky + 40); fill(is_pressed ? BLACK : WHITE); draw_text(keys[i]); pop();
    }
}

void key_pressed(int key) {
    if (key == 30) play_tone(440.0f, 0.5f, 0.4f);  // 1: A4
    if (key == 31) play_tone(523.0f, 0.5f, 0.4f);  // 2: C5
    if (key == 32) play_tone(659.0f, 0.5f, 0.4f);  // 3: E5
    if (key == 33) play_tone(784.0f, 0.5f, 0.4f);  // 4: G5
    if (key == 34) play_noise(0.3f, 0.3f);         // 5: noise burst
}
