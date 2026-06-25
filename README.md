# wagn0

High-level game development API for Wagnostic.

```c
#include "wagn0.h"

void setup() { }

void draw() {
    background(BLUE);
    fill(RED);
    rect(10, 10, 100, 80);
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
| 8   | `uint8_t` | RGB332               | `fill(0xE0)` |
| 16  | `uint16_t`| RGB565               | `fill(0xF800)` |
| 32  | `uint32_t`| RGBA8888             | `fill(0xFF0000FF)` |

Colors can be specified directly or via helpers:

```c
fill(rgb(255, 0, 0));       // red in current BPP
fill(rgba(255, 0, 0, 128)); // with alpha (32bpp only)
fill(hex(0xFF8800));        // hex → pixel_t
fill(0xF800);               // raw pixel_t value
```

## API

```c
// Drawing state
fill(pixel_t c);    no_fill();
stroke(pixel_t c);  no_stroke();  stroke_weight(int);

// Primitives
background(pixel_t c);
rect(x, y, w, h);   ellipse(x, y, w, h);
line(x1, y1, x2, y2);  point(x, y);
triangle(x1,y1,x2,y2,x3,y3);
quad(x1,y1,x2,y2,x3,y3,x4,y4);
arc(x, y, w, h, start_angle, stop_angle);

// Text (bitmap font from olive.c)
text(str, x, y);  text_size(int);  text_width(str);

// Images (alpha test applies only to 32bpp)
Wagn0Image img = create_image_from_data(data, w, h, bpp);
image(img, x, y);  image_scaled(img, x, y, w, h);

// Audio (default synth: sine tones + white noise, mixed into a
// 22050Hz mono s16 ring buffer that the host drains)
play_tone(freq, duration, volume);  // freq > 0
play_noise(duration, volume);       // white noise burst
stop_all_sounds();
__attribute__((weak)) void fill_audio(void);  // override to stream PCM

// Math
float map(v, s1, s2, t1, t2);  float constrain(v, min, max);
float lerp(a, b, t);            float dist(x1, y1, x2, y2);
float sin(x);  float cos(x);
int   random(min, max);
Vec2  vec2(x, y);  vec2_add(a, b);  // etc.

// Status
wagn0.mouse.x  wagn0.mouse.y          // mouse position
wagn0.mouse_down                       // mouse held
wagn0.mouse_pressed  .mouse_released   // edges
wagn0.keys[scancode]                   // keyboard
wagn0.delta_time                       // seconds since last frame
wagn0.frame_count                      // frame counter
```

## User Functions

```c
void setup(void);     // called once on first frame
void update(void);    // called before draw, every frame
void draw(void);      // called every frame
void mouse_pressed(void);
void mouse_released(void);
void key_pressed(int key);
void key_released(int key);
```

All callbacks must be defined in the ROM (wagn0 does not provide
weak defaults — missing definitions become unresolved imports that
prevent the wasm from loading).

## Quick Start

```bash
./wagn0 new mygame
cd mygame
../wagn0 build
../wagn0 run
```

## Structure

```
include/    wagn0.h, wagnostic.h, olive.c
examples/   10 example projects (each with wagn0.json)
templates/  project templates for `wagn0 new`
tools/      asset conversion utilities (img2c, audio2pcm)
```

## Commands

| Command | Description |
|---------|-------------|
| `./wagn0 new <name>` | Create a new project |
| `./wagn0 build` | Compile C → WASM |
| `./wagn0 run [--runner native\|sm]` | Run with wagnostic host |
| `./wagn0 dev [--runner native\|sm]` | Watch, rebuild, and run |

Set `WAGN0_RUNNER=sm` in the environment to default to the SpiderMonkey
host instead of the wasm3 native host.
