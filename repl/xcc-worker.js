import { WASI, File, Directory, OpenFile, ConsoleStdout, PreopenDirectory } from "https://unpkg.com/@bjorn3/browser_wasi_shim@0.3.0/dist/index.js";

self.onmessage = async (e) => {
    if (e.data.type !== 'compile') return;
    const sourceCode = e.data.code;
    const uploadedAssets = e.data.assets || {}; 
    
    try {
        self.postMessage({ type: 'log', data: 'Fetching sysroot...' });
        const sysrootRes = await fetch('sysroot.json');
        const sysrootFiles = await sysrootRes.json();
        
        function buildTree(pathsAndContents) {
            let root = new Map();
            for (const [path, base64Str] of Object.entries(pathsAndContents)) {
                const binString = atob(base64Str);
                const bytes = new Uint8Array(binString.length);
                for (let i = 0; i < binString.length; i++) bytes[i] = binString.charCodeAt(i);
                
                const parts = path.split('/').filter(p => p);
                let current = root;
                for (let i = 0; i < parts.length - 1; i++) {
                    if (!current.has(parts[i])) {
                        current.set(parts[i], new Map());
                    }
                    current = current.get(parts[i]);
                }
                current.set(parts[parts.length - 1], new File(bytes));
            }
            
            function toDirOrFile(map) {
                let entries = [];
                for (const [k, v] of map.entries()) {
                    if (v instanceof Map) {
                        entries.push([k, toDirOrFile(v)]);
                    } else {
                        entries.push([k, v]);
                    }
                }
                return new PreopenDirectory(".", entries).dir;
            }
            return toDirOrFile(root).contents;
        }
        
        const rootContents = buildTree(sysrootFiles);
        rootContents.set("tmp", new Directory(new Map()));
        
        rootContents.set("main.c", new File(new TextEncoder().encode(sourceCode)));
        
        let assetsH = "#pragma once\n";
        assetsH += "typedef struct { const char* path; const unsigned char* data; unsigned int size; } WagnerAsset;\n";
        
        let assetCount = 0;
        let hasPng = false;
        let hasAudio = false;
        
        for (const [filename, base64Str] of Object.entries(uploadedAssets)) {
            const binString = atob(base64Str);
            const bytes = new Uint8Array(binString.length);
            for (let i = 0; i < binString.length; i++) bytes[i] = binString.charCodeAt(i);
            
            let arrayStr = Array.from(bytes).join(', ');
            assetsH += `const unsigned char asset_${assetCount}[] = { ${arrayStr} };\n`;
            
            let lower = filename.toLowerCase();
            if (lower.endsWith('.png')) hasPng = true;
            if (lower.endsWith('.wav') || lower.endsWith('.ogg') || lower.endsWith('.mp3')) hasAudio = true;
            
            assetCount++;
        }
        
        assetsH += "static const WagnerAsset WAGNER_ASSETS[] = {\n";
        let i = 0;
        for (const filename of Object.keys(uploadedAssets)) {
            assetsH += `    {"${filename}", asset_${i}, sizeof(asset_${i})},\n`;
            i++;
        }
        if (i === 0) {
            assetsH += "    {0, 0, 0}\n";
        }
        assetsH += "};\n";
        assetsH += `static const int WAGNER_ASSET_COUNT = ${assetCount};\n`;
        
        rootContents.set("assets.h", new File(new TextEncoder().encode(assetsH)));
        
        let rootDirectory = new PreopenDirectory("/", Array.from(rootContents.entries()));

        const config = e.data.config || {};
        const title = config.title || "web_repl";
        const w = config.width || 320;
        const h = config.height || 240;
        const bpp = config.bpp || 32;
        const scale = config.scale || 2;
        const memoryBytes = config.memory || 16777216;
        const stackSize = memoryBytes;

        let args = [
            "cc",
            "-o", "/main.wasm",
            "-nostdlib",
            "-nodefaultlibs",
            "-I/sysroot/wagner/include",
            "-I/sysroot/wagner/lib/include",
            "-I/sysroot/wagner/lib/decoders",
            "-I/sysroot/include",
            "-I/", 
            `-DWAGNER_TITLE="${title}"`,
            `-DWAGNER_CFG_W=${w}`,
            `-DWAGNER_CFG_H=${h}`,
            `-DWAGNER_CFG_BPP=${bpp}`,
            `-DWAGNER_CFG_SCALE=${scale}`,
            "--entry-point=",
            "-ewupdate",
            `--stack-size=${stackSize}`
        ];
        
        if (hasPng) {
            args.push("-DLODEPNG_NO_COMPILE_DISK");
            args.push("-DLODEPNG_NO_COMPILE_ENCODER");
            args.push("-DLODEPNG_NO_COMPILE_ANCILLARY_CHUNKS");
            args.push("-DLODEPNG_NO_COMPILE_ERROR_TEXT");
            args.push("/sysroot/wagner/lib/decoders/lodepng.c");
        } else {
            args.push("-DWAGNER_NO_PNG_DECODE");
        }

        if (hasAudio) {
            args.push("-DSTB_VORBIS_NO_STDIO");
            args.push("/sysroot/wagner/lib/decoders/stb_vorbis.c");
        } else {
            args.push("-DWAGNER_NO_AUDIO_DECODE");
        }
        
        args.push("/sysroot/wagner/lib/shim/libc_shim.c");
        args.push("/main.c");
        
        self.postMessage({ type: 'log', data: 'Compiling with xcc: ' + args.join(' ') });

        let env = ["PWD=/"];
        let fds = [
            new OpenFile(new File([])), 
            ConsoleStdout.lineBuffered(msg => self.postMessage({ type: 'log', data: '[STDOUT] ' + msg })),
            ConsoleStdout.lineBuffered(msg => self.postMessage({ type: 'log', data: '[STDERR] ' + msg })),
            rootDirectory
        ];
        
        let wasi = new WASI(args, env, fds);
        
        self.postMessage({ type: 'log', data: 'Loading cc.wasm...' });
        let wasmRes = await fetch('cc.wasm');
        let wasmBytes = await wasmRes.arrayBuffer();
        let inst = await WebAssembly.instantiate(wasmBytes, {
            "wasi_snapshot_preview1": wasi.wasiImport,
        });
        
        self.postMessage({ type: 'log', data: 'Running compiler...' });
        let exitCode = wasi.start(inst.instance);
        self.postMessage({ type: 'log', data: 'Exit code: ' + exitCode });
        
        if (exitCode !== 0) {
            self.postMessage({ type: 'error', data: 'Compilation failed (code ' + exitCode + ')' });
            return;
        }
        
        let outFile = rootDirectory.dir.contents.get("main.wasm");
        if (!outFile || !outFile.data) {
            self.postMessage({ type: 'error', data: 'main.wasm was not generated!' });
            return;
        }
        
        self.postMessage({ type: 'log', data: 'ROM generated: ' + outFile.data.byteLength + ' bytes' });
        self.postMessage({ type: 'done', data: outFile.data });
        
    } catch (err) {
        self.postMessage({ type: 'error', data: err.toString() + '\n' + (err.stack || '') });
    }
};
