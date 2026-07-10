# wagner

High-level game development API for Wagnostic.

```c
#include "wagner.h"

void draw() {
    clear(BLUE);
    
    push();
    translate(10, 10);
    fill(RED);
    rect();
    pop();
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
fill(rgb(255, 0, 0));    // red in any BPP
fill(0xFFFF0000);        // same, raw ARGB
```

## Rendering (State-Machine API)

wagner uses a state-machine rendering approach. Instead of passing properties (like coordinates, color, or the target canvas) directly into drawing functions, you configure the state machine, and the drawing functions simply rasterize the current state.

### State Manipulation

```c
push();                                  // save current state (matrix, colors, texture)
pop();                                   // restore previous state

// Transformations
translate(float x, float y);             // move origin
scale(float sx, float sy);               // scale drawing
rotate(float angle_radians);             // rotate around origin

// Styling
fill(pixel_t color);                     // set fill color
no_fill(void);                           // disable filling
stroke(pixel_t color);                   // set stroke color
no_stroke(void);                         // disable stroking
texture(Canvas* tex);                    // bind a texture to be mapped on primitives
```

### Primitives

```c
clear(pixel_t color);                    // fill entire canvas with color

rect(void);                              // 1x1 quad mapped by matrix
circle(void);                            // circle mapped by matrix (can be textured)
triangle(void);                          // triangle mapped by matrix
line(float x1, float y1, float x2, float y2); // draw a line segment
pixel(float x, float y);                 // draw a single pixel
```

### Text

```c
text(const char* text_str);              // draws text at current origin using fill color
int text_width(const char* text_str);    // calculate string width (depends on matrix horizontal scale)
```

### Images

```c
// Async loading (recommended, use in preload callback)
void preload() {
    load_image(&img, "player.png");         // load from assets/
}

// Drawing images using state machine
push();
translate(x, y);
scale(img.width, img.height);
texture(&img);
rect(); // Draws the image, correctly mapping UVs and converting BPP!
pop();
```

## Audio (synth)

```c
play_tone(440.0f, 1.0f, 0.5f);   // sine wave
play_noise(0.5f, 0.3f);          // white noise burst
stop_all_sounds();
int audio_is_playing(void);
```

## Audio (decoded files — WAV/MP3/OGG)

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

## Framerate

```c
set_fps(30);                    // request 30 FPS from the host
// wagner.fps computed automatically every second
// wagner.delta_time reflects actual frame time
```

## Math

```c
float map(v, s1, s2, t1, t2);   float constrain(v, min, max);
float lerp(a, b, t);            pixel_t lerp_color(a, b, t);
float dist(x1, y1, x2, y2);     float dist_sq(x1, y1, x2, y2);
float sin(x);  float cos(x);    float sqrt(x);
void  random_seed(uint32_t);
int   random_int(min, max);     int random(min, max);
```

## Status

| Expression | Value |
|---|---|
| `wagner.mouse.x` / `.y` | Mouse position |
| `wagner.mouse_down` | Mouse held |
| `wagner.mouse_pressed` / `.mouse_released` | Edge detection |
| `wagner.keys[scancode]` | Keyboard state (USB HID) |
| `wagner.delta_time` | Seconds since last frame |
| `wagner.frame_count` | Total frames rendered |
| `wagner.fps` | Actual frames per second (computed) |

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
    // Use with texture() in state machine
}
```

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
