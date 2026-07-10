#define WAGNER_IMPLEMENTATION
#include "wagner.h"

// ---------------------------------------------------------
// PSEUDO-SHADER TOY
// ---------------------------------------------------------
// This function runs for every pixel on the screen.
// x, y: absolute pixel coordinates
// u, v: normalized coordinates (0.0 to 1.0)
// time: elapsed time in seconds (for animation)
pixel_t my_shader(int x, int y, float u, float v, float time) {
    // Example 1: Pulsing gradient
    // uint8_t r = (uint8_t)(u * 255);
    // uint8_t g = (uint8_t)(v * 255);
    // uint8_t b = (uint8_t)((sin(time * 2.0f) * 0.5f + 0.5f) * 255);
    // return rgba(r, g, b, 255);

    // Example 2: Distance field pattern
    float cx = 0.5f + sin(time * 1.5f) * 0.2f;
    float cy = 0.5f + cos(time * 2.0f) * 0.2f;
    
    // distance from current pixel (u,v) to center (cx,cy)
    float d = dist(u, v, cx, cy);
    
    // create rings using sine wave
    float rings = sin(d * 40.0f - time * 5.0f);
    
    // map the sine wave [-1, 1] to a color intensity [0, 255]
    uint8_t intensity = (uint8_t)map(rings, -1.0f, 1.0f, 0.0f, 255.0f);
    
    // combine with a basic background color that shifts over time
    uint8_t r = (uint8_t)(intensity * (sin(time) * 0.5f + 0.5f));
    uint8_t g = (uint8_t)(intensity * (cos(time) * 0.5f + 0.5f));
    uint8_t b = intensity;
    
    return rgba(r, g, b, 255);
}
// ---------------------------------------------------------

void setup(void) {
    set_fps(60);
}

void draw(void) {
    // w_ticks is in milliseconds, convert to seconds
    float time = w_ticks / 1000.0f;
    
    // Using direct pixel manipulation for maximum speed
    // screen.pixels is a void*, since we requested 32bpp it's an array of uint32_t
    uint32_t* vram = (uint32_t*)screen.pixels;
    int idx = 0;
    
    for (int y = 0; y < screen.height; ++y) {
        float v = (float)y / screen.height;
        for (int x = 0; x < screen.width; ++x) {
            float u = (float)x / screen.width;
            
            pixel_t color = my_shader(x, y, u, v, time);
            
            // Set the pixel
            vram[idx++] = color;
        }
    }
}
