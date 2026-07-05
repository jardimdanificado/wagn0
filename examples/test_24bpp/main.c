#include "wagn0.h"

static Canvas img;

void preload() {
    load_image(&img, "test_card.png");
}

void draw() {
    clear(screen, BLACK);
    draw_text(screen, "24BPP MODE (RGB888)", 10, 10, WHITE);

    // Mostra a imagem centrada
    if (img.pixels)
        draw_canvas(screen, img, (320 - img.width) / 2, (240 - img.height) / 2);

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
        draw_rect(screen, i * bar_w, bar_y, bar_w, bar_h, colors[i]);
    }
}
