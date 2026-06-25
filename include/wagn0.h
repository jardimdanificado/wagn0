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

// Audio API below needs w_audio_* globals, so include wagnostic.h here.
#define WAGNOSTIC_IMPLEMENTATION
#include "wagnostic.h"

// ============================================
// BIT DEPTH CONFIGURATION
// ============================================
// Define WAGN0_BPP before including to change pixel format:
//   #define WAGN0_BPP 8   // RGB332  (uint8)
//   #define WAGN0_BPP 16  // RGB565  (uint16) — default
//   #define WAGN0_BPP 32  // RGBA8888 (uint32)

#ifndef WAGN0_BPP
#define WAGN0_BPP 16
#endif

#if WAGN0_BPP == 8
    typedef uint8_t pixel_t;
#elif WAGN0_BPP == 16
    typedef uint16_t pixel_t;
#elif WAGN0_BPP == 32
    typedef uint32_t pixel_t;
#else
    #error "WAGN0_BPP must be 8, 16, or 32"
#endif

// ============================================
// COLOR CONVERSION (input → pixel_t)
// ============================================

static inline pixel_t rgb(uint8_t r, uint8_t g, uint8_t b) {
#if WAGN0_BPP == 8
    return (pixel_t)(((r) & 0xE0) | (((g) & 0xE0) >> 3) | ((b) & 0xC0) >> 6);
#elif WAGN0_BPP == 16
    return (pixel_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3));
#else
    return (pixel_t)((0xFF << 24) | ((b) << 16) | ((g) << 8) | (r));
#endif
}

static inline pixel_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
#if WAGN0_BPP == 32
    return (pixel_t)(((a) << 24) | ((b) << 16) | ((g) << 8) | (r));
#else
    return rgb(r, g, b);  // ignore alpha in 8/16bpp
#endif
}

// Predefined colors — always pixel_t
#define BLACK   rgb(0,0,0)
#define WHITE   rgb(255,255,255)
#define RED     rgb(255,0,0)
#define GREEN   rgb(0,255,0)
#define BLUE    rgb(0,0,255)
#define YELLOW  rgb(255,255,0)
#define CYAN    rgb(0,255,255)
#define MAGENTA rgb(255,0,255)
#define GRAY    rgb(128,128,128)
#define ORANGE  rgb(255,165,0)
#define PURPLE  rgb(128,0,128)

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
    pixel_t fill_color;
    pixel_t stroke_color;
    int stroke_weight;
    bool no_fill;
    bool no_stroke;
    
    // Canvas pointer (internal)
    void* canvas_pixels;
} wagn0;

// ============================================
// MATH CONSTANTS
// ============================================

#define PI 3.14159265358979f
#define TWO_PI 6.28318530717959f
#define HALF_PI 1.5707963267949f

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
    while (x < 0) x += TWO_PI;
    while (x >= TWO_PI) x -= TWO_PI;
    
    if (x > PI) {
        float y = x - PI;
        return -16.0f * y * (PI - y) / (5.0f * PI * PI - 4.0f * y * (PI - y));
    }
    return 16.0f * x * (PI - x) / (5.0f * PI * PI - 4.0f * x * (PI - x));
}

static inline float cos(float x) {
    return sin(x + HALF_PI);
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
// COLOR HELPERS (return pixel_t, BPP-aware)
// ============================================

static inline pixel_t hex(uint32_t h) {
    return rgb((h>>16)&0xFF, (h>>8)&0xFF, h&0xFF);
}

static inline pixel_t hexa(uint32_t h) {
    return rgba((h>>24)&0xFF, (h>>16)&0xFF, (h>>8)&0xFF, h&0xFF);
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

static inline void fill(pixel_t c) {
    wagn0.fill_color = c;
    wagn0.no_fill = false;
}

static inline void no_fill() {
    wagn0.no_fill = true;
}

static inline void stroke(pixel_t c) {
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

void background(pixel_t c);
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
void stop_all_sounds(void);

// Default fill_audio implementation — generates PCM from the tone
// queue into the Wagnostic audio ring buffer. Weak so user code can
// override (e.g. to stream embedded PCM data instead of synthesizing).
__attribute__((weak)) void fill_audio(void);

#define WAGN0_MAX_TONES 8
#define WAGN0_AUDIO_SAMPLE_RATE 22050
#define WAGN0_AUDIO_FADE_SAMPLES 220  // 10ms at 22050Hz

typedef struct {
    uint8_t active;
    float freq;            // 0 = white noise
    float volume;          // 0.0..1.0
    uint32_t start_sample; // global sample count when triggered
    uint32_t duration_samples;
} Wagn0Tone;

static Wagn0Tone _wagn0_tones[WAGN0_MAX_TONES];
static uint32_t _wagn0_audio_sample = 0;
static uint8_t  _wagn0_audio_init = 0;
static int16_t  _wagn0_sin_lut[256];

static void _wagn0_init_sin_lut(void) {
    for (int i = 0; i < 256; i++) {
        _wagn0_sin_lut[i] = (int16_t)(sin(i * TWO_PI / 256.0f) * 32767.0f);
    }
}

void play_tone(float freq, float duration, float volume) {
    for (int i = 0; i < WAGN0_MAX_TONES; i++) {
        if (!_wagn0_tones[i].active) {
            _wagn0_tones[i].active = 1;
            _wagn0_tones[i].freq = freq;
            _wagn0_tones[i].volume = volume;
            _wagn0_tones[i].start_sample = _wagn0_audio_sample;
            _wagn0_tones[i].duration_samples =
                (uint32_t)(duration * (float)WAGN0_AUDIO_SAMPLE_RATE);
            return;
        }
    }
}

void play_noise(float duration, float volume) {
    play_tone(0.0f, duration, volume);
}

void stop_all_sounds(void) {
    for (int i = 0; i < WAGN0_MAX_TONES; i++) {
        _wagn0_tones[i].active = 0;
    }
}

#ifndef WAGN0_CUSTOM_FILL_AUDIO
__attribute__((weak)) void fill_audio(void) {
    if (!_wagn0_audio_init) {
        _wagn0_init_sin_lut();
        w_audio_size         = sizeof(w_audio_buffer);
        w_audio_sample_rate  = WAGN0_AUDIO_SAMPLE_RATE;
        w_audio_bpp          = 2;        // s16
        w_audio_channels     = 1;        // mono
        w_audio_write        = 0;
        w_audio_read         = 0;
        _wagn0_audio_init    = 1;
    }

    uint32_t w    = w_audio_write;
    uint32_t r    = w_audio_read;
    uint32_t size = w_audio_size;

    // Usable bytes = size - 1 (full vs empty distinction)
    uint32_t occupied = (w >= r) ? (w - r) : (size - r + w);
    if (occupied >= size - 1) return;

    // ~10ms of audio per fill_audio call
    uint32_t bytes_to_write = WAGN0_AUDIO_FADE_SAMPLES * 2;  // s16
    if (bytes_to_write > (size - 1 - occupied)) {
        bytes_to_write = size - 1 - occupied;
    }
    if (bytes_to_write < 2) return;
    uint32_t samples = bytes_to_write / 2;

    int16_t* buf = (int16_t*)w_audio_buffer;
    static uint32_t noise_seed = 0x12345;

    for (uint32_t i = 0; i < samples; i++) {
        int32_t mix = 0;
        uint32_t gs = _wagn0_audio_sample + i;

        for (int t = 0; t < WAGN0_MAX_TONES; t++) {
            if (!_wagn0_tones[t].active) continue;
            uint32_t elapsed = gs - _wagn0_tones[t].start_sample;
            if (elapsed >= _wagn0_tones[t].duration_samples) {
                _wagn0_tones[t].active = 0;
                continue;
            }

            float s;
            if (_wagn0_tones[t].freq == 0.0f) {
                noise_seed = noise_seed * 1103515245u + 12345u;
                s = (int16_t)(noise_seed >> 16) / 32768.0f;
            } else {
                float phase = _wagn0_tones[t].freq * (float)gs
                              / (float)WAGN0_AUDIO_SAMPLE_RATE;
                uint8_t idx = (uint8_t)(phase * 256.0f);
                s = _wagn0_sin_lut[idx] / 32768.0f;
            }

            // 10ms fade in/out to avoid pops
            float fade = 1.0f;
            uint32_t dur = _wagn0_tones[t].duration_samples;
            if (elapsed < WAGN0_AUDIO_FADE_SAMPLES) {
                fade = (float)elapsed / (float)WAGN0_AUDIO_FADE_SAMPLES;
            } else if (elapsed > dur - WAGN0_AUDIO_FADE_SAMPLES) {
                fade = (float)(dur - elapsed) / (float)WAGN0_AUDIO_FADE_SAMPLES;
            }

            mix += (int32_t)(s * _wagn0_tones[t].volume * fade * 32767.0f);
        }

        if (mix >  32767) mix =  32767;
        if (mix < -32768) mix = -32768;
        buf[w / 2] = (int16_t)mix;
        w = (w + 2) % size;
    }

    w_audio_write = w;
    _wagn0_audio_sample += samples;
}
#endif // !WAGN0_CUSTOM_FILL_AUDIO

// ============================================
// USER FUNCTIONS (implemented by user)
// ============================================

void setup(void);
void update(void);
void draw(void);
void mouse_pressed(void);
void mouse_released(void);
void key_pressed(int key);
void key_released(int key);

// ============================================
// IMPLEMENTATION
// ============================================

#define OLIVEC_IMPLEMENTATION
#include "olive.c"

// Weak no-op defaults for user callbacks. Prevents the compiler from
// emitting `env.update` etc. as unresolved imports (the native runner
// can't satisfy them). Define WAGN0_NO_DEFAULT_CALLBACKS before
// `#include "wagn0.h"` to opt out when providing your own.
#ifndef WAGN0_NO_DEFAULT_CALLBACKS
__attribute__((weak)) void update(void) {}
__attribute__((weak)) void mouse_pressed(void) {}
__attribute__((weak)) void mouse_released(void) {}
__attribute__((weak)) void key_pressed(int key) { (void)key; }
__attribute__((weak)) void key_released(int key) { (void)key; }
#endif

static int _wagn0_rect_mode = 0;  // 0=CORNER, 1=CENTER

// Internal drawing functions
static inline void _wagn0_set_pixel(int x, int y, pixel_t c) {
    if (x < 0 || x >= wagn0.width || y < 0 || y >= wagn0.height) return;
    pixel_t* pixels = (pixel_t*)wagn0.canvas_pixels;
    pixels[y * wagn0.width + x] = c;
}

static void _wagn0_draw_filled_rect(int x, int y, int w, int h, pixel_t c) {
    for (int iy = y; iy < y + h; iy++) {
        for (int ix = x; ix < x + w; ix++) {
            _wagn0_set_pixel(ix, iy, c);
        }
    }
}

static void _wagn0_draw_rect_outline(int x, int y, int w, int h, pixel_t c, int weight) {
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

void background(pixel_t c) {
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
            float rad = angle * PI / 180.0f;
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

static int _wagn0_text_size = 1;

void text(const char* text_str, int x, int y) {
    Olivec_Canvas oc = olivec_canvas(
        wagn0.canvas_pixels, wagn0.width, wagn0.height,
        wagn0.width, WAGN0_BPP);
    olivec_text(oc, text_str, x, y, olivec_default_font,
                (size_t)_wagn0_text_size, (uint32_t)wagn0.fill_color);
}

void text_size(int size) {
    _wagn0_text_size = size > 0 ? size : 1;
}

int text_width(const char* text_str) {
    int len = 0;
    while (text_str[len]) len++;
    return len * (int)olivec_default_font.width * _wagn0_text_size;
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
            
            if (img.bpp == 32 ? c.a > 128 : true) {
                _wagn0_set_pixel(px, py, rgb(c.r, c.g, c.b));
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
            
            if (img.bpp == 32 ? c.a > 128 : true) {
                _wagn0_set_pixel(px, py, rgb(c.r, c.g, c.b));
            }
        }
    }
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
        wagn0.fill_color   = WHITE;
        wagn0.stroke_color = BLACK;
        wagn0.stroke_weight = 1;
        wagn0.no_fill   = false;
        wagn0.no_stroke = false;
        wagn0.mouse  = vec2(0, 0);
        wagn0.pmouse = vec2(0, 0);
        wagn0.mouse_pressed  = false;
        wagn0.mouse_released = false;
        wagn0.mouse_down     = false;
        wagn0.canvas_pixels = w_vram;
        w_setup("WagnO Game", wagn0.width, wagn0.height, wagn0.bpp, wagn0.scale);
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

    // Fill audio buffer
    fill_audio();

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