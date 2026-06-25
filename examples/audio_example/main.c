#include "wagn0.h"

void setup() {
    w_setup("WagnO - Audio Synth", 320, 240, 16, 2);
}

static int active_count(void) {
    int n = 0;
    for (int i = 0; i < WAGN0_MAX_TONES; i++) {
        if (_wagn0_tones[i].active) n++;
    }
    return n;
}

void draw() {
    background(BLACK);

    fill(WHITE);
    text("press 1-5 to play", 10, 10);
    text("1=A4  2=C5  3=E5  4=G5  5=noise", 10, 30);

    char buf[16];
    int n = active_count();
    int i = 0;
    if (n == 0) { buf[i++] = '0'; }
    else {
        int tmp = n;
        while (tmp > 0) { buf[i++] = '0' + (tmp % 10); tmp /= 10; }
        for (int j = 0; j < i/2; j++) {
            char t = buf[j]; buf[j] = buf[i-1-j]; buf[i-1-j] = t;
        }
    }
    buf[i] = 0;

    fill(GREEN);
    text("active:", 10, 60);
    fill(WHITE);
    text(buf, 80, 60);
}

void key_pressed(int key) {
    if (key == 0x1E) play_tone(440.0f, 0.5f, 0.4f);  // 1: A4
    if (key == 0x1F) play_tone(523.0f, 0.5f, 0.4f);  // 2: C5
    if (key == 0x20) play_tone(659.0f, 0.5f, 0.4f);  // 3: E5
    if (key == 0x21) play_tone(784.0f, 0.5f, 0.4f);  // 4: G5
    if (key == 0x22) play_noise(0.3f, 0.3f);          // 5: noise burst
}

void update(void) {}
void mouse_pressed(void) {}
void mouse_released(void) {}
void key_released(int key) { (void)key; }
