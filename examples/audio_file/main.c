#include "wagner.h"

static WagnerAudio my_audio = {0};

void preload() {
    load_audio(&my_audio, "test.wav");
}

void setup() {
    set_fps(60);
}

void draw() {
    push(); fill(rgb(20, 20, 25)); clear(screen); pop();
    
    int playing = audio_is_playing();
    
    // Pulsing circle
    int cx = 160, cy = 100;
    float r = 50.0f;
    if (playing) {
        r += sin(w_ticks * 0.01f) * 5.0f;
    }
    
    pixel_t color = playing ? GREEN : GRAY;
    
    push(); translate(cx, cy); scale((int)r, (int)r); stroke(color); circle(screen); pop();
    push(); translate(cx, cy); scale((int)r + 1, (int)r + 1); stroke(color); circle(screen); pop();
    
    // Play/Stop label
    const char* label = playing ? "PLAYING" : "STOPPED";
    int tw = text_width(label);
    push(); translate(cx - tw / 2, cy - text_height() / 2); fill(color); text(screen, label); pop();
    
    // Progress ring (fake progress since we don't have get_position)
    if (playing) {
        float progress = (float)(w_ticks % 2000) / 2000.0f;
        int max_i = (int)(progress * 32.0f);
        for (int i = 0; i < max_i; i++) {
            float angle = -HALF_PI + i * (TWO_PI / 32.0f);
            int px = cx + (int)(cos(angle) * (r + 10.0f));
            int py = cy + (int)(sin(angle) * (r + 10.0f));
            push(); translate(px, py); scale(2, 2); fill(CYAN); circle(screen); pop();
        }
    }
    
    // Keybinds panel
    push(); translate(60, 180); scale(200, 40); stroke(rgb(60, 60, 80)); rect(screen); pop();
    push(); translate(70, 185); fill(WHITE); text(screen, "[SPACE] Play/Restart"); pop();
    push(); translate(70, 205); fill(WHITE); text(screen, "[S] Stop"); pop();
    
    if (!my_audio.samples) {
        push(); translate(10, 10); fill(RED); text(screen, "Failed to load test.wav"); pop();
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
