#include "wagn0.h"

Wagn0Audio track1;
Wagn0Audio track2;
Wagn0Audio track3;

void preload() {
    load_audio(&track1, "audio.mp3");
    load_audio(&track2, "audio1.mp3");
    load_audio(&track3, "test.wav");
}

void draw() {
    clear(screen, 0xFF111122);
    draw_text(screen, "Multi Audio Mixer Test", 10, 10, 0xFF00FF00);
    
    draw_text(screen, "Press 1: Play MP3 Track A", 10, 40, 0xFFFFFFFF);
    draw_text(screen, "Press 2: Play MP3 Track B", 10, 60, 0xFFFFFFFF);
    draw_text(screen, "Press 3: Play WAV Sound",   10, 80, 0xFFFFFFFF);
    
    // Animate something so we know it's not frozen
    int anim_x = 10 + (w_ticks / 10) % 200;
    draw_rect(screen, anim_x, 120, 20, 20, 0xFFFF0000);

    draw_text(screen, "Load Status:", 10, 150, 0xFFFFFFFF);
    draw_rect(screen, 10, 170, 20, 20, track1.samples ? 0xFF00FF00 : 0xFFFF0000);
    draw_rect(screen, 40, 170, 20, 20, track2.samples ? 0xFF00FF00 : 0xFFFF0000);
    draw_rect(screen, 70, 170, 20, 20, track3.samples ? 0xFF00FF00 : 0xFFFF0000);

    draw_text(screen, "Is Playing:", 10, 200, 0xFFFFFFFF);
    draw_rect(screen, 10, 220, 20, 20, audio_is_playing() ? 0xFFFFFFFF : 0xFF444444);
}

void key_pressed(int key) {
    if (key == 30 || key == '1') {
        audio_play(&track1);
    } else if (key == 31 || key == '2') {
        audio_play(&track2);
    } else if (key == 32 || key == '3') {
        audio_play(&track3);
    }
}

void key_released(int key) {}
void mouse_pressed() {}
void mouse_released() {}
