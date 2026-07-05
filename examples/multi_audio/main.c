#include "wagn0.h"

static Wagn0Audio track1;
static Wagn0Audio track2;
static Wagn0Audio track3;

void preload() {
    load_audio(&track1, "audio.mp3");
    load_audio(&track2, "audio1.mp3");
    load_audio(&track3, "test.wav");
}

void setup() {
    set_fps(60);
}

static int playing1 = 0;
static int playing2 = 0;
static int playing3 = 0;

void draw() {
    clear(screen, rgb(10, 15, 25));
    draw_text(screen, "Multi-channel Audio Mixer", 10, 10, WHITE);
    
    // Track 1
    int x1 = 15;
    draw_rect_outline(screen, x1, 40, 90, 100, playing1 ? GREEN : rgb(50, 50, 70));
    draw_text(screen, "audio.mp3", x1 + 5, 45, WHITE);
    draw_circle(screen, x1 + 80, 50, 4, track1.samples ? GREEN : RED);
    draw_text(screen, "[1]", x1 + 35, 120, GRAY);
    if (playing1) draw_circle_outline(screen, x1 + 45, 80, (int)(10 + sin(w_ticks*0.02f)*5), GREEN);

    // Track 2
    int x2 = 115;
    draw_rect_outline(screen, x2, 40, 90, 100, playing2 ? CYAN : rgb(50, 50, 70));
    draw_text(screen, "audio1.mp3", x2 + 5, 45, WHITE);
    draw_circle(screen, x2 + 80, 50, 4, track2.samples ? GREEN : RED);
    draw_text(screen, "[2]", x2 + 35, 120, GRAY);
    if (playing2) draw_circle_outline(screen, x2 + 45, 80, (int)(10 + cos(w_ticks*0.02f)*5), CYAN);

    // Track 3
    int x3 = 215;
    draw_rect_outline(screen, x3, 40, 90, 100, playing3 ? YELLOW : rgb(50, 50, 70));
    draw_text(screen, "test.wav", x3 + 5, 45, WHITE);
    draw_circle(screen, x3 + 80, 50, 4, track3.samples ? GREEN : RED);
    draw_text(screen, "[3]", x3 + 35, 120, GRAY);
    if (playing3) draw_circle_outline(screen, x3 + 45, 80, (int)(10 + sin(w_ticks*0.03f)*5), YELLOW);

    draw_text(screen, "[S] Stop all tracks", 90, 170, GRAY);
    
    // Simple state decay to simulate ending
    if (w_ticks % 60 == 0) {
        if (!audio_is_playing()) {
            playing1 = playing2 = playing3 = 0;
        }
    }
}

void key_pressed(int key) {
    if (key == 30 && track1.samples) { audio_play(&track1); playing1 = 1; }
    if (key == 31 && track2.samples) { audio_play(&track2); playing2 = 1; }
    if (key == 32 && track3.samples) { audio_play(&track3); playing3 = 1; }
    if (key == 22) { // S
        audio_stop(&track1);
        audio_stop(&track2);
        audio_stop(&track3);
        playing1 = playing2 = playing3 = 0;
    }
}
