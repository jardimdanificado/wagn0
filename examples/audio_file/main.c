#define WAGN0_NO_DEFAULT_CALLBACKS
#include "wagn0.h"

static Wagn0Audio my_audio = {0};

void setup() {
    w_setup("WagnO - Audio File", 320, 240, 16, 2);
    my_audio = wav_decode(assets_test_wav_data, sizeof(assets_test_wav_data));
    if (my_audio.samples) audio_play(&my_audio);
}

void draw() {
    background(BLACK);
    fill(WHITE);
    if (audio_is_playing()) {
        text("playing...", 10, 10);
    } else if (my_audio.samples) {
        text("press SPACE to play", 10, 10);
    } else {
        text("failed to decode", 10, 10);
    }
}

void update(void) {}
void mouse_pressed(void) {}
void mouse_released(void) {}
void key_released(int k) { (void)k; }
void key_pressed(int key) {
    if (key == 0x2C && my_audio.samples) {  // Space
        audio_play(&my_audio);
    }
}
