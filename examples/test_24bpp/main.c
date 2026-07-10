#include "wagner.h"

static Canvas img;

void preload() {
    load_image(&img, "test_card.png");
}

void draw() {
    clear(BLACK);
    
    push();
    translate(10, 10);
    fill(WHITE);
    text("24BPP MODE (RGB888)");
    pop();

    // Mostra a imagem centrada
    if (img.pixels) {
        push();
        translate((320 - img.width) / 2.0f, (240 - img.height) / 2.0f);
        scale(img.width, img.height);
        texture(&img);
        rect();
        pop();
    }

    // Barras de cor puras para verificar fidelidade RGB
    int bar_w = 320 / 6;
    int bar_y = 200;
    int bar_h = 30;
    uint32_t colors[6] = {
        0xFF0000FF, // Vermelho
        0xFF00FF00, // Verde
        0xFFFF0000, // Azul
        0xFFFFFF00, // Ciano
        0xFF00FFFF, // Amarelo
        0xFFFF00FF, // Magenta
    };
    for (int i = 0; i < 6; i++) {
        push();
        translate(i * bar_w, bar_y);
        scale(bar_w, bar_h);
        fill(colors[i]);
        rect();
        pop();
    }
}
