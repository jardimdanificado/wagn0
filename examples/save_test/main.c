#include "wagn0.h"

static int counter = 0;
static char status_text[128] = "Press SPACE to save counter to file";
static uint32_t last_save_time = 0;

void preload() {
    // We could load an existing save file here if we want!
    // But since Wagnostic host currently clears io_save and io_load simulates
    // reading from the TAR, we won't be able to actually read what we saved
    // unless the host supports writing to the TAR or a virtual FS.
    // For this test, we just test the `file_save` API.
}

void setup() {
    counter = 0;
}

void draw() {
    clear(screen, BLACK);
    
    // Simulate some game logic
    counter++;
    
    if (counter == 60) {
        key_pressed(0x2C);
    }
    
    if (file_is_saving()) {
        draw_text(screen, "Saving in progress...", 10, 10, YELLOW);
    } else {
        if (w_ticks - last_save_time < 2000 && last_save_time > 0) {
            draw_text(screen, status_text, 10, 10, GREEN);
        } else {
            draw_text(screen, status_text, 10, 10, WHITE);
        }
    }
    
    char counter_str[64];
    int n = counter;
    int i = 0;
    if (n == 0) { counter_str[i++] = '0'; }
    else {
        int tmp = n;
        while (tmp > 0) { counter_str[i++] = '0' + (tmp % 10); tmp /= 10; }
        for (int j = 0; j < i/2; j++) {
            char t = counter_str[j]; counter_str[j] = counter_str[i-1-j]; counter_str[i-1-j] = t;
        }
    }
    counter_str[i] = '\0';
    
    draw_text(screen, "Counter:", 10, 50, WHITE);
    draw_text(screen, counter_str, 80, 50, BLUE);
}

void key_pressed(int key) {
    if (key == 0x2C) { // SPACE
        if (!file_is_saving()) {
            // Save the counter value as a string
            char save_data[32];
            int n = counter;
            int i = 0;
            if (n == 0) { save_data[i++] = '0'; }
            else {
                int tmp = n;
                while (tmp > 0) { save_data[i++] = '0' + (tmp % 10); tmp /= 10; }
                for (int j = 0; j < i/2; j++) {
                    char t = save_data[j]; save_data[j] = save_data[i-1-j]; save_data[i-1-j] = t;
                }
            }
            save_data[i] = '\0';
            
            if (file_save("save.txt", save_data, i)) {
                last_save_time = w_ticks;
                // update status
                const char* msg = "Saved successfully!";
                int j = 0;
                while (msg[j]) { status_text[j] = msg[j]; j++; }
                status_text[j] = '\0';
            }
        }
    }
}
