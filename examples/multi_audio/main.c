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
    clear(rgb(10, 15, 25));
    push(); translate(10, 10); fill(WHITE); draw_text("Multi-channel Audio Mixer"); pop();
    
    // Track 1
    int x1 = 15;
    push(); translate(x1, 40); scale(90, 100); stroke(playing1 ? GREEN : rgb(50, 50, 70)); draw_quad(); pop();
    push(); translate(x1 + 5, 45); fill(WHITE); draw_text("audio.mp3"); pop();
    push(); translate(x1 + 80, 50); scale(4, 4); fill(track1.samples ? GREEN : RED); draw_circle(); pop();
    push(); translate(x1 + 35, 120); fill(GRAY); draw_text("[1]"); pop();
    if (playing1) { push(); translate(x1 + 45, 80); scale((int)(10 + sin(w_ticks*0.02f)*5), (int)(10 + sin(w_ticks*0.02f)*5)); stroke(GREEN); draw_circle(); pop(); }

    // Track 2
    int x2 = 115;
    push(); translate(x2, 40); scale(90, 100); stroke(playing2 ? CYAN : rgb(50, 50, 70)); draw_quad(); pop();
    push(); translate(x2 + 5, 45); fill(WHITE); draw_text("audio1.mp3"); pop();
    push(); translate(x2 + 80, 50); scale(4, 4); fill(track2.samples ? GREEN : RED); draw_circle(); pop();
    push(); translate(x2 + 35, 120); fill(GRAY); draw_text("[2]"); pop();
    if (playing2) { push(); translate(x2 + 45, 80); scale((int)(10 + cos(w_ticks*0.02f)*5), (int)(10 + cos(w_ticks*0.02f)*5)); stroke(CYAN); draw_circle(); pop(); }

    // Track 3
    int x3 = 215;
    push(); translate(x3, 40); scale(90, 100); stroke(playing3 ? YELLOW : rgb(50, 50, 70)); draw_quad(); pop();
    push(); translate(x3 + 5, 45); fill(WHITE); draw_text("test.wav"); pop();
    push(); translate(x3 + 80, 50); scale(4, 4); fill(track3.samples ? GREEN : RED); draw_circle(); pop();
    push(); translate(x3 + 35, 120); fill(GRAY); draw_text("[3]"); pop();
    if (playing3) { push(); translate(x3 + 45, 80); scale((int)(10 + sin(w_ticks*0.03f)*5), (int)(10 + sin(w_ticks*0.03f)*5)); stroke(YELLOW); draw_circle(); pop(); }

    push(); translate(90, 170); fill(GRAY); draw_text("[S] Stop all tracks"); pop();
    
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
