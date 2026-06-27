#include "wagn0.h"
#include "data/sprites.h"

#define SPRITE_FLOOR   10
#define SPRITE_WALL    25
#define SPRITE_PLAYER  1
#define SPRITE_MONSTER 45
#define GRID_W 20
#define GRID_H 15
#define CELL 16

static uint8_t tiles[GRID_H][GRID_W];
typedef struct { int x, y, hp; } Entity;
static Entity player;
#define MAX_MONSTERS 10
static Entity monsters[MAX_MONSTERS];
static int num_monsters = 0;
static uint32_t _seed = 12345;
static uint32_t rand_u32() { _seed = _seed * 1103515245 + 12345; return (_seed / 65536) % 32768; }
static int rand_range(int min, int max) { if (max <= min) return min; return min + (rand_u32() % (max - min)); }

typedef struct { int x, y, w, h; } Room;
static Room rooms[15];
static int num_rooms = 0;

// Sprite sheet utility: get a Wagn0Image pointing to a 16x16 cell in the sheet
static Wagn0Image sprite_cell(int id) {
    int sx = (id % 20) * CELL, sy = (id / 20) * CELL;
    Wagn0Image img;
    img.pixels = (uint16_t*)image_raw + sy * image_width + sx;
    img.width = CELL; img.height = CELL; img.bpp = 16;
    return img;
}

static void build_room(Room *r) {
    for (int y = r->y; y < r->y + r->h; y++)
        for (int x = r->x; x < r->x + r->w; x++)
            tiles[y][x] = 0;
}

static void add_h_corridor(int x1, int x2, int y) {
    int a = x1 < x2 ? x1 : x2, b = x1 < x2 ? x2 : x1;
    for (int x = a; x <= b; x++) tiles[y][x] = 0;
}

static void add_v_corridor(int y1, int y2, int x) {
    int a = y1 < y2 ? y1 : y2, b = y1 < y2 ? y2 : y1;
    for (int y = a; y <= b; y++) tiles[y][x] = 0;
}

static bool overlaps(Room *r, Room *list, int n) {
    for (int i = 0; i < n; i++) {
        if (r->x < list[i].x + list[i].w && r->x + r->w > list[i].x &&
            r->y < list[i].y + list[i].h && r->y + r->h > list[i].y) return true;
    } return false;
}

static void generate_map() {
    for (int y = 0; y < GRID_H; y++)
        for (int x = 0; x < GRID_W; x++) tiles[y][x] = 1;
    num_rooms = 0; num_monsters = 0;
    for (int i = 0; i < 30; i++) {
        Room r = { rand_range(1, GRID_W-8), rand_range(1, GRID_H-6), rand_range(4, 8), rand_range(3, 6) };
        if (!overlaps(&r, rooms, num_rooms)) {
            int px = r.x + r.w/2, py = r.y + r.h/2;
            if (num_rooms > 0) {
                int ox = rooms[num_rooms-1].x + rooms[num_rooms-1].w/2;
                int oy = rooms[num_rooms-1].y + rooms[num_rooms-1].h/2;
                add_h_corridor(ox, px, oy); add_v_corridor(oy, py, px);
            }
            rooms[num_rooms++] = r; build_room(&r);
        }
    }
    player.x = rooms[0].x + rooms[0].w/2; player.y = rooms[0].y + rooms[0].h/2; player.hp = 10;
    for (int i = 1; i < num_rooms; i++) {
        if (num_monsters < MAX_MONSTERS) {
            monsters[num_monsters].x = rooms[i].x + rooms[i].w/2;
            monsters[num_monsters].y = rooms[i].y + rooms[i].h/2;
            monsters[num_monsters].hp = 3; num_monsters++;
        }
    }
}

static uint32_t prev_buttons = 0;

void setup() {
    w_setup("Roguelike", 320, 240, 16, 4);
    generate_map();
}

void draw() {
    uint32_t pressed = w_gamepad_buttons & ~prev_buttons;
    prev_buttons = w_gamepad_buttons;
    
    int dx = 0, dy = 0;
    if (pressed & W_BTN_LEFT) dx = -1; if (pressed & W_BTN_RIGHT) dx = 1;
    if (pressed & W_BTN_UP) dy = -1; if (pressed & W_BTN_DOWN) dy = 1;
    
    if (dx != 0 || dy != 0) {
        int nx = player.x + dx; int ny = player.y + dy;
        if (nx >= 0 && nx < GRID_W && ny >= 0 && ny < GRID_H && tiles[ny][nx] == 0) {
            bool monster_there = false;
            for (int i = 0; i < num_monsters; i++)
                if (monsters[i].hp > 0 && monsters[i].x == nx && monsters[i].y == ny)
                    { monsters[i].hp--; monster_there = true; break; }
            if (!monster_there) { player.x = nx; player.y = ny; }
            for (int i = 0; i < num_monsters; i++) {
                if (monsters[i].hp > 0) {
                    int mdx = (player.x > monsters[i].x) ? 1 : (player.x < monsters[i].x ? -1 : 0);
                    int mdy = (player.y > monsters[i].y) ? 1 : (player.y < monsters[i].y ? -1 : 0);
                    int mnx = monsters[i].x + mdx; int mny = monsters[i].y + mdy;
                    if (mnx == player.x && mny == player.y) player.hp--;
                    else if (tiles[mny][mnx] == 0) { monsters[i].x = mnx; monsters[i].y = mny; }
                }
            }
        }
    }

    background(rgb(10, 10, 15));
    int ox = (320 - GRID_W * CELL) / 2, oy = (240 - GRID_H * CELL) / 2;
    
    for (int y = 0; y < GRID_H; y++)
        for (int x = 0; x < GRID_W; x++)
            image_scaled(sprite_cell(tiles[y][x] == 1 ? SPRITE_WALL : SPRITE_FLOOR),
                        ox + x*CELL, oy + y*CELL, CELL, CELL);
    
    image_scaled(sprite_cell(SPRITE_PLAYER), ox + player.x*CELL, oy + player.y*CELL, CELL, CELL);
    
    for (int m = 0; m < num_monsters; m++)
        if (monsters[m].hp > 0)
            image_scaled(sprite_cell(SPRITE_MONSTER), ox + monsters[m].x*CELL, oy + monsters[m].y*CELL, CELL, CELL);
}
