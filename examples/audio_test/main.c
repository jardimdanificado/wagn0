#include "wagn0.h"

void setup() {
    w_setup("Audio Test", 320, 240, 16, 2);
    play_tone(440.0f, 60.0f, 0.3f);
}

void draw() {
    clear(screen, BLACK);
    draw_text(screen, audio_is_playing() ? "playing 440Hz" : "stopped", 10, 10, audio_is_playing() ? GREEN : RED);
    draw_text(screen, "SPACE = restart", 10, 30, WHITE);
    if (w_keys[0x2C] && !audio_is_playing()) {
        stop_all_sounds();
        play_tone(440.0f, 60.0f, 0.3f);
    }
}
