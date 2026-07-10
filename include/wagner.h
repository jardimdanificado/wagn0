#ifndef WAGNER_H
#define WAGNER_H

/**
 * WagnO - Easy Game Development API for Wagnostic
 * 
 * Inspired by p5.js and LÖVE2D, this API provides a simple way to create
 * games and interactive applications for the Wagnostic WASM runtime.
 * 
 * Usage:
 *   #define WAGNER_IMPLEMENTATION
 *   #include "wagner.h"
 * 
 *   void draw() { ... }
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// Audio API below needs w_audio_* globals, so include wagnostic.h here.
#define WAGNOSTIC_IMPLEMENTATION
#include "wagnostic.h"

// Wagnostic new API: ROM must provide a WagnosticState struct
static struct {
    WagnosticState state;
    uint8_t vram[WAGNER_CFG_W * WAGNER_CFG_H * (WAGNER_CFG_BPP == 24 ? 3 : (WAGNER_CFG_BPP / 8))]; // 24bpp→3, 32→4, 16→2, 8→1
    uint8_t audio_buffer[16384];
} _wagner_rom;

#define w_width _wagner_rom.state.width
#define w_height _wagner_rom.state.height
#define w_bpp _wagner_rom.state.bpp
#define w_scale _wagner_rom.state.scale
#define w_mouse_x _wagner_rom.state.mouse_x
#define w_mouse_y _wagner_rom.state.mouse_y
#define w_mouse_buttons _wagner_rom.state.mouse_buttons
#define w_keys _wagner_rom.state.keys
#define w_ticks _wagner_rom.state.ticks
#define w_target_fps _wagner_rom.state.target_fps
#define w_audio_size _wagner_rom.state.audio_size
#define w_audio_sample_rate _wagner_rom.state.audio_sample_rate
#define w_audio_bpp _wagner_rom.state.audio_bpp
#define w_audio_channels _wagner_rom.state.audio_channels
#define w_audio_write _wagner_rom.state.audio_write
#define w_audio_read _wagner_rom.state.audio_read
#define w_gamepad_buttons _wagner_rom.state.gamepad_buttons
#define w_vram _wagner_rom.vram
#define w_audio_buffer _wagner_rom.audio_buffer

// No assets.h anymore. Using Wagnostic .tar TAR API for dynamic loading.

// pixel_t is always uint32_t. Runtime BPP (8/16/32) is handled by
// Canvas.bpp and olivec_set_pixel — no compile-time choice needed.
typedef uint32_t pixel_t;

// ============================================
// COLOR CONVERSION (input → pixel_t)
// ============================================

static inline pixel_t rgb(uint8_t r, uint8_t g, uint8_t b) {
#if WAGNER_CFG_BPP == 16
    return (uint32_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
#elif WAGNER_CFG_BPP == 8
    return (uint32_t)(((r >> 5) << 5) | ((g >> 5) << 2) | (b >> 6));
#else
    return (uint32_t)((0xFF << 24) | ((b) << 16) | ((g) << 8) | (r));
#endif
}

static inline pixel_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
#if WAGNER_CFG_BPP == 16
    return (uint32_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
#elif WAGNER_CFG_BPP == 8
    return (uint32_t)(((r >> 5) << 5) | ((g >> 5) << 2) | (b >> 6));
#else
    return (uint32_t)(((a) << 24) | ((b) << 16) | ((g) << 8) | (r));
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

// Config defaults — overridden by `wagner build` via -D flags
#ifndef WAGNER_CFG_W
#define WAGNER_CFG_W 320
#define WAGNER_CFG_H 240
#define WAGNER_CFG_BPP 16
#define WAGNER_CFG_SCALE 2
#define WAGNER_TITLE "WagnO"
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
} wagner;

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


static inline float sqrt(float x) {
    // Newton's method approximation
    if (x <= 0) return 0;
    float guess = x / 2.0f;
    for (int i = 0; i < 10; i++) {
        guess = (guess + x / guess) / 2.0f;
    }
    return guess;
}

static inline float dist_sq(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return dx * dx + dy * dy;  // squared distance — fast, no sqrt
}

static inline float dist(float x1, float y1, float x2, float y2) {
    return sqrt(dist_sq(x1, y1, x2, y2));
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

static uint32_t _wagner_rng_seed = 12345;

static inline void random_seed(uint32_t seed) {
    _wagner_rng_seed = seed ? seed : 1;
}

static inline int random_int(int min_val, int max_val) {
    _wagner_rng_seed = _wagner_rng_seed * 1103515245 + 12345;
    return min_val + (int)((_wagner_rng_seed >> 16) % (uint32_t)(max_val - min_val + 1));
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
// TRANSFORMS & STATE
// ============================================

typedef struct {
    float a, c, e;
    float b, d, f;
} WagnerMatrix;

typedef struct {
    WagnerMatrix matrix;
    Canvas* target;
    pixel_t fill_color;
    pixel_t stroke_color;
    bool has_fill;
    bool has_stroke;
    Canvas* texture;
    bool has_color_key;
    uint32_t color_key;
} WagnerRenderState;

void push(void);
void pop(void);
void translate(float x, float y);
void rotate(float angle);
void scale(float sx, float sy);
void apply_matrix(float a, float b, float c, float d, float e, float f);

void render_target(Canvas* c);
void fill(pixel_t color);
void no_fill(void);
void stroke(pixel_t color);
void no_stroke(void);
void texture(Canvas* img);
void no_texture(void);
void color_key(uint32_t key);
void no_color_key(void);

// ============================================
// DRAWING PRIMITIVES
// ============================================

void clear(Canvas c);
void rect(Canvas c);
void circle(Canvas c);
void triangle(Canvas c); // Unit triangle
void triangle_pts(Canvas c, float x1, float y1, float x2, float y2, float x3, float y3);
void line(Canvas c, float x1, float y1, float x2, float y2);
void pixel(Canvas c, float x, float y);

// Pixel access
pixel_t pixel_at(Canvas c, int x, int y);

// Texture-mapped triangle (perspective-correct UV)
void triangle3uv(Canvas c, int x1, int y1, int x2, int y2, int x3, int y3,
    float tx1, float ty1, float tx2, float ty2, float tx3, float ty3,
    float z1, float z2, float z3, Canvas texture);

// ============================================
// TEXT FUNCTIONS
// ============================================

void text(Canvas c, const char* text);
int text_width(const char* text);

// ============================================
// IMAGE FUNCTIONS
// ============================================

Canvas canvas_create(int w, int h, int bpp);
Canvas img_create(const void* data, int width, int height, int bpp);
Canvas img_load(const uint8_t* data, size_t size);
static inline pixel_t lerp_color(pixel_t a, pixel_t b, float t);
static inline int text_height(void);

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
} WagnerAudio;

void audio_play(const WagnerAudio* audio);
void audio_stop(WagnerAudio* audio);
int  audio_is_playing(void);

// Decoder wrappers — implementation is guarded by WAGNER_NO_AUDIO_DECODE
// which the build auto-defines when no audio assets are present.
WagnerAudio wav_decode(const uint8_t* data, size_t size);
WagnerAudio mp3_decode(const uint8_t* data, size_t size);
WagnerAudio ogg_decode(const uint8_t* data, size_t size);

void set_fps(uint32_t fps);
void set_fps(uint32_t fps) { w_target_fps = fps; }

// Default fill_audio — plays from active audio if set, otherwise synth.
// Weak so user code can override entirely.
__attribute__((weak)) void fill_audio(void);

#define WAGNER_MAX_TONES 8
#define WAGNER_AUDIO_SAMPLE_RATE 22050
#define WAGNER_AUDIO_FADE_SAMPLES 220  // 10ms at 22050Hz

typedef struct {
    uint8_t active;
    float freq;            // 0 = white noise
    float volume;          // 0.0..1.0
    uint32_t start_sample; // global sample count when triggered
    uint32_t duration_samples;
} WagnerTone;

static WagnerTone _wagner_tones[WAGNER_MAX_TONES];
static uint32_t _wagner_audio_sample = 0;
static uint8_t  _wagner_audio_init = 0;
static int16_t  _wagner_sin_lut[256];

static void _wagner_init_sin_lut(void) {
    for (int i = 0; i < 256; i++) {
        _wagner_sin_lut[i] = (int16_t)(sin(i * TWO_PI / 256.0f) * 32767.0f);
    }
}

void play_tone(float freq, float duration, float volume) {
    for (int i = 0; i < WAGNER_MAX_TONES; i++) {
        if (!_wagner_tones[i].active) {
            _wagner_tones[i].active = 1;
            _wagner_tones[i].freq = freq;
            _wagner_tones[i].volume = volume;
            _wagner_tones[i].start_sample = _wagner_audio_sample;
            _wagner_tones[i].duration_samples =
                (uint32_t)(duration * (float)WAGNER_AUDIO_SAMPLE_RATE);
            return;
        }
    }
}

void play_noise(float duration, float volume) {
    play_tone(0.0f, duration, volume);
}

void stop_all_sounds(void) {
    for (int i = 0; i < WAGNER_MAX_TONES; i++) {
        _wagner_tones[i].active = 0;
    }
}

#define WAGNER_MAX_AUDIO_PLAYING 8
static WagnerAudio _wagner_playing[WAGNER_MAX_AUDIO_PLAYING] = {0};

void audio_play(const WagnerAudio* audio) {
    if (audio && audio->samples && audio->num_samples > 0) {
        int slot = -1;
        int active_count = 0;
        for (int i = 0; i < WAGNER_MAX_AUDIO_PLAYING; i++) {
            if (!_wagner_playing[i].active && slot == -1) slot = i;
            if (_wagner_playing[i].active) active_count++;
        }
        if (slot == -1) slot = 0; // overwrite first if full
        
        _wagner_playing[slot] = *audio;
        _wagner_playing[slot].read_pos = 0;
        _wagner_playing[slot].active = 1;
        
        if (active_count == 0) {
            w_audio_size = sizeof(w_audio_buffer);
            w_audio_sample_rate = audio->sample_rate;
            w_audio_bpp = 2;
            w_audio_channels = 1;
            w_audio_write = 0;
            w_audio_read = 0;
        }
    }
}

void audio_stop(WagnerAudio* audio) {
    if (!audio) return;
    for (int i = 0; i < WAGNER_MAX_AUDIO_PLAYING; i++) {
        if (_wagner_playing[i].active &&
            _wagner_playing[i].samples == audio->samples) {
            _wagner_playing[i].active = 0;
        }
    }
}

int audio_is_playing(void) {
    for (int i = 0; i < WAGNER_MAX_AUDIO_PLAYING; i++) {
        if (_wagner_playing[i].active) return 1;
    }
    return 0;
}

#ifndef WAGNER_CUSTOM_FILL_AUDIO
__attribute__((weak)) void fill_audio(void) {
    if (audio_is_playing()) {
        uint32_t w = w_audio_write;
        uint32_t r = w_audio_read;
        uint32_t size = w_audio_size;
        uint32_t occupied = (w >= r) ? (w - r) : (size - r + w);
        uint32_t free = size - 1 - occupied;
        if (free < 2) return;
        uint32_t to_write = free / 2;
        if (to_write > 2048) to_write = 2048;

        int16_t* audio = (int16_t*)w_audio_buffer;
        
        for (uint32_t i = 0; i < to_write; i++) {
            int32_t mixed = 0;
            int any_playing = 0;
            for (int j = 0; j < WAGNER_MAX_AUDIO_PLAYING; j++) {
                if (_wagner_playing[j].active) {
                    any_playing = 1;
                    int16_t sample = _wagner_playing[j].samples[_wagner_playing[j].read_pos];
                    mixed += sample;
                    _wagner_playing[j].read_pos++;
                    if (_wagner_playing[j].read_pos >= _wagner_playing[j].num_samples) {
                        _wagner_playing[j].active = 0;
                    }
                }
            }
            if (!any_playing) {
                to_write = i;
                break;
            }
            if (mixed > 32767) mixed = 32767;
            else if (mixed < -32768) mixed = -32768;
            audio[(w / 2 + i) % (size / 2)] = (int16_t)mixed;
        }
        if (to_write > 0) {
            w_audio_write = (w + to_write * 2) % size;
        }
        return;
    }

    if (!_wagner_audio_init) {
        _wagner_init_sin_lut();
        w_audio_size         = sizeof(w_audio_buffer);
        w_audio_sample_rate  = WAGNER_AUDIO_SAMPLE_RATE;
        w_audio_bpp          = 2;
        w_audio_channels     = 1;
        w_audio_write        = 0;
        w_audio_read         = 0;
        _wagner_audio_init    = 1;
    }

    uint32_t w    = w_audio_write;
    uint32_t r    = w_audio_read;
    uint32_t size = w_audio_size;

    uint32_t occupied = (w >= r) ? (w - r) : (size - r + w);
    if (occupied >= size - 1) return;

    uint32_t bytes_to_write = WAGNER_AUDIO_FADE_SAMPLES * 2;
    if (bytes_to_write > (size - 1 - occupied)) {
        bytes_to_write = size - 1 - occupied;
    }
    if (bytes_to_write < 2) return;
    uint32_t samples = bytes_to_write / 2;

    int16_t* buf = (int16_t*)w_audio_buffer;
    static uint32_t noise_seed = 0x12345;

    for (uint32_t i = 0; i < samples; i++) {
        int32_t mix = 0;
        uint32_t gs = _wagner_audio_sample + i;

        for (int t = 0; t < WAGNER_MAX_TONES; t++) {
            if (!_wagner_tones[t].active) continue;
            uint32_t elapsed = gs - _wagner_tones[t].start_sample;
            if (elapsed >= _wagner_tones[t].duration_samples) {
                _wagner_tones[t].active = 0;
                continue;
            }

            float s;
            if (_wagner_tones[t].freq == 0.0f) {
                noise_seed = noise_seed * 1103515245u + 12345u;
                s = (int16_t)(noise_seed >> 16) / 32768.0f;
            } else {
                float phase = _wagner_tones[t].freq * (float)gs
                              / (float)WAGNER_AUDIO_SAMPLE_RATE;
                uint8_t idx = (uint8_t)(phase * 256.0f);
                s = _wagner_sin_lut[idx] / 32768.0f;
            }

            // 10ms fade in/out to avoid pops
            float fade = 1.0f;
            uint32_t dur = _wagner_tones[t].duration_samples;
            if (elapsed < WAGNER_AUDIO_FADE_SAMPLES) {
                fade = (float)elapsed / (float)WAGNER_AUDIO_FADE_SAMPLES;
            } else if (elapsed > dur - WAGNER_AUDIO_FADE_SAMPLES) {
                fade = (float)(dur - elapsed) / (float)WAGNER_AUDIO_FADE_SAMPLES;
            }

            mix += (int32_t)(s * _wagner_tones[t].volume * fade * 32767.0f);
        }

        if (mix >  32767) mix =  32767;
        if (mix < -32768) mix = -32768;
        buf[w / 2] = (int16_t)mix;
        w = (w + 2) % size;
    }

    w_audio_write = w;
    _wagner_audio_sample += samples;
}
#endif // !WAGNER_CUSTOM_FILL_AUDIO

// ============================================
// USER FUNCTIONS (implemented by user)
// ============================================

// Defaults for config — overridden by `wagner build` via -DWAGNER_CFG_* flags

void preload(void);
void setup(void);
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

// Weak declarations for optional callbacks. If undefined, their address is NULL.
__attribute__((weak)) void preload(void);
__attribute__((weak)) void setup(void);
__attribute__((weak)) void mouse_pressed(void);
__attribute__((weak)) void mouse_released(void);
__attribute__((weak)) void key_pressed(int key);
__attribute__((weak)) void key_released(int key);

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

static WagnerRenderState _wagner_state_stack[32] = {
    { {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f}, NULL, 0, 0, false, false, NULL, false, 0 }
};
static int _wagner_state_sp = 0;

static inline WagnerRenderState* _wagner_current_state(void) {
    return &_wagner_state_stack[_wagner_state_sp];
}

static inline WagnerMatrix* _wagner_current_matrix(void) {
    return &_wagner_state_stack[_wagner_state_sp].matrix;
}

void push(void) {
    if (_wagner_state_sp < 31) {
        _wagner_state_stack[_wagner_state_sp + 1] = _wagner_state_stack[_wagner_state_sp];
        _wagner_state_sp++;
    }
}

void pop(void) {
    if (_wagner_state_sp > 0) {
        _wagner_state_sp--;
    }
}

static inline Canvas _wagner_get_target(void) {
    Canvas* t = _wagner_current_state()->target;
    return t ? *t : screen;
}

void render_target(Canvas* c) { _wagner_current_state()->target = c; }
void fill(pixel_t color) { _wagner_current_state()->has_fill = true; _wagner_current_state()->fill_color = color; }
void no_fill(void) { _wagner_current_state()->has_fill = false; }
void stroke(pixel_t color) { _wagner_current_state()->has_stroke = true; _wagner_current_state()->stroke_color = color; }
void no_stroke(void) { _wagner_current_state()->has_stroke = false; }
void texture(Canvas* img) { _wagner_current_state()->texture = img; }
void no_texture(void) { _wagner_current_state()->texture = NULL; }
void color_key(uint32_t key) { _wagner_current_state()->has_color_key = true; _wagner_current_state()->color_key = key; }
void no_color_key(void) { _wagner_current_state()->has_color_key = false; }

static inline void _wagner_mat_mul(WagnerMatrix* m, float a, float b, float c, float d, float e, float f) {
    float nm_a = m->a * a + m->c * b;
    float nm_b = m->b * a + m->d * b;
    float nm_c = m->a * c + m->c * d;
    float nm_d = m->b * c + m->d * d;
    float nm_e = m->a * e + m->c * f + m->e;
    float nm_f = m->b * e + m->d * f + m->f;
    m->a = nm_a; m->b = nm_b; m->c = nm_c; m->d = nm_d; m->e = nm_e; m->f = nm_f;
}

void translate(float x, float y) {
    _wagner_mat_mul(_wagner_current_matrix(), 1.0f, 0.0f, 0.0f, 1.0f, x, y);
}

void rotate(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    _wagner_mat_mul(_wagner_current_matrix(), c, s, -s, c, 0.0f, 0.0f);
}

void scale(float sx, float sy) {
    _wagner_mat_mul(_wagner_current_matrix(), sx, 0.0f, 0.0f, sy, 0.0f, 0.0f);
}

void apply_matrix(float a, float b, float c, float d, float e, float f) {
    _wagner_mat_mul(_wagner_current_matrix(), a, b, c, d, e, f);
}

static inline void _wagner_transform(float* x, float* y) {
    WagnerMatrix* m = _wagner_current_matrix();
    float nx = m->a * (*x) + m->c * (*y) + m->e;
    float ny = m->b * (*x) + m->d * (*y) + m->f;
    *x = nx;
    *y = ny;
}

static inline int _wagner_is_axis_aligned(void) {
    WagnerMatrix* m = _wagner_current_matrix();
    return (wabs(m->b) < 0.001f && wabs(m->c) < 0.001f);
}

// Helper: read a pixel from any image/Canvas and return RGBA components
static inline Color _pixel_to_rgba(Canvas img, int index) {
    Color c = {0,0,0,255};
    if (img.bpp == 32) {
        uint32_t p = ((uint32_t*)img.pixels)[index];
        c.r = p & 0xFF; c.g = (p>>8)&0xFF; c.b = (p>>16)&0xFF; c.a = (p>>24)&0xFF;
    } else if (img.bpp == 24) {
        uint8_t* p = &((uint8_t*)img.pixels)[index * 3];
        c.r = p[0]; c.g = p[1]; c.b = p[2]; c.a = 255;
    } else if (img.bpp == 16) {
        uint16_t p = ((uint16_t*)img.pixels)[index];
        c.r = ((p>>11)&0x1F)*255/31; c.g = ((p>>5)&0x3F)*255/63; c.b = (p&0x1F)*255/31;
    } else {
        uint8_t p = ((uint8_t*)img.pixels)[index];
        c.r = ((p>>5)&7)*255/7; c.g = ((p>>2)&7)*255/7; c.b = (p&3)*255/3;
    }
    return c;
}

static inline void _canvas_set_pixel(Canvas c, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (x < 0 || x >= c.width || y < 0 || y >= c.height) return;
    if (c.bpp == 32) ((uint32_t*)c.pixels)[y * c.stride + x] = r | (g<<8) | (b<<16) | (a<<24);
    else if (c.bpp == 24) {
        uint8_t* p = &((uint8_t*)c.pixels)[(y * c.stride + x) * 3];
        p[0] = r; p[1] = g; p[2] = b;
    } else if (c.bpp == 16) ((uint16_t*)c.pixels)[y * c.stride + x] = ((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3);
    else ((uint8_t*)c.pixels)[y * c.stride + x] = ((r&0xE0)|((g&0xE0)>>3)|((b&0xC0)>>6));
}

void clear(Canvas c) {
    WagnerRenderState* state = _wagner_current_state();
    if (!state->has_fill) return;
    pixel_t color = state->fill_color;
    size_t n = (size_t)c.width * c.height;
    if (c.bpp == 32) { uint32_t* p = (uint32_t*)c.pixels; while (n--) *p++ = (uint32_t)color; }
    else if (c.bpp == 24) {
        uint8_t* p = (uint8_t*)c.pixels;
        uint8_t r = color & 0xFF, g = (color >> 8) & 0xFF, b = (color >> 16) & 0xFF;
        while (n--) { *p++ = r; *p++ = g; *p++ = b; }
    }
    else if (c.bpp == 16) { uint16_t* p = (uint16_t*)c.pixels; while (n--) *p++ = (uint16_t)color; }
    else { uint8_t* p = (uint8_t*)c.pixels; while (n--) *p++ = (uint8_t)color; }
}


static void _wagner_triangle_textured(Canvas c, int x1, int y1, int x2, int y2, int x3, int y3, 
                                     float tx1, float ty1, float tx2, float ty2, float tx3, float ty3, 
                                     Canvas tex, int has_tint, pixel_t tint_color) {
    int lx = x1, hx = x1, ly = y1, hy = y1;
    if (x2 < lx) lx = x2; if (x3 < lx) lx = x3; if (x2 > hx) hx = x2; if (x3 > hx) hx = x3;
    if (y2 < ly) ly = y2; if (y3 < ly) ly = y3; if (y2 > hy) hy = y2; if (y3 > hy) hy = y3;
    if (lx < 0) lx = 0; if (hx >= (int)c.width) hx = (int)c.width - 1;
    if (ly < 0) ly = 0; if (hy >= (int)c.height) hy = (int)c.height - 1;
    
    // Convert tint color to RGBA if needed
    Color tint = {255, 255, 255, 255};
    if (has_tint) {
        // We create a temporary 1x1 canvas to convert target format back to RGBA
        Canvas tmp_c = c;
        tmp_c.pixels = &tint_color;
        tmp_c.width = 1; tmp_c.stride = 1; tmp_c.height = 1;
        tint = _pixel_to_rgba(tmp_c, 0);
    }

    for (int y = ly; y <= hy; ++y) {
        for (int x = lx; x <= hx; ++x) {
            int u1, u2, det;
            if (olivec_barycentric(x1, y1, x2, y2, x3, y3, x, y, &u1, &u2, &det)) {
                float u3 = (float)(det - u1 - u2) / det, fu1 = (float)u1 / det, fu2 = (float)u2 / det;
                float tx = tx1*fu1 + tx2*fu2 + tx3*u3;
                float ty = ty1*fu1 + ty2*fu2 + ty3*u3;
                
                int sx = (int)(tx * tex.width);
                int sy = (int)(ty * tex.height);
                if (sx < 0) sx = 0; if (sx >= tex.width) sx = tex.width - 1;
                if (sy < 0) sy = 0; if (sy >= tex.height) sy = tex.height - 1;
                
                Color col = _pixel_to_rgba(tex, sy * tex.width + sx);
                
                if (has_tint) {
                    col.r = (col.r * tint.r) / 255;
                    col.g = (col.g * tint.g) / 255;
                    col.b = (col.b * tint.b) / 255;
                    col.a = (col.a * tint.a) / 255;
                }
                
                if (col.a > 128) { // simple alpha test
                    _canvas_set_pixel(c, x, y, col.r, col.g, col.b, col.a);
                }
            }
        }
    }
}

void rect(Canvas c) {
        Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height, (size_t)c.stride, c.bpp };
    WagnerRenderState* state = _wagner_current_state();
    
    if (state->texture != NULL) {
        Canvas img = *state->texture;
        if (_wagner_is_axis_aligned()) {
            float fx = 0.0f, fy = 0.0f; _wagner_transform(&fx, &fy);
            WagnerMatrix* m = _wagner_current_matrix();
            int nx = (int)fx, ny = (int)fy, nw = (int)m->a, nh = (int)m->d;
            if (nw < 0) { nx += nw; nw = -nw; }
            if (nh < 0) { ny += nh; nh = -nh; }
            
            for (int iy = 0; iy < nh; iy++) {
                for (int ix = 0; ix < nw; ix++) {
                    int px = nx + ix, py = ny + iy;
                    if (px < 0 || px >= c.width || py < 0 || py >= c.height) continue;
                    int sx = ix * img.width / nw, sy = iy * img.height / nh;
                    
                    if (state->has_color_key) {
                        uint32_t p = 0;
                        if (img.bpp == 32) p = ((uint32_t*)img.pixels)[sy * img.width + sx];
                        else if (img.bpp == 24) {
                            uint8_t* pxc = &((uint8_t*)img.pixels)[(sy * img.width + sx) * 3];
                            p = pxc[0] | (pxc[1]<<8) | (pxc[2]<<16);
                        } else if (img.bpp == 16) p = ((uint16_t*)img.pixels)[sy * img.width + sx];
                        else p = ((uint8_t*)img.pixels)[sy * img.width + sx];
                        if (p == state->color_key) continue;
                    }
                    
                    Color col = _pixel_to_rgba(img, sy * img.width + sx);
                    if (img.bpp == 32 && col.a <= 128) continue;
                    _canvas_set_pixel(c, px, py, col.r, col.g, col.b, col.a);
                }
            }
        } else {
            float x1 = 0.0f, y1 = 0.0f; _wagner_transform(&x1, &y1);
            float x2 = 1.0f, y2 = 0.0f; _wagner_transform(&x2, &y2);
            float x3 = 1.0f, y3 = 1.0f; _wagner_transform(&x3, &y3);
            float x4 = 0.0f, y4 = 1.0f; _wagner_transform(&x4, &y4);
            Olivec_Canvas tex_oc = { img.pixels, (size_t)img.width, (size_t)img.height, (size_t)img.stride, img.bpp };
            _wagner_triangle_textured(c, (int)x1, (int)y1, (int)x2, (int)y2, (int)x4, (int)y4, 0, 0, 1, 0, 0, 1, img, state->has_fill, state->fill_color);
            _wagner_triangle_textured(c, (int)x2, (int)y2, (int)x3, (int)y3, (int)x4, (int)y4, 1, 0, 1, 1, 0, 1, img, state->has_fill, state->fill_color);
        }
    } else if (state->has_fill) {
        if (_wagner_is_axis_aligned()) {
            float fx = 0, fy = 0; _wagner_transform(&fx, &fy);
            WagnerMatrix* m = _wagner_current_matrix();
            int nx = (int)fx, ny = (int)fy, nw = (int)(1.0f * m->a), nh = (int)(1.0f * m->d);
            if (nw < 0) { nx += nw; nw = -nw; }
            if (nh < 0) { ny += nh; nh = -nh; }
            olivec_rect(oc, nx, ny, nw, nh, state->fill_color);
        } else {
            float x1 = 0.0f, y1 = 0.0f; _wagner_transform(&x1, &y1);
            float x2 = 1.0f, y2 = 0.0f; _wagner_transform(&x2, &y2);
            float x3 = 1.0f, y3 = 1.0f; _wagner_transform(&x3, &y3);
            float x4 = 0.0f, y4 = 1.0f; _wagner_transform(&x4, &y4);
            olivec_triangle(oc, (int)x1, (int)y1, (int)x2, (int)y2, (int)x4, (int)y4, state->fill_color);
            olivec_triangle(oc, (int)x2, (int)y2, (int)x3, (int)y3, (int)x4, (int)y4, state->fill_color);
        }
    }
    
    if (state->has_stroke) {
        float x1 = 0.0f, y1 = 0.0f; _wagner_transform(&x1, &y1);
        float x2 = 1.0f, y2 = 0.0f; _wagner_transform(&x2, &y2);
        float x3 = 1.0f, y3 = 1.0f; _wagner_transform(&x3, &y3);
        float x4 = 0.0f, y4 = 1.0f; _wagner_transform(&x4, &y4);
        olivec_line(oc, (int)x1, (int)y1, (int)x2, (int)y2, state->stroke_color);
        olivec_line(oc, (int)x2, (int)y2, (int)x3, (int)y3, state->stroke_color);
        olivec_line(oc, (int)x3, (int)y3, (int)x4, (int)y4, state->stroke_color);
        olivec_line(oc, (int)x4, (int)y4, (int)x1, (int)y1, state->stroke_color);
    }
}

void circle(Canvas c) {
        Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height, (size_t)c.stride, c.bpp };
    WagnerRenderState* state = _wagner_current_state();
    
    if (state->texture != NULL) {
        Canvas img = *state->texture;
        Olivec_Canvas tex_oc = { img.pixels, (size_t)img.width, (size_t)img.height, (size_t)img.stride, img.bpp };
        
        float cx_f = 0, cy_f = 0; _wagner_transform(&cx_f, &cy_f);
        float last_x = 1.0f, last_y = 0.0f; _wagner_transform(&last_x, &last_y);
        
        float last_u = 1.0f, last_v = 0.5f;
        for (int i = 1; i <= 32; i++) {
            float ang = i * TWO_PI / 32.0f;
            float px = cos(ang), py = sin(ang);
            float pu = px * 0.5f + 0.5f;
            float pv = py * 0.5f + 0.5f;
            _wagner_transform(&px, &py);
            
            _wagner_triangle_textured(c, (int)cx_f, (int)cy_f, (int)last_x, (int)last_y, (int)px, (int)py, 0.5f, 0.5f, last_u, last_v, pu, pv, img, state->has_fill, state->fill_color);
            last_x = px; last_y = py;
            last_u = pu; last_v = pv;
        }
    } else if (state->has_fill) {
        if (_wagner_is_axis_aligned()) {
            float fx = 0, fy = 0; _wagner_transform(&fx, &fy);
            WagnerMatrix* m = _wagner_current_matrix();
            float rx = wabs(m->a), ry = wabs(m->d);
            
            if (wabs(rx - ry) < 0.001f) {
                int r = (int)rx;
                for (int y = -r; y <= r; y++) {
                    int dx = (int)sqrt((float)(r * r - y * y));
                    for (int x = -dx; x <= dx; x++) olivec_set_pixel(oc, (int)fx + x, (int)fy + y, state->fill_color);
                }
            } else {
                int irx = (int)rx, iry = (int)ry;
                if (irx > 0 && iry > 0) {
                    for (int iy = -iry; iy <= iry; iy++)
                        for (int ix = -irx; ix <= irx; ix++)
                            if ((float)(ix*ix)/(irx*irx) + (float)(iy*iy)/(iry*iry) <= 1.0f)
                                olivec_set_pixel(oc, (int)fx + ix, (int)fy + iy, state->fill_color);
                }
            }
        } else {
            float cx_f = 0, cy_f = 0; _wagner_transform(&cx_f, &cy_f);
            float last_x = 1.0f, last_y = 0.0f; _wagner_transform(&last_x, &last_y);
            for (int i = 1; i <= 32; i++) {
                float ang = i * TWO_PI / 32.0f;
                float px = cos(ang), py = sin(ang); _wagner_transform(&px, &py);
                olivec_triangle(oc, (int)cx_f, (int)cy_f, (int)last_x, (int)last_y, (int)px, (int)py, state->fill_color);
                last_x = px; last_y = py;
            }
        }
    }
    if (state->has_stroke) {
        if (_wagner_is_axis_aligned()) {
            float fx = 0, fy = 0; _wagner_transform(&fx, &fy);
            WagnerMatrix* m = _wagner_current_matrix();
            olivec_circle(oc, (int)fx, (int)fy, (int)wabs(m->a), state->stroke_color);
        } else {
            float last_x = 1.0f, last_y = 0.0f; _wagner_transform(&last_x, &last_y);
            for (int i = 1; i <= 32; i++) {
                float ang = i * TWO_PI / 32.0f;
                float px = cos(ang), py = sin(ang); _wagner_transform(&px, &py);
                olivec_line(oc, (int)last_x, (int)last_y, (int)px, (int)py, state->stroke_color);
                last_x = px; last_y = py;
            }
        }
    }
}

void triangle_pts(Canvas c, float x1, float y1, float x2, float y2, float x3, float y3) {
        Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height, (size_t)c.stride, c.bpp };
    WagnerRenderState* state = _wagner_current_state();
    float fx1 = x1, fy1 = y1; _wagner_transform(&fx1, &fy1);
    float fx2 = x2, fy2 = y2; _wagner_transform(&fx2, &fy2);
    float fx3 = x3, fy3 = y3; _wagner_transform(&fx3, &fy3);
    if (state->has_fill) olivec_triangle(oc, (int)fx1, (int)fy1, (int)fx2, (int)fy2, (int)fx3, (int)fy3, state->fill_color);
    if (state->has_stroke) {
        olivec_line(oc, (int)fx1, (int)fy1, (int)fx2, (int)fy2, state->stroke_color);
        olivec_line(oc, (int)fx2, (int)fy2, (int)fx3, (int)fy3, state->stroke_color);
        olivec_line(oc, (int)fx3, (int)fy3, (int)fx1, (int)fy1, state->stroke_color);
    }
}

void triangle(Canvas c) { triangle_pts(c, 0, -1, 0.866025f, 0.5f, -0.866025f, 0.5f); }

void line(Canvas c, float x1, float y1, float x2, float y2) {
        Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height, (size_t)c.stride, c.bpp };
    WagnerRenderState* state = _wagner_current_state();
    if (!state->has_stroke) return;
    float fx1 = x1, fy1 = y1; _wagner_transform(&fx1, &fy1);
    float fx2 = x2, fy2 = y2; _wagner_transform(&fx2, &fy2);
    olivec_line(oc, (int)fx1, (int)fy1, (int)fx2, (int)fy2, state->stroke_color);
}

void pixel(Canvas c, float x, float y) {
        Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height, (size_t)c.stride, c.bpp };
    WagnerRenderState* state = _wagner_current_state();
    if (!state->has_fill) return;
    float fx = x, fy = y; _wagner_transform(&fx, &fy);
    olivec_set_pixel(oc, (int)fx, (int)fy, state->fill_color);
}

pixel_t pixel_at(Canvas c, int x, int y) {
        Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height, (size_t)c.stride, c.bpp };
    return (pixel_t)olivec_get_pixel(oc, x, y);
}

void triangle3uv(Canvas c,
    int x1, int y1, int x2, int y2, int x3, int y3,
    float tx1, float ty1, float tx2, float ty2, float tx3, float ty3,
    float z1, float z2, float z3, Canvas texture)
{
    Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height, (size_t)c.stride, c.bpp };
    Olivec_Canvas tex = { texture.pixels, (size_t)texture.width, (size_t)texture.height, (size_t)texture.stride, texture.bpp };
    float fx1 = x1, fy1 = y1; _wagner_transform(&fx1, &fy1);
    float fx2 = x2, fy2 = y2; _wagner_transform(&fx2, &fy2);
    float fx3 = x3, fy3 = y3; _wagner_transform(&fx3, &fy3);
    olivec_triangle3uv(oc, (int)fx1, (int)fy1, (int)fx2, (int)fy2, (int)fx3, (int)fy3,
        tx1, ty1, tx2, ty2, tx3, ty3,
        z1, z2, z3, tex);
}

// ============================================
// TEXT FUNCTIONS IMPLEMENTATION
// ============================================


void text(Canvas c, const char* text_str) {
        WagnerRenderState* state = _wagner_current_state();
    if (!state->has_fill) return;
    
    float fx = 0, fy = 0; _wagner_transform(&fx, &fy);
    WagnerMatrix* m = _wagner_current_matrix();
    int size = (int)wabs(m->a);
    if (size < 1) size = 1;
    
    char buf[256];
    const char* src = text_str;
    char* dst = buf;
    while (*src && dst - buf < 255) {
        char ch = *src++;
        if (ch >= 'A' && ch <= 'Z') ch += 32;
        *dst++ = ch;
    }
    *dst = 0;
    Olivec_Canvas oc = { c.pixels, (size_t)c.width, (size_t)c.height, (size_t)c.stride, c.bpp };
    olivec_text(oc, buf, (int)fx, (int)fy, olivec_default_font, (size_t)size, (uint32_t)state->fill_color);
}


int text_width(const char* text_str) {
    int len = 0;
    while (text_str[len]) len++;
    WagnerMatrix* m = _wagner_current_matrix();
    int size = (int)wabs(m->a);
    if (size < 1) size = 1;
    return len * (int)olivec_default_font.width * size;
}

// ============================================
// IMAGE FUNCTIONS IMPLEMENTATION
// ============================================

Canvas canvas_create(int w, int h, int bpp) {
    size_t bpp_bytes = (bpp == 24) ? 3 : (bpp / 8);
    void* px = malloc((size_t)w * h * bpp_bytes);
    if (!px) return (Canvas){0};
    Canvas c = { px, w, h, w, (uint8_t)bpp };
    return c;
}

Canvas img_create(const void* data, int width, int height, int bpp) {
    Canvas img = { (void*)data, width, height, width, (uint8_t)bpp };
    return img;
 }


static inline pixel_t lerp_color(pixel_t a, pixel_t b, float t) {
    uint8_t ar = a & 0xFF,        ag = (a >> 8) & 0xFF,  ab_ = (a >> 16) & 0xFF;
    uint8_t br = b & 0xFF,        bg = (b >> 8) & 0xFF,  bb_ = (b >> 16) & 0xFF;
    uint8_t r = (uint8_t)(ar + (br - ar) * t);
    uint8_t g = (uint8_t)(ag + (bg - ag) * t);
    uint8_t bv = (uint8_t)(ab_ + (bb_ - ab_) * t);
    return rgb(r, g, bv);
}

static inline int text_height(void) {
    WagnerMatrix* m = _wagner_current_matrix();
    int size = (int)wabs(m->d);
    if (size < 1) size = 1;
    return (int)(olivec_default_font.height * (size_t)size);
}



// ============================================
// DECODER WRAPPERS FOR ASSET DATA
// ============================================

// Guard: build auto-defines WAGNER_NO_PNG_DECODE when no PNG assets.
#ifndef WAGNER_NO_PNG_DECODE
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

#ifndef WAGNER_NO_AUDIO_DECODE
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

WagnerAudio wav_decode(const uint8_t* data, size_t size) {
    WagnerAudio a = {0};
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

WagnerAudio mp3_decode(const uint8_t* data, size_t size) {
    WagnerAudio a = {0};
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

WagnerAudio ogg_decode(const uint8_t* data, size_t size) {
    WagnerAudio a = {0};
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

// ============================================
// PRELOAD SYSTEM
// ============================================

typedef enum {
    WAGNER_ASSET_IMAGE,
    WAGNER_ASSET_AUDIO,
    WAGNER_ASSET_DATA
} WagnerAssetType;

typedef struct {
    char path[128];
    WagnerAssetType type;
    void* target;
} WagnerAssetRequest;

#define WAGNER_MAX_ASSET_REQUESTS 64
static WagnerAssetRequest _wagner_asset_queue[WAGNER_MAX_ASSET_REQUESTS];
static int _wagner_queue_head = 0;
static int _wagner_queue_tail = 0;

static int _wagner_preloading_state = 0;
// 0 = Initial (run preload, then move to 1)
// 1 = Loading (process queue until empty, then move to 2)
// 2 = Setup (run setup, then move to 3)
// 3 = Running

static int _wagner_io_step = 0; 
// 0 = Idle / Dequeue next
// 1 = Waiting for size
// 2 = Waiting for data
static uint8_t* _wagner_io_buffer = NULL;

void load_image(Canvas* out, const char* path) {
    if (_wagner_queue_tail >= WAGNER_MAX_ASSET_REQUESTS) return;
    WagnerAssetRequest* req = &_wagner_asset_queue[_wagner_queue_tail++];
    int i = 0;
    while(path[i] && i < 127) { req->path[i] = path[i]; i++; }
    req->path[i] = '\0';
    req->type = WAGNER_ASSET_IMAGE;
    req->target = out;
}

void load_audio(WagnerAudio* out, const char* path) {
    if (_wagner_queue_tail >= WAGNER_MAX_ASSET_REQUESTS) return;
    WagnerAssetRequest* req = &_wagner_asset_queue[_wagner_queue_tail++];
    int i = 0;
    while(path[i] && i < 127) { req->path[i] = path[i]; i++; }
    req->path[i] = '\0';
    req->type = WAGNER_ASSET_AUDIO;
    req->target = out;
}

typedef struct {
    void* data;
    uint32_t size;
} WagnerData;

void load_data(WagnerData* out, const char* path) {
    if (_wagner_queue_tail >= WAGNER_MAX_ASSET_REQUESTS) return;
    WagnerAssetRequest* req = &_wagner_asset_queue[_wagner_queue_tail++];
    int i = 0;
    while(path[i] && i < 127) { req->path[i] = path[i]; i++; }
    req->path[i] = '\0';
    req->type = WAGNER_ASSET_DATA;
    req->target = out;
}

static char _wagner_save_path[128];
bool file_save(const char* path, const void* data, uint32_t size) {
    if (_wagner_rom.state.io_save) return false; // Busy
    int i = 0;
    while(path[i] && i < 127) { _wagner_save_path[i] = path[i]; i++; }
    _wagner_save_path[i] = '\0';
    _wagner_rom.state.io_save_buffer = (uint32_t)(uintptr_t)data;
    _wagner_rom.state.io_save_size = size;
    _wagner_rom.state.io_save = (uint32_t)(uintptr_t)_wagner_save_path;
    return true;
}

bool file_is_saving(void) {
    return _wagner_rom.state.io_save != 0;
}

static void _wagner_copy_str(uint8_t* dst, const char* src) {
    int i = 0;
    while(src[i] && i < 255) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

int wupdate() {
    static int init = 0;
    if (!init) {
        init = 1;
        wagner.width  = 320; wagner.height = 240;
        wagner.bpp    = 16;  wagner.scale  = 4;
        wagner.frame_count = 0; wagner.fps = 0;
        wagner.delta_time = 0.016f;
        wagner.mouse  = vec2(0, 0); wagner.pmouse = vec2(0, 0);
        wagner.mouse_pressed = false; wagner.mouse_released = false;
        wagner.mouse_down = false;
        
        _wagner_rom.state.vram_offset = sizeof(WagnosticState);
        _wagner_rom.state.audio_buffer_offset = sizeof(WagnosticState) + sizeof(_wagner_rom.vram);
        wagner.canvas_pixels = w_vram;
        screen.pixels = w_vram; screen.width = 320; screen.height = 240;
        screen.stride = 320; screen.bpp = 16;
        w_setup(&_wagner_rom.state, WAGNER_TITLE, WAGNER_CFG_W, WAGNER_CFG_H, WAGNER_CFG_BPP, WAGNER_CFG_SCALE);
        wagner.width = w_width; wagner.height = w_height;
        wagner.bpp = w_bpp; wagner.scale = w_scale;
        screen.width = w_width; screen.height = w_height;
        screen.stride = w_width; screen.bpp = (uint8_t)w_bpp;
    }

    // Input processing
    wagner.pmouse = wagner.mouse;
    wagner.mouse = vec2(w_mouse_x, w_mouse_y);
    bool cur = (w_mouse_buttons & 1) != 0;
    wagner.mouse_pressed = cur && !wagner.mouse_down;
    wagner.mouse_released = !cur && wagner.mouse_down;
    wagner.mouse_down = cur;
    wagner.mouse_button = cur ? 1 : 0;
    for (int i = 0; i < 256; i++) {
        bool k = w_keys[i] != 0;
        wagner.keys_pressed[i] = k && !wagner.keys[i];
        wagner.keys_released[i] = !k && wagner.keys[i];
        wagner.keys[i] = k;
    }

    if (_wagner_preloading_state == 0) {
        if (preload) preload();
        _wagner_preloading_state = 1;
        _wagner_io_step = 0;
    }

    if (_wagner_preloading_state == 1) {
        if (_wagner_queue_head >= _wagner_queue_tail) {
            _wagner_preloading_state = 2; // Done loading
        } else {
            WagnerAssetRequest* req = &_wagner_asset_queue[_wagner_queue_head];
            
            if (_wagner_io_step == 0) {
                // Step 0: Ask for file size
                _wagner_rom.state.io_load = (uint32_t)(uintptr_t)_wagner_rom.state.title; // use title temporarily or allocated buffer
                // Actually, let's use a 256 byte buffer inside state or just a static buffer
                static uint8_t path_buf[256];
                _wagner_copy_str(path_buf, req->path);
                _wagner_rom.state.io_load = (uint32_t)(uintptr_t)path_buf;
                _wagner_rom.state.io_load_buffer = 0; // ask for size
                _wagner_rom.state.io_load_size = 0;
                _wagner_io_step = 1;
                return (int)&_wagner_rom.state;
            } 
            else if (_wagner_io_step == 1) {
                // Step 1: Wait for size to be returned
                if (_wagner_rom.state.io_load != 0) return (int)&_wagner_rom.state; // Host hasn't cleared it yet
                
                uint32_t size = _wagner_rom.state.io_load_size;
                if (size == 0) {
                    // File not found or empty
                    _wagner_queue_head++;
                    _wagner_io_step = 0;
                    return (int)&_wagner_rom.state;
                }
                
                // Allocate buffer
                _wagner_io_buffer = (uint8_t*)malloc(size);
                if (!_wagner_io_buffer) {
                    _wagner_queue_head++;
                    _wagner_io_step = 0;
                    return (int)&_wagner_rom.state;
                }
                
                // Request actual data
                static uint8_t path_buf[256];
                _wagner_copy_str(path_buf, req->path);
                _wagner_rom.state.io_load = (uint32_t)(uintptr_t)path_buf;
                _wagner_rom.state.io_load_buffer = (uint32_t)(uintptr_t)_wagner_io_buffer;
                _wagner_io_step = 2;
                return (int)&_wagner_rom.state;
            }
            else if (_wagner_io_step == 2) {
                // Step 2: Wait for data
                if (_wagner_rom.state.io_load != 0) return (int)&_wagner_rom.state; // Not ready
                
                uint32_t size = _wagner_rom.state.io_load_size;
                
                // Decode data
                if (req->type == WAGNER_ASSET_IMAGE) {
#ifndef WAGNER_NO_PNG_DECODE
                    Canvas img = img_load(_wagner_io_buffer, size);
                    if (req->target) *((Canvas*)req->target) = img;
#endif
                } else if (req->type == WAGNER_ASSET_AUDIO) {
#ifndef WAGNER_NO_AUDIO_DECODE
                    // Basic sniffer to decide between wav/mp3/ogg based on path extension
                    int len = 0; while(req->path[len]) len++;
                    if (len >= 4) {
                        const char* ext = &req->path[len-4];
                        if (ext[1]=='w' && ext[2]=='a' && ext[3]=='v') {
                            WagnerAudio a = wav_decode(_wagner_io_buffer, size);
                            if (req->target) *((WagnerAudio*)req->target) = a;
                        }
                        else if (ext[1]=='m' && ext[2]=='p' && ext[3]=='3') {
                            WagnerAudio a = mp3_decode(_wagner_io_buffer, size);
                            if (req->target) *((WagnerAudio*)req->target) = a;
                        }
                        else if (ext[1]=='o' && ext[2]=='g' && ext[3]=='g') {
                            WagnerAudio a = ogg_decode(_wagner_io_buffer, size);
                            if (req->target) *((WagnerAudio*)req->target) = a;
                        }
                    }
#endif
                } else if (req->type == WAGNER_ASSET_DATA) {
                    if (req->target) {
                        WagnerData* d = (WagnerData*)req->target;
                        d->data = _wagner_io_buffer;
                        d->size = size;
                    }
                    _wagner_io_buffer = NULL; // Do not free it! User owns it.
                }
                
                if (_wagner_io_buffer) free(_wagner_io_buffer);
                _wagner_io_buffer = NULL;
                
                _wagner_queue_head++;
                _wagner_io_step = 0;
                return (int)&_wagner_rom.state;
            }
        }
    }

    if (_wagner_preloading_state == 2) {
        if (setup) setup();
        _wagner_preloading_state = 3;
    }

    // Regular draw loop
    static uint32_t last_ticks = 0;
    uint32_t now = w_ticks;
    if (last_ticks > 0) wagner.delta_time = (now - last_ticks) / 1000.0f;
    last_ticks = now; wagner.frame_count++;
    
    draw();
    fill_audio();
    
    static uint32_t _fps_timer = 0;
    if (_fps_timer == 0) _fps_timer = now;
    uint32_t _fps_elapsed = now - _fps_timer;
    if (_fps_elapsed >= 1000) {
        wagner.fps = (wagner.frame_count * 1000) / (_fps_elapsed ? _fps_elapsed : 1);
        wagner.frame_count = 0;
        _fps_timer = now;
    }
    
    if (wagner.mouse_pressed && mouse_pressed) mouse_pressed();
    if (wagner.mouse_released && mouse_released) mouse_released();
    for (int i = 0; i < 256; i++) {
        if (wagner.keys_pressed[i] && key_pressed) key_pressed(i);
        if (wagner.keys_released[i] && key_released) key_released(i);
    }
    w_redraw(&_wagner_rom.state);
    return (int)&_wagner_rom.state;
}

#endif // WAGNER_H