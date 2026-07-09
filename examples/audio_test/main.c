#include "wagner.h"

void setup() {
    set_fps(60);
    play_tone(440.0f, 60.0f, 0.3f);
}

void draw() {
    clear(rgb(10, 15, 20));
    
    int playing = audio_is_playing();
    pixel_t status_color = playing ? GREEN : RED;
    
    push(); translate(10, 10); fill(WHITE); draw_text("Audio Tone Test"); pop();
    push(); translate(10, 30); fill(status_color); draw_text(playing ? "PLAYING 440Hz" : "STOPPED"); pop();
    push(); translate(10, 50); fill(GRAY); draw_text("SPACE = restart tone"); pop();
    
    // Draw waveform
    int wave_y = 140;
    int wave_h = 50;
    
    push();
    translate(10, wave_y - wave_h - 10);
    scale(300, wave_h * 2 + 20);
    stroke(rgb(40, 40, 50));
    draw_quad();
    pop();
    
    if (playing) {
        float scroll = w_ticks * 0.05f;
        int prev_x = 10;
        int prev_y = wave_y + (int)(sin(scroll) * wave_h);
        
        for (int x = 11; x < 310; x++) {
            float phase = scroll + (x * 0.1f);
            int y = wave_y + (int)(sin(phase) * wave_h);
            push(); translate(prev_x, prev_y); stroke(CYAN); draw_line(x - (prev_x), y - (prev_y), 0, 0); pop();
            prev_x = x;
            prev_y = y;
        }
    } else {
        push();
        translate(10, wave_y);
        stroke(rgb(80, 80, 80));
        draw_line(300, 0, 0, 0);
        pop();
    }
}

void key_pressed(int key) {
    if (key == 44) { // Space
        stop_all_sounds();
        play_tone(440.0f, 60.0f, 0.3f);
    }
}
