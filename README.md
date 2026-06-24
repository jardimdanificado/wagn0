# wagn0

high-level game development API for wagnostic.

```c
#include "wagn0.h"

void draw() {
    background(WAGN0_BLUE);
    fill(WAGN0_RED);
    rect(10, 10, 100, 80);
}
```

## Quick Start

```bash
./wagn0 new mygame
cd mygame
../wagn0 build
../wagn0 run
```

## Structure

```
include/
├── wagn0.h
│   wagnostic.h
└── olive.c

examples/
├── draw_example/
├── images_example/
├── audio_example/
├── audio_test/
├── mouse_platformer/
├── roguelike_example/
├── tracker_example/
├── test_8bpp/
├── test_16bpp/
└── test_32bpp/

templates/           project templates for `wagn0 new`
tools/               asset conversion utilities
```

## Commands

| Command | Description |
|---------|-------------|
| `./wagn0 new <name>` | Create a new project |
| `./wagn0 build` | Compile C → WASM |
| `./wagn0 run` | Run with wagnostic host |
| `./wagn0 dev` | Watch, rebuild, and run |
