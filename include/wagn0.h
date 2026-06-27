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
 *   void draw() { ... }
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Audio API below needs w_audio_* globals, so include wagnostic.h here.
#define WAGNOSTIC_IMPLEMENTATION
#include "wagnostic.h"

// Auto-include generated assets.h (created by `wagn0 asset`).
// Stub is generated when no assets/ exists, so the include always works.
#if defined(__has_include)
#  if __has_include("assets.h")
#    include "assets.h"
#  endif
#elif __has_include("assets.h")
#    include "assets.h"
#endif

// GIF decoder (gifdec + posix_shim). Build auto-defines WAGN0_NO_GIF_DECODE
// when no GIF assets are found, so gifdec.h is only included when needed.
#ifndef WAGN0_NO_GIF_DECODE
#include "gifdec.h"
#include "posix_shim.h"
#endif

// pixel_t is always uint32_t. Runtime BPP (8/16/32) is handled by
// Canvas.bpp and olivec_set_pixel — no compile-time choice needed.
typedef uint32_t pixel_t;

// ============================================
// COLOR CONVERSION (input → pixel_t)
// ============================================

static inline pixel_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return (uint32_t)((0xFF << 24) | ((b) << 16) | ((g) << 8) | (r));
}

static inline pixel_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (uint32_t)(((a) << 24) | ((b) << 16) | ((g) << 8) | (r));
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

// Config defaults — overridden by `wagn0 build` via -D flags
#ifndef WAGN0_CFG_W
#define WAGN0_CFG_W 320
#define WAGN0_CFG_H 240
#define WAGN0_CFG_BPP 16
#define WAGN0_CFG_SCALE 2
#define WAGN0_TITLE "WagnO"
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

typedef struct { void* pixels; int width; int height; int stride; uint8_t bpp; } Canvas;

// ============================================
// GLOBAL STATE (managed by WagnO)
// ============================================

static struct {
    // Screen config
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
    
    // Main canvas (wraps w_vram)
    void* canvas_pixels;
} wagn0;

// The default screen canvas — initialized in wupdate()
Canvas screen;

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

static inline float wabs(float x) {
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
// CANVAS FUNCTIONS
// ============================================

Canvas canvas_sub(Canvas src, int x, int y, int w, int h);

// ============================================
// DRAWING PRIMITIVES
// ============================================

void clear(Canvas c, pixel_t color);
void draw_rect(Canvas c, int x, int y, int w, int h, pixel_t color);
void draw_ellipse(Canvas c, int x, int y, int w, int h, pixel_t color);
void draw_line(Canvas c, int x1, int y1, int x2, int y2, pixel_t color);
void draw_pixel(Canvas c, int x, int y, pixel_t color);
void draw_triangle(Canvas c, int x1, int y1, int x2, int y2, int x3, int y3, pixel_t color);

// Pixel access
pixel_t pixel_at(Canvas c, int x, int y);
void pixel_set(Canvas c, int x, int y, pixel_t color);

// Texture-mapped triangle (perspective-correct UV)
void draw_triangle3uv(Canvas c, int x1, int y1, int x2, int y2, int x3, int y3,
    float tx1, float ty1, float tx2, float ty2, float tx3, float ty3,
    float z1, float z2, float z3, Canvas texture);

// ============================================
// TEXT FUNCTIONS
// ============================================

void draw_text(Canvas c, const char* text, int x, int y, pixel_t color);
void text_size(int size);
int text_width(const char* text);

// ============================================
// IMAGE FUNCTIONS
// ============================================

Canvas img_create(const void* data, int width, int height, int bpp);
void draw_canvas(Canvas c, Canvas img, int x, int y);
void draw_canvas_scaled(Canvas c, Canvas img, int x, int y, int w, int h);
Canvas img_load(const uint8_t* data, size_t size);

// ============================================
// AUDIO FUNCTIONS
// ============================================

void play_tone(float freq, float duration, float volume);
void play_noise(float duration, float volume);
void stop_all_sounds(void);

// Decoded audio buffer for playing files (WAV/MP3/OGG).
typedef struct {
    const int16_t* samples;
    uint32_t num_samples;
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bpp;
    uint32_t read_pos;
    uint8_t active;
} Wagn0Audio;

void audio_play(const Wagn0Audio* audio);
int  audio_is_playing(void);

// Decoder wrappers — implementation is guarded by WAGN0_NO_AUDIO_DECODE
// which the build auto-defines when no audio assets are present.
Wagn0Audio wav_decode(const uint8_t* data, size_t size);
Wagn0Audio mp3_decode(const uint8_t* data, size_t size);
Wagn0Audio ogg_decode(const uint8_t* data, size_t size);

void set_fps(uint32_t fps);
void set_fps(uint32_t fps) { w_target_fps = fps; }

// Default fill_audio — plays from active audio if set, otherwise synth.
// Weak so user code can override entirely.
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

static Wagn0Audio _wagn0_playing = {0};

void audio_play(const Wagn0Audio* audio) {
    if (audio && audio->samples && audio->num_samples > 0) {
        _wagn0_playing = *audio;
        _wagn0_playing.read_pos = 0;
        _wagn0_playing.active = 1;
        w_audio_size = sizeof(w_audio_buffer);
        w_audio_sample_rate = audio->sample_rate;
        w_audio_bpp = 2;
        w_audio_channels = 1;
        w_audio_write = 0;
        w_audio_read = 0;
    }
}

int audio_is_playing(void) {
    return _wagn0_playing.active;
}

#ifndef WAGN0_CUSTOM_FILL_AUDIO
__attribute__((weak)) void fill_audio(void) {
    if (_wagn0_playing.active) {
        uint32_t w = w_audio_write;
        uint32_t r = w_audio_read;
        uint32_t size = w_audio_size;
        uint32_t occupied = (w >= r) ? (w - r) : (size - r + w);
        uint32_t free = size - 1 - occupied;
        if (free < 2) return;
        uint32_t to_write = free / 2;
        if (to_write > 2048) to_write = 2048;
        uint32_t remaining = _wagn0_playing.num_samples - _wagn0_playing.read_pos;
        if (to_write > remaining) to_write = remaining;
        if (to_write == 0) { _wagn0_playing.active = 0; return; }
        int16_t* buf = (int16_t*)w_audio_buffer;
        for (uint32_t i = 0; i < to_write; i++) {
            buf[w / 2] = _wagn0_playing.samples[_wagn0_playing.read_pos++];
            w = (w + 2) % size;
        }
        w_audio_write = w;
        return;
    }

    if (!_wagn0_audio_init) {
        _wagn0_init_sin_lut();
        w_audio_size         = sizeof(w_audio_buffer);
        w_audio_sample_rate  = WAGN0_AUDIO_SAMPLE_RATE;
        w_audio_bpp          = 2;
        w_audio_channels     = 1;
        w_audio_write        = 0;
        w_audio_read         = 0;
        _wagn0_audio_init    = 1;
    }

    uint32_t w    = w_audio_write;
    uint32_t r    = w_audio_read;
    uint32_t size = w_audio_size;

    uint32_t occupied = (w >= r) ? (w - r) : (size - r + w);
    if (occupied >= size - 1) return;

    uint32_t bytes_to_write = WAGN0_AUDIO_FADE_SAMPLES * 2;
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

// Defaults for config — overridden by `wagn0 build` via -DWAGN0_CFG_* flags

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
__attribute__((weak)) void mouse_pressed(void) {}
__attribute__((weak)) void mouse_released(void) {}
__attribute__((weak)) void key_pressed(int key) { (void)key; }
__attribute__((weak)) void key_released(int key) { (void)key; }
#endif

// ============================================
// CANVAS & DRAWING IMPLEMENTATION
// ============================================

Canvas canvas_sub(Canvas src, int x, int y, int w, int h) {
    Olivec_Canvas oc = { src.pixels, (size_t)src.width, (size_t)src.height,
                        (size_t)src.stride, src.bpp };
    Olivec_Canvas sub = olivec_subcanvas(oc, x, y, w, h);
    Canvas c = { sub.pixels, (int)sub.width, (int)sub.height, (int)sub.stride, sub.bpp };
    return c;
}

void clear(Canvas c, pixel_t color) {
    size_t n = (size_t)c.width * c.height;
    if (c.bpp == 32) { uint32_t* p = (uint32_t*)c.pixels; while (n--) *p++ = (uint32_t)color; }
    else if (c.bpp == 16) { uint16_t* p = (uint16_t*)c.pixels; while (n--) *p++ = (uint16_t)color; }
    else { uint8_t* p = (uint8_t*)c.pixels; while (n--) *p++ = (uint8_t)color; }
}

void draw_rect(Canvas c, int x, int y, int w, int h, pixel_t color) {
    Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height,
                        (size_t)c.stride, c.bpp };
    olivec_rect(oc, x, y, w, h, color);
}

void draw_ellipse(Canvas c, int x, int y, int w, int h, pixel_t color) {
    Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height,
                        (size_t)c.stride, c.bpp };
    int rx = w / 2, ry = h / 2;
    for (int iy = -ry; iy <= ry; iy++)
        for (int ix = -rx; ix <= rx; ix++)
            if ((float)(ix*ix)/(rx*rx) + (float)(iy*iy)/(ry*ry) <= 1.0f)
                olivec_set_pixel(oc, x + ix, y + iy, (uint32_t)color);
}

void draw_line(Canvas c, int x1, int y1, int x2, int y2, pixel_t color) {
    Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height,
                        (size_t)c.stride, c.bpp };
    olivec_line(oc, x1, y1, x2, y2, color);
}

void draw_pixel(Canvas c, int x, int y, pixel_t color) {
    Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height,
                        (size_t)c.stride, c.bpp };
    olivec_set_pixel(oc, x, y, (uint32_t)color);
}

void draw_triangle(Canvas c, int x1, int y1, int x2, int y2, int x3, int y3, pixel_t color) {
    Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height,
                        (size_t)c.stride, c.bpp };
    olivec_triangle(oc, x1, y1, x2, y2, x3, y3, color);
}

pixel_t pixel_at(Canvas c, int x, int y) {
    Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height,
                        (size_t)c.stride, c.bpp };
    return (pixel_t)olivec_get_pixel(oc, x, y);
}

void pixel_set(Canvas c, int x, int y, pixel_t color) {
    Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height,
                        (size_t)c.stride, c.bpp };
    olivec_set_pixel(oc, x, y, (uint32_t)color);
}

void draw_triangle3uv(Canvas c,
    int x1, int y1, int x2, int y2, int x3, int y3,
    float tx1, float ty1, float tx2, float ty2, float tx3, float ty3,
    float z1, float z2, float z3, Canvas texture)
{
    Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height,
                        (size_t)c.stride, c.bpp };
    Olivec_Canvas tex = { texture.pixels, (size_t)texture.width, (size_t)texture.height,
                         (size_t)texture.stride, texture.bpp };
    olivec_triangle3uv(oc, x1, y1, x2, y2, x3, y3,
        tx1, ty1, tx2, ty2, tx3, ty3,
        z1, z2, z3, tex);
}

// ============================================
// TEXT FUNCTIONS IMPLEMENTATION
// ============================================

static int _wagn0_text_size = 1;

void draw_text(Canvas c, const char* text_str, int x, int y, pixel_t color) {
    // olivec font only has a-z and 0-9. Convert uppercase to lowercase.
    char buf[256];
    const char* src = text_str;
    char* dst = buf;
    while (*src && dst - buf < 255) {
        char ch = *src++;
        if (ch >= 'A' && ch <= 'Z') ch += 32;  // → lowercase
        *dst++ = ch;
    }
    *dst = 0;
    Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height,
                        (size_t)c.stride, c.bpp };
    olivec_text(oc, buf, x, y, olivec_default_font,
                (size_t)_wagn0_text_size, (uint32_t)color);
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

 Canvas img_create(const void* data, int width, int height, int bpp) {
    Canvas img = { (void*)data, width, height, width, (uint8_t)bpp };
    return img;
 }

// Helper: read a pixel from any image/Canvas and return RGBA components
static inline Color _pixel_to_rgba(Canvas img, int index) {
    Color c = {0,0,0,255};
    if (img.bpp == 32) {
        uint32_t p = ((uint32_t*)img.pixels)[index];
        c.r = p & 0xFF; c.g = (p>>8)&0xFF; c.b = (p>>16)&0xFF; c.a = (p>>24)&0xFF;
    } else if (img.bpp == 16) {
        uint16_t p = ((uint16_t*)img.pixels)[index];
        c.r = ((p>>11)&0x1F)*255/31; c.g = ((p>>5)&0x3F)*255/63; c.b = (p&0x1F)*255/31;
    } else {
        uint8_t p = ((uint8_t*)img.pixels)[index];
        c.r = ((p>>5)&7)*255/7; c.g = ((p>>2)&7)*255/7; c.b = (p&3)*255/3;
    }
    return c;
}

// Helper: write RGBA to canvas with runtime BPP conversion
static inline void _canvas_set_pixel(Canvas c, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (x < 0 || x >= c.width || y < 0 || y >= c.height) return;
    if (c.bpp == 32) ((uint32_t*)c.pixels)[y * c.stride + x] = (a<<24)|(b<<16)|(g<<8)|r;
    else if (c.bpp == 16) ((uint16_t*)c.pixels)[y * c.stride + x] = ((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3);
    else ((uint8_t*)c.pixels)[y * c.stride + x] = ((r&0xE0)|((g&0xE0)>>3)|((b&0xC0)>>6));
}

void draw_canvas(Canvas c, Canvas img, int x, int y) {
    if (!img.pixels) return;
    for (int iy = 0; iy < img.height; iy++) {
        for (int ix = 0; ix < img.width; ix++) {
            int px = x + ix, py = y + iy;
            if (px < 0 || px >= c.width || py < 0 || py >= c.height) continue;
            Color col = _pixel_to_rgba(img, iy * img.width + ix);
            if (img.bpp == 32 && col.a <= 128) continue;
            _canvas_set_pixel(c, px, py, col.r, col.g, col.b, col.a);
        }
    }
}

void draw_canvas_scaled(Canvas c, Canvas img, int x, int y, int w, int h) {
    if (!img.pixels) return;
    for (int iy = 0; iy < h; iy++) {
        for (int ix = 0; ix < w; ix++) {
            int px = x + ix, py = y + iy;
            if (px < 0 || px >= c.width || py < 0 || py >= c.height) continue;
            int sx = ix * img.width / w, sy = iy * img.height / h;
            Color col = _pixel_to_rgba(img, sy * img.width + sx);
            if (img.bpp == 32 && col.a <= 128) continue;
            _canvas_set_pixel(c, px, py, col.r, col.g, col.b, col.a);
        }
    }
}

// ============================================
// DECODER WRAPPERS FOR ASSET DATA
// ============================================

// Guard: build auto-defines WAGN0_NO_PNG_DECODE when no PNG assets.
#ifndef WAGN0_NO_PNG_DECODE
unsigned lodepng_decode32(unsigned char** out, unsigned* w, unsigned* h,
                         const unsigned char* in, size_t insize);

Canvas img_load(const uint8_t* data, size_t size) {
    uint8_t* decoded = 0;
    unsigned w, h;
    if (lodepng_decode32(&decoded, &w, &h, data, size)) return (Canvas){0};
    return (Canvas){ .pixels = decoded, .width = (int)w, .height = (int)h, .stride = (int)w, .bpp = 32 };
}
#endif

// ============================================
// AUDIO DECODERS
// ============================================

#ifndef WAGN0_NO_AUDIO_DECODE
#define DR_WAV_NO_STDIO
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define DR_MP3_NO_STDIO
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

#include "stb_vorbis.h"

static inline void _monoize(const int16_t* stereo, int16_t* mono,
                            uint64_t frames, uint8_t channels) {
    for (uint64_t i = 0; i < frames; i++) {
        int32_t sum = 0;
        for (uint8_t c = 0; c < channels; c++)
            sum += stereo[i * channels + c];
        mono[i] = (int16_t)(sum / channels);
    }
}

Wagn0Audio wav_decode(const uint8_t* data, size_t size) {
    Wagn0Audio a = {0};
    drwav wav;
    if (!drwav_init_memory(&wav, data, size, NULL)) return a;
    uint64_t frames = wav.totalPCMFrameCount;
    uint8_t ch = (uint8_t)wav.channels;
    int16_t* tmp = (int16_t*)malloc((size_t)(frames * ch * sizeof(int16_t)));
    if (!tmp) { drwav_uninit(&wav); return a; }
    drwav_read_pcm_frames_s16(&wav, frames, tmp);
    drwav_uninit(&wav);
    int16_t* mono = (int16_t*)malloc((size_t)(frames * sizeof(int16_t)));
    if (!mono) { free(tmp); return a; }
    _monoize(tmp, mono, frames, ch);
    free(tmp);
    a.samples = mono; a.num_samples = (uint32_t)frames;
    a.sample_rate = wav.sampleRate; a.channels = 1; a.bpp = 2;
    return a;
}

Wagn0Audio mp3_decode(const uint8_t* data, size_t size) {
    Wagn0Audio a = {0};
    drmp3 mp3;
    if (!drmp3_init_memory(&mp3, data, size, NULL)) return a;
    uint64_t frames = drmp3_get_pcm_frame_count(&mp3);
    uint8_t ch = (uint8_t)mp3.channels;
    int16_t* tmp = (int16_t*)malloc((size_t)(frames * ch * sizeof(int16_t)));
    if (!tmp) { drmp3_uninit(&mp3); return a; }
    drmp3_read_pcm_frames_s16(&mp3, frames, tmp);
    drmp3_uninit(&mp3);
    int16_t* mono = (int16_t*)malloc((size_t)(frames * sizeof(int16_t)));
    if (!mono) { free(tmp); return a; }
    _monoize(tmp, mono, frames, ch);
    free(tmp);
    a.samples = mono; a.num_samples = (uint32_t)frames;
    a.sample_rate = mp3.sampleRate; a.channels = 1; a.bpp = 2;
    return a;
}

Wagn0Audio ogg_decode(const uint8_t* data, size_t size) {
    Wagn0Audio a = {0};
    int ch, rate;
    short* output;
    int len = stb_vorbis_decode_memory(data, (int)size, &ch, &rate, &output);
    if (len <= 0) return a;
    int16_t* mono = (int16_t*)malloc((size_t)(len * sizeof(int16_t)));
    if (!mono) { free(output); return a; }
    if (ch > 1) {
        _monoize(output, mono, (uint64_t)len, (uint8_t)ch);
    } else {
        for (int i = 0; i < len; i++) mono[i] = output[i];
    }
    free(output);
    a.samples = mono; a.num_samples = (uint32_t)len;
    a.sample_rate = (uint32_t)rate; a.channels = 1; a.bpp = 2;
    return a;
}
#endif

// ============================================
// MAIN WAGNO FUNCTIONS
// ============================================

int wupdate() {
    static int init = 0;
    if (!init) {
        init = 1;
        wagn0.width  = 320; wagn0.height = 240;
        wagn0.bpp    = 16;  wagn0.scale  = 4;
        wagn0.frame_count = 0; wagn0.fps = 0;
        wagn0.delta_time = 0.016f;
        wagn0.mouse  = vec2(0, 0); wagn0.pmouse = vec2(0, 0);
        wagn0.mouse_pressed = false; wagn0.mouse_released = false;
        wagn0.mouse_down = false;
        // Initialize screen canvas
        wagn0.canvas_pixels = w_vram;
        screen.pixels = w_vram; screen.width = 320; screen.height = 240;
        screen.stride = 320; screen.bpp = 16;
        w_setup(WAGN0_TITLE, WAGN0_CFG_W, WAGN0_CFG_H, WAGN0_CFG_BPP, WAGN0_CFG_SCALE);
        wagn0.width = w_width; wagn0.height = w_height;
        wagn0.bpp = w_bpp; wagn0.scale = w_scale;
        screen.width = w_width; screen.height = w_height;
        screen.stride = w_width; screen.bpp = (uint8_t)w_bpp;
    }
    // Update time
    static uint32_t last_ticks = 0;
    uint32_t now = w_ticks;
    if (last_ticks > 0) wagn0.delta_time = (now - last_ticks) / 1000.0f;
    last_ticks = now; wagn0.frame_count++;
    
    wagn0.pmouse = wagn0.mouse;
    wagn0.mouse = vec2(w_mouse_x, w_mouse_y);
    bool cur = (w_mouse_buttons & 1) != 0;
    wagn0.mouse_pressed = cur && !wagn0.mouse_down;
    wagn0.mouse_released = !cur && wagn0.mouse_down;
    wagn0.mouse_down = cur;
    wagn0.mouse_button = cur ? 1 : 0;
    for (int i = 0; i < 256; i++) {
        bool k = w_keys[i] != 0;
        wagn0.keys_pressed[i] = k && !wagn0.keys[i];
        wagn0.keys_released[i] = !k && wagn0.keys[i];
        wagn0.keys[i] = k;
    }
    draw();
    fill_audio();
    // Auto-calculate FPS
    static uint32_t _fps_timer = 0;
    if (_fps_timer == 0) _fps_timer = now;
    uint32_t _fps_elapsed = now - _fps_timer;
    if (_fps_elapsed >= 1000) {
        wagn0.fps = (wagn0.frame_count * 1000) / (_fps_elapsed ? _fps_elapsed : 1);
        wagn0.frame_count = 0;
        _fps_timer = now;
    }
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