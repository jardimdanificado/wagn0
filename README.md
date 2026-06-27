# wagn0

High-level game development API for Wagnostic.

```c
#include "wagn0.h"

void draw() {
    clear(screen, BLUE);
    draw_rect(screen, 10, 10, 100, 80, RED);
}
```

## Bit Depth

```c
#include "wagn0.h"
```

`pixel_t` is always `uint32_t` (RGBA8888). The canvas BPP is set at
runtime via `w_setup()` and `wagn0.json`. All drawing functions convert
to the canvas format automatically.

| `w_setup` BPP | Pixel format | Canvas buffer |
|---|---|---|
| 8   | RGB332 | 1 byte per pixel |
| 16  | RGB565 | 2 bytes per pixel |
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
```

### Primitives

```c
clear(screen, color);                              // fill entire canvas
draw_rect(screen, x, y, w, h, color);              // filled rectangle
draw_ellipse(screen, x, y, w, h, color);            // filled ellipse
draw_line(screen, x1, y1, x2, y2, color);           // line
draw_pixel(screen, x, y, color);                   // single pixel
draw_triangle(screen, x1,y1, x2,y2, x3,y3, color); // filled triangle
draw_triangle3uv(screen, x1,y1, x2,y2, x3,y3,      // texture-mapped
    tx1,ty1, tx2,ty2, tx3,ty3, z1,z2,z3, tex);

// Pixel access
pixel_t c = pixel_at(canvas, x, y);                // read pixel
pixel_set(canvas, x, y, color);                    // write pixel
```

### Text

```c
draw_text(screen, "Hello", x, y, color);
text_size(int);
int text_width(const char*);
```

### Images

```c
Canvas img = img_load(data, size);          // decode PNG asset → Canvas
Canvas img = img_create(data, w, h, bpp);   // create from raw pixels
draw_canvas(screen, img, x, y);             // with BPP conversion
draw_canvas_scaled(screen, img, x, y, w, h); // scaled, with BPP conversion
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
Wagn0Audio snd = wav_decode(data, size);    // or mp3_decode, ogg_decode
audio_play(&snd);                           // start playback
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
float lerp(a, b, t);            float dist(x1, y1, x2, y2);
float sin(x);  float cos(x);
int   random(min, max);
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
void draw(void);           // every frame
void mouse_pressed(void);
void mouse_released(void);
void key_pressed(int key);
void key_released(int key);
```

All callbacks must be defined. Use `#define WAGN0_NO_DEFAULT_CALLBACKS` before
including `wagn0.h` if you define your own (to avoid weak-default conflicts).

## Assets

Put files in `assets/` and they're automatically embedded as C arrays:

```
assets/
├── sprites/icon.png   → assets_sprites_icon_png_data + ASSETS_SPRITES_ICON_PNG_W/H
├── sfx/jump.wav       → assets_sfx_jump_wav_data + ASSETS_SFX_JUMP_WAV_RATE/LEN
└── data/config.json   → assets_data_config_data (raw bytes)
```

```c
Canvas icon = img_load(assets_sprites_icon_png_data,
                       sizeof(assets_sprites_icon_png_data));
draw_canvas(screen, icon, 10, 10);
Wagn0Audio sfx = wav_decode(assets_sfx_jump_wav_data,
                            sizeof(assets_sfx_jump_wav_data));
```

Decoders (lodepng, dr_wav, etc.) are auto-linked by the build system when
matching file types are found in `assets/`.

## Quick Start

```bash
wagn0 new mygame
cd mygame
# place PNGs in assets/, write main.c
wagn0 build       # auto-generates assets.h + compiles with decoders
wagn0 run         # run with native host
wagn0 run --runner sm   # use SpiderMonkey host
wagn0 dev         # watch, rebuild, run
```

## Structure

```
lib/          decoders, shim, fake POSIX headers (for gifdec)
include/      wagn0.h, wagnostic.h, olive.c
examples/     12 example projects (each with wagn0.json)
templates/    project skeleton for `wagn0 new`
```

## Commands

| Command | Description |
|---------|-------------|
| `wagn0 new <name>` | Create project with `assets/` directory |
| `wagn0 build` | Scan `assets/`, generate `assets.h`, compile to WASM |
| `wagn0 run [--runner native\|sm]` | Run with host |
| `wagn0 dev [--runner native\|sm]` | Watch + rebuild + run |

## Credits

wagn0 uses several third-party libraries. Many thanks to their authors:

| Library | Author | Link |
|---|---|---|
| olive.c | tsoding | https://github.com/tsoding/olive.c |
| lodepng | Lode Vandevenne | https://github.com/lvandeve/lodepng |
| stb_vorbis | Sean Barrett | https://github.com/nothings/stb |
| dr_wav, dr_mp3 | David Reid (mackron) | https://github.com/mackron/dr_libs |
| gifdec | lecram | https://github.com/lecram/gifdec |
| wasm3 | Volodymyr Shymanskyy | https://github.com/wasm3/wasm3 |
