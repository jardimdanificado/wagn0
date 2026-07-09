#include "wagner.h"

static Canvas img = {0};

void preload() {
    load_image(&img, "sprite.png");
}

void setup() {
    set_fps(60);
}

void draw() {
    clear(rgb(20, 25, 30));
    
    push();
    translate(10, 10);
    fill(WHITE);
    text("Transform and Texture Demo");
    pop();
    
    if (!img.pixels) {
        push(); translate(10, 30); fill(RED); text("Loading image..."); pop();
        return;
    }
    
    // Rotating textured quad
    push();
    translate(80, 100);
    rotate(w_ticks * 0.002f);
    scale(60, 60);
    texture(&img);
    quad(); // Draws a textured quad rotated around its top-left, since translate is top-left in default state machine unless we translate to center first
    pop();
    
    // Rotating textured quad around its center
    push();
    translate(240, 100);
    rotate(-w_ticks * 0.003f);
    translate(-30, -30); // Offset half the size to center it
    scale(60, 60);
    texture(&img);
    quad();
    pop();
    
    // Textured circle (using texture mapped to a circle)
    push();
    translate(80, 200);
    scale(40, 40);
    texture(&img); // A circle will use the texture bound to the state!
    circle();
    pop();
    
    // Scaled, tinted and rotated quad
    push();
    translate(240, 200);
    rotate(w_ticks * 0.01f);
    translate(-40, -20);
    scale(80, 40);
    fill(rgb(255, 100, 100)); // Tint color (mixes with texture in olivec)
    texture(&img);
    quad();
    pop();
    
    // Text showing labels
    push(); translate(80 - 30, 140); fill(GRAY); text("Rotated Quad"); pop();
    push(); translate(240 - 40, 140); fill(GRAY); text("Centered Rotated"); pop();
    push(); translate(80 - 40, 230); fill(GRAY); text("Textured Circle"); pop();
    push(); translate(240 - 40, 230); fill(GRAY); text("Tinted & Scaled"); pop();
}
