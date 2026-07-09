# wagner

High-level game development API for Wagnostic.

```c
#include "wagner.h"

void draw() {
    clear(screen, BLUE);
    draw_rect(screen, 10, 10, 100, 80, RED);
}
```

## Bit Depth

```c
#include "wagner.h"
```

`pixel_t` is always `uint32_t` (RGBA8888). The canvas BPP is set at
runtime via `w_setup()` and `wagner.json`. All drawing functions convert
to the canvas format automatically.

| `w_setup` BPP | Pixel format | Canvas buffer |
|---|---|---|
| 8   | RGB332 | 1 byte per pixel |
| 16  | RGB565 | 2 bytes per pixel |
| 24  | RGB888 | 3 bytes per pixel |
| 32  | RGBA8888 | 4 bytes per pixel |

Colors are always specified as RGBA8888 (`0xAABBGGRR`) and converted
to the canvas format by the drawing functions:

```c
draw_pixel(screen, 10, 10, rgb(255, 0, 0));    // red in any BPP
draw_pixel(screen, 10, 10, 0xFFFF0000);          // same, raw ARGB
```

## API

### Canvas (screen buffer)

```c
Canvas screen;         // global canvas (wraps w_vram)
Canvas c = canvas_sub(screen, x, y, w, h);  // sub-canvas (viewport, HUD)
Canvas offscreen = canvas_create(w, h, bpp); // dynamically allocated canvas
```

### Primitives

```c
clear(screen, color);                              // fill entire canvas
draw_rect(screen, x, y, w, h, color);              // filled rectangle
draw_rect_outline(screen, x, y, w, h, color);      // rectangle outline
draw_circle(screen, cx, cy, r, color);             // filled circle
draw_circle_outline(screen, cx, cy, r, color);     // circle outline
draw_ellipse(screen, x, y, w, h, color);            // filled ellipse
draw_line(screen, x1, y1, x2, y2, color);           // line
draw_pixel(screen, x, y, color);                   // single pixel
draw_triangle(screen, x1,y1, x2,y2, x3,y3, color); // filled triangle
draw_triangle3uv(screen, x1,y1, x2,y2, x3,y3,      // texture-mapped
    tx1,ty1, tx2,ty2, tx3,ty3, z1,z2,z3, tex);

// Pixel access
pixel_t c = pixel_at(canvas, x, y);                // read pixel
```

### Text

```c
draw_text(screen, "Hello", x, y, color);
text_size(int);
int text_width(const char*);
int text_height(void);
```

### Images

```c
// Async loading (recommended, use in preload callback)
void preload() {
    load_image(&img, "player.png");         // load from assets/
}

// Memory loading
Canvas img = img_load(data, size);          // decode PNG buffer → Canvas
Canvas img = img_create(data, w, h, bpp);   // create from raw pixels

// Drawing
draw_canvas(screen, img, x, y);             // with BPP conversion
draw_canvas_scaled(screen, img, x, y, w, h); // scaled, with BPP conversion
draw_canvas_ex(screen, img, x, y, use_color_key, color_key); // with color key transparency
draw_canvas_scaled_ex(screen, img, x, y, w, h, use_color_key, color_key); // scaled, with color key
```

Images loaded with `img_load()` or created with `img_create()` return a
`Canvas` with `stride = width`. They can be used anywhere a Canvas is
expected — sub-canvases, blitting, or direct pixel access.

### Audio (synth)

```c
play_tone(440.0f, 1.0f, 0.5f);   // sine wave
play_noise(0.5f, 0.3f);          // white noise burst
stop_all_sounds();
int audio_is_playing(void);
```

### Audio (decoded files — WAV/MP3/OGG)

```c
// Async loading (recommended, use in preload callback)
void preload() {
    load_audio(&snd, "jump.wav");           // load from assets/
}

// Memory decoding
WagnerAudio snd = wav_decode(data, size);    // or mp3_decode, ogg_decode
audio_play(&snd);                           // start playback
audio_stop(&snd);                           // stop playback
audio_is_playing();                         // still playing?
__attribute__((weak)) void fill_audio(void); // hook to stream custom PCM
```

### Framerate

```c
set_fps(30);                    // request 30 FPS from the host
// wagn0.fps computed automatically every second
// wagn0.delta_time reflects actual frame time
```

### Math

```c
float map(v, s1, s2, t1, t2);  float constrain(v, min, max);
float lerp(a, b, t);            pixel_t lerp_color(a, b, t);
float dist(x1, y1, x2, y2);     float dist_sq(x1, y1, x2, y2);
float sin(x);  float cos(x);    float sqrt(x);
void  random_seed(uint32_t);
int   random_int(min, max);     int random(min, max);
Vec2  vec2(x, y);  vec2_add(a, b);  // etc.
```

### Status

| Expression | Value |
|---|---|
| `wagn0.mouse.x` / `.y` | Mouse position |
| `wagn0.mouse_down` | Mouse held |
| `wagn0.mouse_pressed` / `.mouse_released` | Edge detection |
| `wagn0.keys[scancode]` | Keyboard state (USB HID) |
| `wagn0.delta_time` | Seconds since last frame |
| `wagn0.frame_count` | Total frames rendered |
| `wagn0.fps` | Actual frames per second (computed) |

## User Callbacks

```c
void preload(void);        // load assets here
void setup(void);          // init after assets are loaded
void draw(void);           // every frame
void mouse_pressed(void);
void mouse_released(void);
void key_pressed(int key);
void key_released(int key);
```

All callbacks must be defined. Use `#define WAGNER_NO_DEFAULT_CALLBACKS` before
including `wagner.h` if you define your own (to avoid weak-default conflicts).

## Assets (VFS)

Put files in `assets/`. They are bundled into the `.tar` Virtual File System automatically during `wagner build` and can be loaded dynamically at runtime:

```c
size_t size;
uint8_t* png_data = file_load("assets/icon.png", &size);
if (png_data) {
    Canvas icon = img_load(png_data, size);
    draw_canvas(screen, icon, 10, 10);
}

uint8_t* wav_data = file_load("assets/jump.wav", &size);
if (wav_data) {
    WagnerAudio sfx = wav_decode(wav_data, size);
}
```

Decoders (lodepng, dr_wav, etc.) are auto-linked by the build system when
matching file types are found in `assets/`.

Saving is completely transparent. Write to any path inside the `.tar` and it will be appended permanently (append-only inplace save):

```c
file_save("save.txt", "score: 100", 10);
```

## Quick Start

```bash
wagner new mygame
cd mygame
# place PNGs in assets/, write main.c
wagner build       # compiles with decoders and packages .tar VFS
wagner run         # run with native host
wagner run --runner sm   # use SpiderMonkey host
wagner dev         # watch, rebuild, run
```

## Structure

```
lib/          decoders, shim, fake POSIX headers (for gifdec)
include/      wagner.h, wagnostic.h, olive.c
examples/     12 example projects (each with wagner.json)
templates/    project skeleton for `wagner new`
```

## Commands

| Command | Description |
|---------|-------------|
| `wagner new <name>` | Create project with `assets/` directory |
| `wagner build` | Scan `assets/`, package into `.tar`, compile to WASM |
| `wagner run [--runner native\|sm]` | Run with host |
| `wagner dev [--runner native\|sm]` | Watch + rebuild + run |

## Credits

wagner uses several third-party libraries. Many thanks to their authors:

| Library | Author | Link |
|---|---|---|
| olive.c | tsoding | https://github.com/tsoding/olive.c |
| lodepng | Lode Vandevenne | https://github.com/lvandeve/lodepng |
| stb_vorbis | Sean Barrett | https://github.com/nothings/stb |
| dr_wav, dr_mp3 | David Reid (mackron) | https://github.com/mackron/dr_libs |
| gifdec | lecram | https://github.com/lecram/gifdec |
| wasm3 | Volodymyr Shymanskyy | https://github.com/wasm3/wasm3 |
