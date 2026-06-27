# wagn0

High-level game development API for Wagnostic.

```c
#include "wagn0.h"

void setup() {
    w_setup("my wagn0 game", 320, 240, 16, 2);
}

void draw() {
    clear(screen, BLUE);
    draw_rect(screen, 10, 10, 100, 80, RED);
}
```

## Bit Depth

```c
#define WAGN0_BPP 32   // 8, 16 (default), or 32
#include "wagn0.h"
```

Pixel type and color macros adapt automatically:

| BPP | `pixel_t` | `rgb(r,g,b)` returns | Example |
|-----|-----------|----------------------|---------|
| 8   | `uint8_t` | RGB332               | `draw_pixel(screen, 10, 10, 0xE0)` |
| 16  | `uint16_t`| RGB565               | `draw_pixel(screen, 10, 10, 0xF800)` |
| 32  | `uint32_t`| RGBA8888             | `draw_pixel(screen, 10, 10, 0xFF0000FF)` |

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
```

### Text

```c
draw_text(screen, "Hello", x, y, color);
text_size(int);
int text_width(const char*);
```

### Images

```c
Image img = img_load(data, size);           // decode PNG asset → Image
Image img = img_create(data, w, h, bpp);    // create from raw pixels
draw_image(screen, img, x, y);
draw_image_scaled(screen, img, x, y, w, h);
```

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
void setup(void);          // once on first frame
void update(void);         // before draw, every frame
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
Image icon = img_load(assets_sprites_icon_png_data,
                      sizeof(assets_sprites_icon_png_data));
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
