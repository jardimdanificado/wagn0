#ifndef WAGN0_H
#define WAGN0_H

/**
 * WagnO - Easy Game Development API for Wagnostic
 * 
 * Inspired by p5.js and LÖVE2D, this API provides a simple way to create
 * games and interactive applications for the Wagnostic WASM runtime.
 * 
 * Usage:
 *   #define WAGN0_IMPLEMENTATION
 *   #include "wagn0.h"
 * 
 *   void setup() { ... }
 *   void update() { ... }
 *   void draw() { ... }
 */

#include <stdint.h>
#include <stdbool.h>

// ============================================
// WAGNOSTIC COMPATIBILITY MACROS
// ============================================

#ifndef W_RGB565
#define W_RGB565(r, g, b) (uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))
#endif

#ifndef W_RGBA
#define W_RGBA(r, g, b, a) (uint32_t)(((a) << 24) | ((b) << 16) | ((g) << 8) | (r))
#endif

#ifndef W_RGB332
#define W_RGB332(r, g, b) (uint8_t)(((r) & 0xE0) | (((g) & 0xE0) >> 3) | (((b) & 0xC0) >> 6))
#endif

// ============================================
// CORE TYPES
// ============================================

typedef struct {
    float x, y;
} Vec2;

typedef struct {
    uint8_t r, g, b, a;
} Color;

// Rect is defined in wagnostic.h

// ============================================
// GLOBAL STATE (managed by WagnO)
// ============================================

static struct {
    // Screen
    int width;
    int height;
    int bpp;
    int scale;
    
    // Time
    float delta_time;
    uint32_t frame_count;
    uint32_t fps;
    
    // Input - Mouse
    Vec2 mouse;
    Vec2 pmouse;  // previous mouse
    bool mouse_pressed;
    bool mouse_released;
    bool mouse_down;
    int mouse_button;  // which button
    
    // Input - Keyboard
    bool keys[256];
    bool keys_pressed[256];
    bool keys_released[256];
    
    // Graphics
    Color fill_color;
    Color stroke_color;
    int stroke_weight;
    bool no_fill;
    bool no_stroke;
    
    // Canvas pointer (internal)
    void* canvas_pixels;
} wagn0;

// ============================================
// COLOR CONSTANTS
// ============================================

#define WAGN0_BLACK   ((Color){0, 0, 0, 255})
#define WAGN0_WHITE   ((Color){255, 255, 255, 255})
#define WAGN0_RED     ((Color){255, 0, 0, 255})
#define WAGN0_GREEN   ((Color){0, 255, 0, 255})
#define WAGN0_BLUE    ((Color){0, 0, 255, 255})
#define WAGN0_YELLOW  ((Color){255, 255, 0, 255})
#define WAGN0_CYAN    ((Color){0, 255, 255, 255})
#define WAGN0_MAGENTA ((Color){255, 0, 255, 255})
#define WAGN0_GRAY    ((Color){128, 128, 128, 255})
#define WAGN0_ORANGE  ((Color){255, 165, 0, 255})
#define WAGN0_PURPLE  ((Color){128, 0, 128, 255})

// ============================================
// MATH UTILITIES
// ============================================

#define WAGN0_PI 3.14159265358979f
#define WAGN0_TWO_PI 6.28318530717959f
#define WAGN0_HALF_PI 1.5707963267949f

static inline float map(float value, float start1, float stop1, float start2, float stop2) {
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

static inline float constrain(float value, float min_val, float max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

static inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

static inline float dist(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return dx * dx + dy * dy;  // squared distance for speed
}

static inline float sqrt(float x) {
    // Newton's method approximation
    if (x <= 0) return 0;
    float guess = x / 2.0f;
    for (int i = 0; i < 10; i++) {
        guess = (guess + x / guess) / 2.0f;
    }
    return guess;
}

static inline float abs(float x) {
    return x < 0 ? -x : x;
}

static inline float min(float a, float b) {
    return a < b ? a : b;
}

static inline float max(float a, float b) {
    return a > b ? a : b;
}

// Trigonometric functions using Bhaskara I approximation
static inline float sin(float x) {
    // Normalize to [0, 2π]
    while (x < 0) x += WAGN0_TWO_PI;
    while (x >= WAGN0_TWO_PI) x -= WAGN0_TWO_PI;
    
    if (x > WAGN0_PI) {
        float y = x - WAGN0_PI;
        return -16.0f * y * (WAGN0_PI - y) / (5.0f * WAGN0_PI * WAGN0_PI - 4.0f * y * (WAGN0_PI - y));
    }
    return 16.0f * x * (WAGN0_PI - x) / (5.0f * WAGN0_PI * WAGN0_PI - 4.0f * x * (WAGN0_PI - x));
}

static inline float cos(float x) {
    return sin(x + WAGN0_HALF_PI);
}

static inline int random_int(int min_val, int max_val) {
    // Simple LCG random
    static uint32_t seed = 12345;
    seed = seed * 1103515245 + 12345;
    return min_val + (seed >> 16) % (max_val - min_val + 1);
}

static inline float random(float min_val, float max_val) {
    return min_val + (float)random_int(0, 10000) / 10000.0f * (max_val - min_val);
}

// ============================================
// COLOR FUNCTIONS
// ============================================

static inline Color color_rgb(uint8_t r, uint8_t g, uint8_t b) {
    Color c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = 255;
    return c;
}

static inline Color color_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    Color c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}

static inline Color color_hex(uint32_t hex) {
    Color c;
    c.r = (hex >> 16) & 0xFF;
    c.g = (hex >> 8) & 0xFF;
    c.b = hex & 0xFF;
    c.a = 255;
    return c;
}

static inline uint16_t color_to_rgb565(Color c) {
    return W_RGB565(c.r, c.g, c.b);
}

// ============================================
// VECTOR FUNCTIONS
// ============================================

static inline Vec2 vec2(float x, float y) {
    return (Vec2){x, y};
}

static inline Vec2 vec2_add(Vec2 a, Vec2 b) {
    return (Vec2){a.x + b.x, a.y + b.y};
}

static inline Vec2 vec2_sub(Vec2 a, Vec2 b) {
    return (Vec2){a.x - b.x, a.y - b.y};
}

static inline Vec2 vec2_mul(Vec2 v, float s) {
    return (Vec2){v.x * s, v.y * s};
}

static inline float vec2_len(Vec2 v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

static inline Vec2 vec2_normalize(Vec2 v) {
    float len = vec2_len(v);
    if (len == 0) return (Vec2){0, 0};
    return (Vec2){v.x / len, v.y / len};
}

// ============================================
// DRAWING STATE FUNCTIONS
// ============================================

static inline void fill(Color c) {
    wagn0.fill_color = c;
    wagn0.no_fill = false;
}

static inline void no_fill() {
    wagn0.no_fill = true;
}

static inline void stroke(Color c) {
    wagn0.stroke_color = c;
    wagn0.no_stroke = false;
}

static inline void no_stroke() {
    wagn0.no_stroke = true;
}

static inline void stroke_weight(int w) {
    wagn0.stroke_weight = w;
}

// ============================================
// DRAWING PRIMITIVES (declared, implemented below)
// ============================================

void background(Color c);
void rect(int x, int y, int w, int h);
void rect_mode(int mode);  // 0=CORNER, 1=CENTER
void ellipse(int x, int y, int w, int h);
void line(int x1, int y1, int x2, int y2);
void point(int x, int y);
void triangle(int x1, int y1, int x2, int y2, int x3, int y3);
void quad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
void arc(int x, int y, int w, int h, float start, float stop);

// ============================================
// TEXT FUNCTIONS
// ============================================

void text(const char* text, int x, int y);
void text_size(int size);
int text_width(const char* text);

// ============================================
// IMAGE FUNCTIONS
// ============================================

typedef struct {
    void* pixels;
    int width;
    int height;
    int bpp;  // 8, 16, or 32
} Wagn0Image;

Wagn0Image create_image(int width, int height, int bpp);
Wagn0Image create_image_from_data(const void* data, int width, int height, int bpp);
void image(Wagn0Image img, int x, int y);
void image_scaled(Wagn0Image img, int x, int y, int w, int h);
void load_image(Wagn0Image* img, const void* data, int width, int height, int bpp);

// ============================================
// AUDIO FUNCTIONS
// ============================================

void play_tone(float freq, float duration, float volume);
void play_noise(float duration, float volume);

// ============================================
// USER FUNCTIONS (must be implemented by user)
// ============================================

void setup(void);
void update(void);
void draw(void);

// Optional user functions
void mouse_pressed(void);
void mouse_released(void);
void key_pressed(int key);
void key_released(int key);

// ============================================
// IMPLEMENTATION
// ============================================

#ifndef WAGNOSTIC_IMPLEMENTATION
#define WAGNOSTIC_IMPLEMENTATION
#endif
#include "wagnostic.h"
#define OLIVEC_IMPLEMENTATION
#include "olive.c"

static int _wagn0_rect_mode = 0;  // 0=CORNER, 1=CENTER

// Internal drawing functions
static inline void _wagn0_set_pixel(int x, int y, Color c) {
    if (x < 0 || x >= wagn0.width || y < 0 || y >= wagn0.height) return;
    uint16_t* pixels = (uint16_t*)wagn0.canvas_pixels;
    pixels[y * wagn0.width + x] = color_to_rgb565(c);
}

static void _wagn0_draw_filled_rect(int x, int y, int w, int h, Color c) {
    for (int iy = y; iy < y + h; iy++) {
        for (int ix = x; ix < x + w; ix++) {
            _wagn0_set_pixel(ix, iy, c);
        }
    }
}

static void _wagn0_draw_rect_outline(int x, int y, int w, int h, Color c, int weight) {
    for (int i = 0; i < weight; i++) {
        // Top
        for (int ix = x + i; ix < x + w - i; ix++) {
            _wagn0_set_pixel(ix, y + i, c);
        }
        // Bottom
        for (int ix = x + i; ix < x + w - i; ix++) {
            _wagn0_set_pixel(ix, y + h - 1 - i, c);
        }
        // Left
        for (int iy = y + i; iy < y + h - i; iy++) {
            _wagn0_set_pixel(x + i, iy, c);
        }
        // Right
        for (int iy = y + i; iy < y + h - i; iy++) {
            _wagn0_set_pixel(x + w - 1 - i, iy, c);
        }
    }
}

// ============================================
// DRAWING PRIMITIVES IMPLEMENTATION
// ============================================

void background(Color c) {
    _wagn0_draw_filled_rect(0, 0, wagn0.width, wagn0.height, c);
}

void rect(int x, int y, int w, int h) {
    if (_wagn0_rect_mode == 1) {  // CENTER
        x -= w / 2;
        y -= h / 2;
    }
    
    if (!wagn0.no_fill) {
        _wagn0_draw_filled_rect(x, y, w, h, wagn0.fill_color);
    }
    if (!wagn0.no_stroke) {
        _wagn0_draw_rect_outline(x, y, w, h, wagn0.stroke_color, wagn0.stroke_weight);
    }
}

void rect_mode(int mode) {
    _wagn0_rect_mode = mode;
}

void ellipse(int x, int y, int w, int h) {
    int rx = w / 2;
    int ry = h / 2;
    
    // Draw filled ellipse
    for (int iy = -ry; iy <= ry; iy++) {
        for (int ix = -rx; ix <= rx; ix++) {
            float nx = (float)ix / rx;
            float ny = (float)iy / ry;
            if (nx * nx + ny * ny <= 1.0f) {
                if (!wagn0.no_fill) {
                    _wagn0_set_pixel(x + ix, y + iy, wagn0.fill_color);
                }
            }
        }
    }
    
    if (!wagn0.no_stroke) {
        // Draw outline
        for (int angle = 0; angle < 360; angle++) {
            float rad = angle * WAGN0_PI / 180.0f;
            int px = x + (int)(rx * cos(rad));
            int py = y + (int)(ry * sin(rad));
            _wagn0_set_pixel(px, py, wagn0.stroke_color);
        }
    }
}

void line(int x1, int y1, int x2, int y2) {
    // Bresenham's line algorithm
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = dx > dy ? dx : dy;
    if (steps == 0) steps = 1;
    
    float x_inc = (float)dx / steps;
    float y_inc = (float)dy / steps;
    
    float x = x1;
    float y = y1;
    
    for (int i = 0; i <= steps; i++) {
        _wagn0_set_pixel((int)x, (int)y, wagn0.stroke_color);
        x += x_inc;
        y += y_inc;
    }
}

void point(int x, int y) {
    _wagn0_set_pixel(x, y, wagn0.stroke_color);
}

void triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    if (!wagn0.no_fill) {
        // Scanline fill
        int min_y = y1 < y2 ? (y1 < y3 ? y1 : y3) : (y2 < y3 ? y2 : y3);
        int max_y = y1 > y2 ? (y1 > y3 ? y1 : y3) : (y2 > y3 ? y2 : y3);
        
        for (int y = min_y; y <= max_y; y++) {
            int x_min = wagn0.width, x_max = 0;
            
            // Check edges with zero-division protection
            if (y >= y1 && y <= y2 && y2 != y1) {
                int x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
                if (x < x_min) x_min = x;
                if (x > x_max) x_max = x;
            }
            if (y >= y2 && y <= y3 && y3 != y2) {
                int x = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
                if (x < x_min) x_min = x;
                if (x > x_max) x_max = x;
            }
            if (y >= y3 && y <= y1 && y1 != y3) {
                int x = x3 + (x1 - x3) * (y - y3) / (y1 - y3);
                if (x < x_min) x_min = x;
                if (x > x_max) x_max = x;
            }
            
            for (int x = x_min; x <= x_max; x++) {
                _wagn0_set_pixel(x, y, wagn0.fill_color);
            }
        }
    }
    
    if (!wagn0.no_stroke) {
        line(x1, y1, x2, y2);
        line(x2, y2, x3, y3);
        line(x3, y3, x1, y1);
    }
}

void quad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    if (!wagn0.no_stroke) {
        line(x1, y1, x2, y2);
        line(x2, y2, x3, y3);
        line(x3, y3, x4, y4);
        line(x4, y4, x1, y1);
    }
}

void arc(int x, int y, int w, int h, float start, float stop) {
    // Simple arc implementation
    int rx = w / 2;
    int ry = h / 2;
    
    for (float angle = start; angle < stop; angle += 0.01f) {
        int px = x + (int)(rx * cos(angle));
        int py = y + (int)(ry * sin(angle));
        _wagn0_set_pixel(px, py, wagn0.stroke_color);
    }
}

// ============================================
// TEXT FUNCTIONS IMPLEMENTATION
// ============================================

void text(const char* text, int x, int y) {
    // Simple bitmap font rendering
    // For now, just draw a placeholder rectangle for each character
    int char_width = 6;
    int char_height = 8;
    
    for (int i = 0; text[i] != '\0'; i++) {
        int cx = x + i * char_width;
        if (!wagn0.no_fill) {
            _wagn0_draw_filled_rect(cx, y, char_width - 1, char_height - 1, wagn0.fill_color);
        }
    }
}

void text_size(int size) {
    // Placeholder - would need actual font support
}

int text_width(const char* text) {
    int len = 0;
    while (text[len]) len++;
    return len * 6;  // assuming 6px per character
}

// ============================================
// IMAGE FUNCTIONS IMPLEMENTATION
// ============================================

Wagn0Image create_image(int width, int height, int bpp) {
    Wagn0Image img;
    img.width = width;
    img.height = height;
    img.bpp = bpp;
    img.pixels = NULL;
    return img;
}

Wagn0Image create_image_from_data(const void* data, int width, int height, int bpp) {
    Wagn0Image img;
    img.width = width;
    img.height = height;
    img.bpp = bpp;
    img.pixels = (void*)data;
    return img;
}

void image(Wagn0Image img, int x, int y) {
    if (!img.pixels) return;
    
    // Draw image pixel by pixel
    for (int iy = 0; iy < img.height; iy++) {
        for (int ix = 0; ix < img.width; ix++) {
            int px = x + ix;
            int py = y + iy;
            
            if (px < 0 || px >= wagn0.width || py < 0 || py >= wagn0.height) continue;
            
            Color c;
            if (img.bpp == 32) {
                uint32_t* pixels = (uint32_t*)img.pixels;
                uint32_t p = pixels[iy * img.width + ix];
                c.r = p & 0xFF;
                c.g = (p >> 8) & 0xFF;
                c.b = (p >> 16) & 0xFF;
                c.a = (p >> 24) & 0xFF;
            } else if (img.bpp == 16) {
                uint16_t* pixels = (uint16_t*)img.pixels;
                uint16_t p = pixels[iy * img.width + ix];
                c.r = ((p >> 11) & 0x1F) * 255 / 31;
                c.g = ((p >> 5) & 0x3F) * 255 / 63;
                c.b = (p & 0x1F) * 255 / 31;
                c.a = 255;
            } else {
                uint8_t* pixels = (uint8_t*)img.pixels;
                uint8_t p = pixels[iy * img.width + ix];
                c.r = ((p >> 5) & 0x07) * 255 / 7;
                c.g = ((p >> 2) & 0x07) * 255 / 7;
                c.b = (p & 0x03) * 255 / 3;
                c.a = 255;
            }
            
            if (c.a > 128) {  // Simple alpha test
                _wagn0_set_pixel(px, py, c);
            }
        }
    }
}

void image_scaled(Wagn0Image img, int x, int y, int w, int h) {
    if (!img.pixels) return;
    
    // Simple nearest-neighbor scaling
    for (int iy = 0; iy < h; iy++) {
        for (int ix = 0; ix < w; ix++) {
            int src_x = ix * img.width / w;
            int src_y = iy * img.height / h;
            
            int px = x + ix;
            int py = y + iy;
            
            if (px < 0 || px >= wagn0.width || py < 0 || py >= wagn0.height) continue;
            
            Color c;
            if (img.bpp == 32) {
                uint32_t* pixels = (uint32_t*)img.pixels;
                uint32_t p = pixels[src_y * img.width + src_x];
                c.r = p & 0xFF;
                c.g = (p >> 8) & 0xFF;
                c.b = (p >> 16) & 0xFF;
                c.a = (p >> 24) & 0xFF;
            } else if (img.bpp == 16) {
                uint16_t* pixels = (uint16_t*)img.pixels;
                uint16_t p = pixels[src_y * img.width + src_x];
                c.r = ((p >> 11) & 0x1F) * 255 / 31;
                c.g = ((p >> 5) & 0x3F) * 255 / 63;
                c.b = (p & 0x1F) * 255 / 31;
                c.a = 255;
            } else {
                uint8_t* pixels = (uint8_t*)img.pixels;
                uint8_t p = pixels[src_y * img.width + src_x];
                c.r = ((p >> 5) & 0x07) * 255 / 7;
                c.g = ((p >> 2) & 0x07) * 255 / 7;
                c.b = (p & 0x03) * 255 / 3;
                c.a = 255;
            }
            
            if (c.a > 128) {
                _wagn0_set_pixel(px, py, c);
            }
        }
    }
}

// ============================================
// AUDIO FUNCTIONS (placeholders)
// ============================================

void play_tone(float freq, float duration, float volume) {
    // Would need to write to audio buffer
    // Placeholder for now
}

void play_noise(float duration, float volume) {
    // Placeholder
}

// ============================================
// MAIN WAGNO FUNCTIONS
// ============================================

int wupdate() {
    // One-time initialization
    static int init = 0;
    if (!init) {
        init = 1;
        wagn0.width  = 320;
        wagn0.height = 240;
        wagn0.bpp    = 16;
        wagn0.scale  = 4;
        wagn0.frame_count  = 0;
        wagn0.fps    = 0;
        wagn0.delta_time = 0.016f;
        wagn0.fill_color   = WAGN0_WHITE;
        wagn0.stroke_color = WAGN0_BLACK;
        wagn0.stroke_weight = 1;
        wagn0.no_fill   = false;
        wagn0.no_stroke = false;
        wagn0.mouse  = vec2(0, 0);
        wagn0.pmouse = vec2(0, 0);
        wagn0.mouse_pressed  = false;
        wagn0.mouse_released = false;
        wagn0.mouse_down     = false;
        wagn0.canvas_pixels = w_vram;
        w_setup("WagnO Game", wagn0.width, wagn0.height, wagn0.bpp, wagn0.scale, 0);
        setup();
    }
    // Update time
    static uint32_t last_ticks = 0;
    uint32_t current_ticks = w_ticks;
    if (last_ticks > 0) {
        wagn0.delta_time = (current_ticks - last_ticks) / 1000.0f;
    }
    last_ticks = current_ticks;
    wagn0.frame_count++;
    
    // Update input state
    wagn0.pmouse = wagn0.mouse;
    wagn0.mouse = vec2(w_mouse_x, w_mouse_y);
    
    // Detect mouse press/release
    bool current_mouse_down = (w_mouse_buttons & 1) != 0;
    wagn0.mouse_pressed = current_mouse_down && !wagn0.mouse_down;
    wagn0.mouse_released = !current_mouse_down && wagn0.mouse_down;
    wagn0.mouse_down = current_mouse_down;
    wagn0.mouse_button = current_mouse_down ? 1 : 0;
    
    // Detect key press/release
    for (int i = 0; i < 256; i++) {
        bool current_key = w_keys[i] != 0;
        wagn0.keys_pressed[i] = current_key && !wagn0.keys[i];
        wagn0.keys_released[i] = !current_key && wagn0.keys[i];
        wagn0.keys[i] = current_key;
    }
    
    // Call user functions
    update();
    
    // Clear drawing state
    wagn0.no_fill = false;
    wagn0.no_stroke = false;
    wagn0.stroke_weight = 1;
    
    // Call user draw
    draw();
    
    // Handle user callbacks
    if (wagn0.mouse_pressed) mouse_pressed();
    if (wagn0.mouse_released) mouse_released();
    
    for (int i = 0; i < 256; i++) {
        if (wagn0.keys_pressed[i]) key_pressed(i);
        if (wagn0.keys_released[i]) key_released(i);
    }
    
    w_redraw();
    return 1;
}

#endif // WAGN0_H