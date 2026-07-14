const fs = require('fs');
const { WASI, File, Directory, OpenFile, ConsoleStdout, PreopenDirectory } = require("@bjorn3/browser_wasi_shim");

(async () => {
    const sysrootFiles = JSON.parse(fs.readFileSync('sysroot.json'));
    let root = new Map();
    for (const [path, base64Str] of Object.entries(sysrootFiles)) {
        const bytes = Buffer.from(base64Str, 'base64');
        const parts = path.split('/').filter(p => p);
        let current = root;
        for (let i = 0; i < parts.length - 1; i++) {
            if (!current.has(parts[i])) current.set(parts[i], new Map());
            current = current.get(parts[i]);
        }
        current.set(parts[parts.length - 1], new File(bytes));
    }
    
    function toDirOrFile(map) {
        let entries = [];
        for (const [k, v] of map.entries()) {
            if (v instanceof Map) entries.push([k, toDirOrFile(v)]);
            else entries.push([k, v]);
        }
        return new PreopenDirectory(".", entries).dir;
    }
    const rootContents = toDirOrFile(root).contents;
    
    rootContents.set("tmp", new Directory(new Map()));
    rootContents.set("main.c", new File(new TextEncoder().encode(`
        #include "wagner.h"
        void preload() {}
        void setup() {}
        void draw() {}
        void mouse_pressed() {}
        void mouse_released() {}
        void key_pressed(int key) {}
        void key_released(int key) {}
        int _start() { lodepng_decode32(0,0,0,0,0); return 0; }
    `)));
    rootContents.set("assets.h", new File(new TextEncoder().encode(`
        #pragma once
        typedef struct { const char* path; const unsigned char* data; unsigned int size; } WagnerAsset;
        static const WagnerAsset WAGNER_ASSETS[] = { {0, 0, 0} };
        static const int WAGNER_ASSET_COUNT = 0;
    `)));
    
    let rootDirectory = new PreopenDirectory("/", Array.from(rootContents.entries()));

    let args = [
        "cc", "-o", "/main.wasm", "-nostdlib", "-nodefaultlibs",
        "-I/sysroot/wagner/include", "-I/sysroot/wagner/lib/include", "-I/sysroot/wagner/lib/decoders",
        "-I/sysroot/include", "-I/", 
        "-DLODEPNG_NO_COMPILE_DISK", "-DLODEPNG_NO_COMPILE_ENCODER", 
        "-DLODEPNG_NO_COMPILE_ANCILLARY_CHUNKS", "-DLODEPNG_NO_COMPILE_ERROR_TEXT",
        "/sysroot/wagner/lib/decoders/lodepng.c",
        "/sysroot/wagner/lib/shim/libc_shim.c", "/main.c"
    ];
    
    let env = ["PWD=/"];
    let fds = [
        new OpenFile(new File([])), 
        ConsoleStdout.lineBuffered(msg => console.log('[STDOUT]', msg)),
        ConsoleStdout.lineBuffered(msg => console.error('[STDERR]', msg)),
        rootDirectory
    ];
    
    let wasi = new WASI(args, env, fds);
    let wasmBytes = fs.readFileSync('cc.wasm');
    let inst = await WebAssembly.instantiate(wasmBytes, {
        "wasi_snapshot_preview1": wasi.wasiImport,
    });
    
    let exitCode = wasi.start(inst.instance);
    console.log('Exit code:', exitCode);
})();
