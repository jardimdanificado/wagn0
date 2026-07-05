#include "wagn0.h"

static Wagn0Audio my_audio = {0};

void preload() {
    load_audio(&my_audio, "test.wav");
}

void setup() {
    set_fps(60);
}

void draw() {
    clear(screen, rgb(20, 20, 25));
    
    int playing = audio_is_playing();
    
    // Pulsing circle
    int cx = 160, cy = 100;
    float r = 50.0f;
    if (playing) {
        r += sin(w_ticks * 0.01f) * 5.0f;
    }
    
    pixel_t color = playing ? GREEN : GRAY;
    
    draw_circle_outline(screen, cx, cy, (int)r, color);
    draw_circle_outline(screen, cx, cy, (int)r + 1, color);
    
    // Play/Stop label
    const char* label = playing ? "PLAYING" : "STOPPED";
    int tw = text_width(label);
    draw_text(screen, label, cx - tw / 2, cy - text_height() / 2, color);
    
    // Progress ring (fake progress since we don't have get_position)
    if (playing) {
        float progress = (float)(w_ticks % 2000) / 2000.0f;
        int max_i = (int)(progress * 32.0f);
        for (int i = 0; i < max_i; i++) {
            float angle = -HALF_PI + i * (TWO_PI / 32.0f);
            int px = cx + (int)(cos(angle) * (r + 10.0f));
            int py = cy + (int)(sin(angle) * (r + 10.0f));
            draw_circle(screen, px, py, 2, CYAN);
        }
    }
    
    // Keybinds panel
    draw_rect_outline(screen, 60, 180, 200, 40, rgb(60, 60, 80));
    draw_text(screen, "[SPACE] Play/Restart", 70, 185, WHITE);
    draw_text(screen, "[S] Stop", 70, 205, WHITE);
    
    if (!my_audio.samples) {
        draw_text(screen, "Failed to load test.wav", 10, 10, RED);
    }
}

void key_pressed(int key) {
    if (key == 44 && my_audio.samples) {  // Space
        audio_stop(&my_audio);
        audio_play(&my_audio);
    }
    if (key == 22) { // S
        audio_stop(&my_audio);
    }
}
