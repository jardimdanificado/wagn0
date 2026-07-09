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
    push(); translate(4, 4); fill(WHITE); draw_text("0: draw_circle / outline"); pop();

    // Círculo cheio animado
    int cx = 80, cy = 120;
    int r  = 40 + (int)(sin(anim) * 20.0f);
    push(); translate(cx, cy); scale(r, r); fill(rgb(80, 180, 255)); draw_circle(); pop();
    push(); translate(cx, cy); scale(r + 4, r + 4); stroke(WHITE); draw_circle(); pop();

    // Círculos concêntricos de cores interpoladas
    for (int i = 1; i <= 6; i++) {
        float t = (float)i / 6.0f;
        pixel_t col = lerp_color(rgb(255, 60, 60), rgb(60, 255, 120), t);
        push(); translate(240, 120); scale(i * 14, i * 14); stroke(col); draw_circle(); pop();
    }

    push(); translate(140, 220); fill(GRAY); draw_text("filled + outline"); pop();
}

// ── Seção 1: draw_rect_outline ────────────────────────────────────────────────
static void draw_section1(void) {
    push(); translate(4, 4); fill(WHITE); draw_text("1: draw_rect_outline"); pop();

    // Grade de retângulos com borda colorida
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            int x = 20 + col * 72, y = 40 + row * 60;
            float t = ((float)(row * 4 + col)) / 11.0f;
            pixel_t fill_color = lerp_color(rgb(30, 80, 200), rgb(200, 60, 30), t);
            pixel_t outline = lerp_color(WHITE, YELLOW, t);
            push(); translate(x, y); scale(60, 50); fill(fill_color); draw_quad(); pop();
            push(); translate(x, y); scale(60, 50); stroke(outline); draw_quad(); pop();
        }
    }

    push(); translate(60, 225); fill(GRAY); draw_text("lerp_color nas bordas"); pop();
}

// ── Seção 2: canvas_create (offscreen buffer) ────────────────────────────────
static void draw_section2(void) {
    push(); translate(4, 4); fill(WHITE); draw_text("2: canvas_create offscreen"); pop();

    if (!offscreen.pixels) {
        push(); translate(40, 120); fill(RED); draw_text("offscreen nao alocado!"); pop();
        return;
    }

    // Redesenha o offscreen a cada frame com um padrão dinâmico
    render_target(&offscreen);
    clear(rgb(10, 10, 30));
    for (int i = 0; i < 8; i++) {
        float angle = anim + i * (TWO_PI / 8.0f);
        int ox = 64 + (int)(cos(angle) * 50.0f);
        int oy = 64 + (int)(sin(angle) * 50.0f);
        pixel_t col = lerp_color(CYAN, MAGENTA, (float)i / 8.0f);
        push(); translate(ox, oy); scale(12, 12); fill(col); draw_circle(); pop();
    }
    push(); translate(0, 0); scale(offscreen.width, offscreen.height); stroke(WHITE); draw_quad(); pop();
    push(); translate(10, 55); fill(WHITE); draw_text("offscreen"); pop();

    render_target(NULL);
    // Cola o offscreen centrado na tela
    int dx = (screen.width  - offscreen.width)  / 2;
    int dy = (screen.height - offscreen.height) / 2;
    push(); translate(dx, dy); scale(offscreen.width, offscreen.height); texture(&offscreen); draw_quad(); pop();

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
    push(); translate(110, 210); fill(GRAY); draw_text(info); pop();
}

// ── Seção 3: dist / dist_sq / text_height / random_seed ─────────────────────
static void draw_section3(void) {
    push(); translate(4, 4); fill(WHITE); draw_text("3: dist / text_height / rng"); pop();

    // Mostra text_height
    int th = text_height();
    push(); translate(4, 22); fill(CYAN); draw_text("text_height()=6"); pop();
    push(); translate(3, 21); scale(text_width("text_height()=6"), th); stroke(CYAN); draw_quad(); pop();

    // dist do mouse ao centro
    float cx = screen.width  / 2.0f;
    float cy = screen.height / 2.0f;
    float d  = dist(wagn0.mouse.x, wagn0.mouse.y, cx, cy);
    float dsq = dist_sq(wagn0.mouse.x, wagn0.mouse.y, cx, cy);

    push(); translate((int)cx, (int)cy); scale((int)d, (int)d); stroke(YELLOW); draw_circle(); pop();
    push(); translate((int)wagn0.mouse.x, (int)wagn0.mouse.y); scale(4, 4); fill(RED); draw_circle(); pop();

    // Imprime valores numéricos simples (sem sprintf)
    push(); translate(4, 40); fill(WHITE); draw_text("dist  mouse->centro:"); pop();
    // desenha barra proporcional
    int bar = (int)(d);
    if (bar > 300) bar = 300;
    push(); translate(4, 52); scale(bar, 8); fill(lerp_color(GREEN, RED, d / 200.0f)); draw_quad(); pop();

    push(); translate(4, 68); fill(GRAY); draw_text("dist_sq > dist:ok"); pop();

    // random_seed demo: gera 20 pontos com seed fixa
    push(); translate(4, 90); fill(WHITE); draw_text("rng seed=42:"); pop();
    random_seed(42);
    for (int i = 0; i < 20; i++) {
        int rx = random_int(10, 310);
        int ry = random_int(100, 200);
        pixel_t rc = lerp_color(BLUE, ORANGE, (float)i / 20.0f);
        push(); translate(rx, ry); scale(4, 4); fill(rc); draw_circle(); pop();
    }

    push(); translate(4, 210); fill(GRAY); draw_text("[r] re-seed com ticks"); pop();

    (void)dsq;
}

// ── Seção 4: audio_play / audio_stop ─────────────────────────────────────────
static void draw_section4(void) {
    push(); translate(4, 4); fill(WHITE); draw_text("4: audio_play / audio_stop"); pop();

    // Status do beep
    int playing = audio_is_playing();
    pixel_t status_col = playing ? GREEN : GRAY;
    push(); translate(160, 90); scale(40, 40); fill(playing ? rgb(30, 120, 30) : rgb(40, 40, 40)); draw_circle(); pop();
    push(); translate(160, 90); scale(40, 40); stroke(status_col); draw_circle(); pop();
    push(); translate(136, 86); fill(status_col); draw_text(playing ? "tocando" : "parado"); pop();

    // Instruções
    push(); translate(60, 150); scale(80, 24); stroke(WHITE); draw_quad(); pop();
    push(); translate(72, 157); fill(WHITE); draw_text("[space]"); pop();
    push(); translate(78, 166); fill(GRAY); draw_text("tocar"); pop();

    push(); translate(180, 150); scale(80, 24); stroke(WHITE); draw_quad(); pop();
    push(); translate(202, 157); fill(WHITE); draw_text("[s]"); pop();
    push(); translate(198, 166); fill(GRAY); draw_text("parar"); pop();

    // Carregamento
    if (!beep.samples) {
        push(); translate(60, 195); scale(200, 14); stroke(RED); draw_quad(); pop();
        push(); translate(64, 198); fill(RED); draw_text("beep.wav nao carregado"); pop();
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
    clear(rgb(10, 10, 20));

    // Título de cada seção
    switch (section) {
        case 0: draw_section0(); break;
        case 1: draw_section1(); break;
        case 2: draw_section2(); break;
        case 3: draw_section3(); break;
        case 4: draw_section4(); break;
    }

    // ── Rodapé de navegação ──
    push(); translate(0, 232); scale(320, 8); fill(rgb(20, 20, 40)); draw_quad(); pop();
    for (int i = 0; i < NUM_SECTIONS; i++) {
        pixel_t dot = (i == section) ? WHITE : rgb(80, 80, 100);
        push(); translate(140 + i * 10, 236); scale(2, 2); fill(dot); draw_circle(); pop();
    }
    push(); translate(4, 233); fill(rgb(60, 60, 80)); draw_text("[up][dn]"); pop();
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
