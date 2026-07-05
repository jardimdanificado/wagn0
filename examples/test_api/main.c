/**
 * test_api — exercita todas as funções adicionadas no último fix do wagn0:
 *
 *   draw_circle / draw_circle_outline
 *   draw_rect_outline
 *   canvas_create  (offscreen buffer)
 *   lerp_color
 *   text_height
 *   dist / dist_sq
 *   random_seed / random_int
 *   audio_stop
 *
 * Controles:
 *   SPACE  — toca beep.wav
 *   S      — para o beep (audio_stop)
 *   R      — re-semente o RNG com ticks atual
 *   ↑↓     — muda seção exibida (0-4)
 */
#include "wagn0.h"

// ── Assets ───────────────────────────────────────────────────────────────────
static Wagn0Audio beep = {0};
static int        beep_playing = 0;

// ── State ────────────────────────────────────────────────────────────────────
static int   section  = 0;   // qual seção está visível (0-4)
#define NUM_SECTIONS 5

// offscreen canvas criado via canvas_create
static Canvas offscreen;

// para animar círculos
static float anim = 0.0f;

// ── Seção 0: draw_circle / draw_circle_outline ────────────────────────────────
static void draw_section0(void) {
    draw_text(screen, "0: draw_circle / outline", 4, 4, WHITE);

    // Círculo cheio animado
    int cx = 80, cy = 120;
    int r  = 40 + (int)(sin(anim) * 20.0f);
    draw_circle(screen, cx, cy, r, rgb(80, 180, 255));
    draw_circle_outline(screen, cx, cy, r + 4, WHITE);

    // Círculos concêntricos de cores interpoladas
    for (int i = 1; i <= 6; i++) {
        float t = (float)i / 6.0f;
        pixel_t col = lerp_color(rgb(255, 60, 60), rgb(60, 255, 120), t);
        draw_circle_outline(screen, 240, 120, i * 14, col);
    }

    draw_text(screen, "filled + outline", 140, 220, GRAY);
}

// ── Seção 1: draw_rect_outline ────────────────────────────────────────────────
static void draw_section1(void) {
    draw_text(screen, "1: draw_rect_outline", 4, 4, WHITE);

    // Grade de retângulos com borda colorida
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            int x = 20 + col * 72, y = 40 + row * 60;
            float t = ((float)(row * 4 + col)) / 11.0f;
            pixel_t fill    = lerp_color(rgb(30, 80, 200), rgb(200, 60, 30), t);
            pixel_t outline = lerp_color(WHITE, YELLOW, t);
            draw_rect(screen, x, y, 60, 50, fill);
            draw_rect_outline(screen, x, y, 60, 50, outline);
        }
    }

    draw_text(screen, "lerp_color nas bordas", 60, 225, GRAY);
}

// ── Seção 2: canvas_create (offscreen buffer) ────────────────────────────────
static void draw_section2(void) {
    draw_text(screen, "2: canvas_create offscreen", 4, 4, WHITE);

    if (!offscreen.pixels) {
        draw_text(screen, "offscreen nao alocado!", 40, 120, RED);
        return;
    }

    // Redesenha o offscreen a cada frame com um padrão dinâmico
    clear(offscreen, rgb(10, 10, 30));
    for (int i = 0; i < 8; i++) {
        float angle = anim + i * (TWO_PI / 8.0f);
        int ox = 64 + (int)(cos(angle) * 50.0f);
        int oy = 64 + (int)(sin(angle) * 50.0f);
        pixel_t col = lerp_color(CYAN, MAGENTA, (float)i / 8.0f);
        draw_circle(offscreen, ox, oy, 12, col);
    }
    draw_rect_outline(offscreen, 0, 0, offscreen.width, offscreen.height, WHITE);
    draw_text(offscreen, "offscreen", 10, 55, WHITE);

    // Cola o offscreen centrado na tela
    int dx = (screen.width  - offscreen.width)  / 2;
    int dy = (screen.height - offscreen.height) / 2;
    draw_canvas(screen, offscreen, dx, dy);

    // Mostra dimensões
    char info[64];
    info[0] = 'w'; info[1] = ':';
    int w = offscreen.width;
    info[2] = '0' + (w / 100) % 10;
    info[3] = '0' + (w /  10) % 10;
    info[4] = '0' + (w      ) % 10;
    info[5] = ' '; info[6] = 'h'; info[7] = ':';
    int h = offscreen.height;
    info[8]  = '0' + (h / 100) % 10;
    info[9]  = '0' + (h /  10) % 10;
    info[10] = '0' + (h      ) % 10;
    info[11] = '\0';
    draw_text(screen, info, 110, 210, GRAY);
}

// ── Seção 3: dist / dist_sq / text_height / random_seed ─────────────────────
static void draw_section3(void) {
    draw_text(screen, "3: dist / text_height / rng", 4, 4, WHITE);

    // Mostra text_height
    int th = text_height();
    draw_text(screen, "text_height()=6", 4, 22, CYAN);
    draw_rect_outline(screen, 3, 21, text_width("text_height()=6"), th, CYAN);

    // dist do mouse ao centro
    float cx = screen.width  / 2.0f;
    float cy = screen.height / 2.0f;
    float d  = dist(wagn0.mouse.x, wagn0.mouse.y, cx, cy);
    float dsq = dist_sq(wagn0.mouse.x, wagn0.mouse.y, cx, cy);

    draw_circle_outline(screen, (int)cx, (int)cy, (int)d, YELLOW);
    draw_circle(screen, (int)wagn0.mouse.x, (int)wagn0.mouse.y, 4, RED);

    // Imprime valores numéricos simples (sem sprintf)
    draw_text(screen, "dist  mouse->centro:", 4, 40, WHITE);
    // desenha barra proporcional
    int bar = (int)(d);
    if (bar > 300) bar = 300;
    draw_rect(screen, 4, 52, bar, 8, lerp_color(GREEN, RED, d / 200.0f));

    draw_text(screen, "dist_sq > dist:ok", 4, 68, GRAY);

    // random_seed demo: gera 20 pontos com seed fixa
    draw_text(screen, "rng seed=42:", 4, 90, WHITE);
    random_seed(42);
    for (int i = 0; i < 20; i++) {
        int rx = random_int(10, 310);
        int ry = random_int(100, 200);
        pixel_t rc = lerp_color(BLUE, ORANGE, (float)i / 20.0f);
        draw_circle(screen, rx, ry, 4, rc);
    }

    draw_text(screen, "[r] re-seed com ticks", 4, 210, GRAY);

    (void)dsq;
}

// ── Seção 4: audio_play / audio_stop ─────────────────────────────────────────
static void draw_section4(void) {
    draw_text(screen, "4: audio_play / audio_stop", 4, 4, WHITE);

    // Status do beep
    int playing = audio_is_playing();
    pixel_t status_col = playing ? GREEN : GRAY;
    draw_circle(screen, 160, 90, 40, playing ? rgb(30, 120, 30) : rgb(40, 40, 40));
    draw_circle_outline(screen, 160, 90, 40, status_col);
    draw_text(screen, playing ? "tocando" : "parado", 136, 86, status_col);

    // Instruções
    draw_rect_outline(screen, 60, 150, 80, 24, WHITE);
    draw_text(screen, "[space]", 72, 157, WHITE);
    draw_text(screen, "tocar", 78, 166, GRAY);

    draw_rect_outline(screen, 180, 150, 80, 24, WHITE);
    draw_text(screen, "[s]", 202, 157, WHITE);
    draw_text(screen, "parar", 198, 166, GRAY);

    // Carregamento
    if (!beep.samples) {
        draw_rect_outline(screen, 60, 195, 200, 14, RED);
        draw_text(screen, "beep.wav nao carregado", 64, 198, RED);
    }
}

// ── Callbacks ─────────────────────────────────────────────────────────────────
void preload(void) {
    load_audio(&beep, "beep.wav");
}

void setup(void) {
    set_fps(60);
    // Cria offscreen de 128x128 em 32bpp
    offscreen = canvas_create(128, 128, 32);
    // Seed inicial determinística
    random_seed(12345);
}

void draw(void) {
    anim += wagn0.delta_time * 2.0f;

    // ── HUD: barra de navegação ──
    clear(screen, rgb(10, 10, 20));

    // Título de cada seção
    switch (section) {
        case 0: draw_section0(); break;
        case 1: draw_section1(); break;
        case 2: draw_section2(); break;
        case 3: draw_section3(); break;
        case 4: draw_section4(); break;
    }

    // ── Rodapé de navegação ──
    draw_rect(screen, 0, 232, 320, 8, rgb(20, 20, 40));
    for (int i = 0; i < NUM_SECTIONS; i++) {
        pixel_t dot = (i == section) ? WHITE : rgb(80, 80, 100);
        draw_circle(screen, 140 + i * 10, 236, 2, dot);
    }
    draw_text(screen, "[up][dn]", 4, 233, rgb(60, 60, 80));
}

void key_pressed(int key) {
    // Navegação: cima=72/82, baixo=81/84 (SDL scancodes)
    if (key == 82 || key == 72) { // up
        section = (section - 1 + NUM_SECTIONS) % NUM_SECTIONS;
    }
    if (key == 81 || key == 75) { // down
        section = (section + 1) % NUM_SECTIONS;
    }
    // Audio
    if (key == 44 || key == ' ') { // space
        if (beep.samples) {
            audio_play(&beep);
            beep_playing = 1;
        }
    }
    if (key == 22 || key == 's') { // S
        audio_stop(&beep);
        beep_playing = 0;
    }
    // Re-seed com ticks
    if (key == 21 || key == 'r') {
        random_seed(w_ticks);
    }
}
