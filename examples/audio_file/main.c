#include "wagn0.h"

static Wagn0Audio my_audio = {0};

void preload() {
    load_audio(&my_audio, "test.wav");
}

void setup() {
    if (my_audio.samples) audio_play(&my_audio);
}

void draw() {

    clear(screen, BLACK);
    if (audio_is_playing()) {
        draw_text(screen, "playing...", 10, 10, WHITE);
    } else if (my_audio.samples) {
        draw_text(screen, "press SPACE to play", 10, 10, WHITE);
    } else {
        draw_text(screen, "failed to decode", 10, 10, WHITE);
    }
}

void key_pressed(int key) {
    if (key == 0x2C && my_audio.samples) {  // Space
        audio_play(&my_audio);
    }
}
