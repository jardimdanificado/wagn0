#ifndef WAGNOSTIC_H
#define WAGNOSTIC_H

/**
 * Wagnostic - WASM Multimedia Runtime
 *
 * API based on a single state struct. ROMs allocate a WagnosticState
 * somewhere in their linear memory and return its address from wupdate().
 * The host dereferences that pointer to read/write state.
 *
 * Usage:
 *   #include "wagnostic.h"
 *
 *   static struct {
 *       WagnosticState state;
 *       uint8_t vram[320 * 240 * 2];
 *   } rom;
 *
 *   int wupdate() {
 *       rom.state.width = 320;
 *       rom.state.height = 240;
 *       rom.state.bpp = 16;
 *       rom.state.vram_offset = (uint32_t)((uint8_t*)rom.vram - (uint8_t*)&rom.state);
 *       // ... draw to rom.vram ...
 *       rom.state.dirty_count = 1;
 *       rom.state.dirty_rects[0] = (Rect){0, 0, 320, 240};
 *       return (int)&rom.state;
 *   }
 */

#include <stdint.h>

// ============================================
// TYPES
// ============================================

#ifndef WAGNOSTIC_RECT_DEFINED
#define WAGNOSTIC_RECT_DEFINED
typedef struct {
    int x, y, w, h;
} Rect;
#endif

// ============================================
// GAMEPAD BUTTON CONSTANTS
// ============================================

#define W_BTN_UP     (1 << 0)
#define W_BTN_DOWN   (1 << 1)
#define W_BTN_LEFT   (1 << 2)
#define W_BTN_RIGHT  (1 << 3)
#define W_BTN_A      (1 << 4)
#define W_BTN_B      (1 << 5)
#define W_BTN_X      (1 << 6)
#define W_BTN_Y      (1 << 7)
#define W_BTN_L1     (1 << 8)
#define W_BTN_R1     (1 << 9)
#define W_BTN_START  (1 << 10)
#define W_BTN_SELECT (1 << 11)
#define W_BTN_L2     (1 << 12)
#define W_BTN_R2     (1 << 13)

// ============================================
// COLOR CONVERSION MACROS
// ============================================

#define W_RGB565(r, g, b) (uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))
#define W_RGBA(r, g, b, a) (uint32_t)(((a) << 24) | ((b) << 16) | ((g) << 8) | (r))
#define W_RGB332(r, g, b) (uint8_t)(((r) & 0xE0) | (((g) & 0xE0) >> 3) | (((b) & 0xC0) >> 6))

// ============================================
// STATE STRUCT
// ============================================

#define W_MAX_DIRTY_RECTS 32

typedef struct {
    // --- Screen Configuration (ROM writes, Host reads) ---
    uint32_t width;
    uint32_t height;
    uint32_t scale;
    char title[128];

    // --- Dirty Rectangles (ROM writes, Host reads) ---
    uint32_t dirty_rects;

    // --- Input (Host writes, ROM reads) ---
    int32_t mouse_x;
    int32_t mouse_y;
    uint32_t mouse_buttons;
    int32_t mouse_wheel;
    uint8_t keys[256];
    uint32_t gamepad_buttons;

    // --- Timing (Host writes, ROM reads) ---
    uint32_t ticks;

    // --- Framerate control (ROM writes, Host reads) ---
    uint32_t target_fps;

    // --- Audio (ROM writes, Host reads) ---
    uint32_t audio_size;
    uint32_t audio_sample_rate;
    uint32_t audio_bpp;
    uint32_t audio_channels;
    uint32_t audio_write;
    uint32_t audio_read;
    uint32_t audio_underrun;
    uint32_t audio_overrun;

    // --- Buffer offsets (from state base address) ---
    // ROM sets these to point to separately-allocated buffers.
    // If 0, the host treats the buffer as missing.
    uint32_t vram_offset;
    uint32_t audio_buffer_offset;
    uint32_t r_bits;
    uint32_t r_shift;
    uint32_t g_bits;
    uint32_t g_shift;
    uint32_t b_bits;
    uint32_t b_shift;
    uint32_t a_bits;
    uint32_t a_shift;
    uint32_t x_bits;
    uint32_t x_shift;

    // --- I/O Operations (ROM writes, Host reads/writes) ---
    uint8_t  reserved[516];
} WagnosticState;

typedef struct {
    uint32_t count;
    Rect rects[W_MAX_DIRTY_RECTS];
} WagnosticDirtyList;

typedef struct {
    uint16_t x, y;            // Scissor position (VRAM region)
    uint16_t width, height;   // Scissor size (VRAM region)
    uint32_t shader_ptr;      // Offset to null-terminated GLSL string
    uint32_t params_ptr;      // Offset to float array 'u_params'
} WagnosticShaderJob;

// ============================================
// BUFFER ACCESS MACROS
// ============================================

/* SET_BPP(s, bpp) — sets channel bits/shifts for standard pixel formats. */
#define SET_BPP(s, bpp_val) do { \
    if ((bpp_val) == 32) { \
        (s)->r_bits=8;(s)->r_shift=16; \
        (s)->g_bits=8;(s)->g_shift=8; \
        (s)->b_bits=8;(s)->b_shift=0; \
        (s)->a_bits=8;(s)->a_shift=24; \
        (s)->x_bits=0;(s)->x_shift=0; \
    } else if ((bpp_val) == 16) { \
        (s)->r_bits=5;(s)->r_shift=11; \
        (s)->g_bits=6;(s)->g_shift=5; \
        (s)->b_bits=5;(s)->b_shift=0; \
        (s)->a_bits=0;(s)->a_shift=0; \
        (s)->x_bits=0;(s)->x_shift=0; \
    } else if ((bpp_val) == 8) { \
        (s)->r_bits=3;(s)->r_shift=5; \
        (s)->g_bits=3;(s)->g_shift=2; \
        (s)->b_bits=2;(s)->b_shift=0; \
        (s)->a_bits=0;(s)->a_shift=0; \
        (s)->x_bits=0;(s)->x_shift=0; \
    } \
} while(0)

#define W_VRAM(s)       ((uint8_t*)(s) + (s)->vram_offset)
#define W_AUDIO_BUF(s)  ((uint8_t*)(s) + (s)->audio_buffer_offset)

// ============================================
// HELPER FUNCTIONS (inline)
// ============================================

static inline void w_setup(WagnosticState *s, const char* title, int width, int height, int bpp, int scale) {
    s->width = (uint32_t)width;
    s->height = (uint32_t)height;
    s->scale = (uint32_t)scale;
    if (title) {
        int i = 0;
        while (title[i] && i < 127) {
            s->title[i] = title[i];
            i++;
        }
        s->title[i] = '\0';
    }
    SET_BPP(s, bpp);
}

static inline void w_redraw(WagnosticState *s, WagnosticDirtyList *dl) {
    dl->count = 1;
    dl->rects[0].x = 0;
    dl->rects[0].y = 0;
    dl->rects[0].w = (int)s->width;
    dl->rects[0].h = (int)s->height;
    s->dirty_rects = (uint32_t)dl;
}

static inline void w_redraw_rect(WagnosticState *s, WagnosticDirtyList *dl, int x, int y, int w, int h) {
    if (dl->count < W_MAX_DIRTY_RECTS) {
        dl->rects[dl->count].x = x;
        dl->rects[dl->count].y = y;
        dl->rects[dl->count].w = w;
        dl->rects[dl->count].h = h;
        dl->count++;
    }
    s->dirty_rects = (uint32_t)dl;
}

// ============================================
// CONVENIENCE MACROS
// ============================================

#define W_KEY_DOWN(s, scancode) ((s)->keys[scancode] != 0)
#define W_MOUSE_LEFT(s) (((s)->mouse_buttons & 1) != 0)
#define W_MOUSE_RIGHT(s) (((s)->mouse_buttons & 2) != 0)

#endif // WAGNOSTIC_H
