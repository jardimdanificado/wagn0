#include "wagn0.h"

void setup() {
    set_fps(60);
    play_tone(440.0f, 60.0f, 0.3f);
}

void draw() {
    clear(screen, rgb(10, 15, 20));
    
    int playing = audio_is_playing();
    pixel_t status_color = playing ? GREEN : RED;
    
    draw_text(screen, "Audio Tone Test", 10, 10, WHITE);
    draw_text(screen, playing ? "PLAYING 440Hz" : "STOPPED", 10, 30, status_color);
    draw_text(screen, "SPACE = restart tone", 10, 50, GRAY);
    
    // Draw waveform
    int wave_y = 140;
    int wave_h = 50;
    
    draw_rect_outline(screen, 10, wave_y - wave_h - 10, 300, wave_h * 2 + 20, rgb(40, 40, 50));
    
    if (playing) {
        float scroll = w_ticks * 0.05f;
        int prev_x = 10;
        int prev_y = wave_y + (int)(sin(scroll) * wave_h);
        
        for (int x = 11; x < 310; x++) {
            float phase = scroll + (x * 0.1f);
            int y = wave_y + (int)(sin(phase) * wave_h);
            draw_line(screen, prev_x, prev_y, x, y, CYAN);
            prev_x = x;
            prev_y = y;
        }
    } else {
        draw_line(screen, 10, wave_y, 310, wave_y, rgb(80, 80, 80));
    }
}

void key_pressed(int key) {
    if (key == 44) { // Space
        stop_all_sounds();
        play_tone(440.0f, 60.0f, 0.3f);
    }
}
