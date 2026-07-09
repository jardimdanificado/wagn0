#include "wagn0.h"

static Canvas sheet;
static int sprite_w = 24, sprite_h = 24;

typedef struct { float x, y, vx, vy; int w, h; pixel_t tint; } Sprite;
#define SPRITE_COUNT 200
static Sprite sprites[SPRITE_COUNT];

void preload() {
    load_image(&sheet, "sprite.png");
}

void setup() {
    set_fps(60);
    random_seed(42);
    for (int i = 0; i < SPRITE_COUNT; i++) {
        sprites[i].w = sprite_w; 
        sprites[i].h = sprite_h;
        sprites[i].x = (float)(random_int(0, 320 - sprite_w));
        sprites[i].y = (float)(random_int(0, 240 - sprite_h));
        sprites[i].vx = (float)random_int(1, 4) * (random_int(0, 1) ? -1 : 1);
        sprites[i].vy = (float)random_int(1, 4) * (random_int(0, 1) ? -1 : 1);
        sprites[i].tint = lerp_color(CYAN, MAGENTA, (float)i / SPRITE_COUNT);
    }
}

static void draw_number(int n, int x, int y, pixel_t color) {
    char str[16];
    int i = 0;
    if (n == 0) { str[i++] = '0'; }
    else {
        int tmp = n;
        while (tmp > 0) { str[i++] = '0' + (tmp % 10); tmp /= 10; }
        for (int j = 0; j < i/2; j++) {
            char t = str[j]; str[j] = str[i-1-j]; str[i-1-j] = t;
        }
    }
    str[i] = '\0';
    push();
    translate(x, y);
    fill(color);
    draw_text(str);
    pop();
}

void draw() {
    // Background gradient
    for (int y = 0; y < 240; y += 4) {
        pixel_t col = lerp_color(rgb(10, 10, 30), rgb(30, 10, 40), (float)y / 240.0f);
        push();
        translate(0, y);
        scale(320, 4);
        fill(col);
        draw_quad();
        pop();
    }

    if (sheet.pixels) {
        for (int i = 0; i < SPRITE_COUNT; i++) {
            sprites[i].x += sprites[i].vx; 
            sprites[i].y += sprites[i].vy;
            if (sprites[i].x <= 0 || sprites[i].x + sprites[i].w >= 320) sprites[i].vx *= -1;
            if (sprites[i].y <= 0 || sprites[i].y + sprites[i].h >= 240) sprites[i].vy *= -1;
            
            push();
            translate(sprites[i].x, sprites[i].y);
            scale(sprite_w, sprite_h);
            
            // Draw a tinted rectangle behind the sprite for flavor
            fill(sprites[i].tint);
            draw_quad();
            
            // Draw sprite
            texture(&sheet);
            draw_quad();
            
            pop();
        }
    }

    // UI
    push();
    translate(5, 5);
    scale(70, 20);
    fill(rgb(0, 0, 0));
    stroke(GRAY);
    draw_quad();
    pop();

    push();
    translate(10, 10);
    fill(GRAY);
    draw_text("FPS:");
    pop();
    
    draw_number(wagn0.fps, 40, 10, WHITE);
    
    push();
    translate(90, 10);
    fill(WHITE);
    draw_text("200 Bouncing Sprites");
    pop();
}
