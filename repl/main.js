let editor;
let compiledWasmBuffer = null;
let uploadedAssets = {}; // { filename: base64 }

const files = {
    "main.c": `#include "wagner.h"

void preload() {
    // load_image(&img, "test.png");
}

void setup() {
    set_fps(60);
}

void draw() {
    push(); fill(rgb(30, 30, 40)); clear(); pop();
    push(); translate(100, 110); fill(WHITE); text("Wagner REPL"); pop();
}`,
    "wagner.json": `{
  "title": "web_repl",
  "width": 320,
  "height": 240,
  "bpp": 32,
  "scale": 2,
  "memory": 16777216
}`
};

let currentFile = "main.c";

// Setup Editor
window.onload = () => {
    editor = monaco.editor.create(document.getElementById('monaco-root'), {
        value: files[currentFile],
        language: 'c',
        theme: 'vs-dark',
        minimap: { enabled: false },
        fontSize: 14,
        automaticLayout: true
    });

    editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.Enter, () => {
        compileCode();
    });

    editor.onDidChangeModelContent(() => {
        files[currentFile] = editor.getValue();
    });

    document.querySelectorAll('.tab').forEach(tab => {
        tab.onclick = () => {
            document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
            tab.classList.add('active');
            
            const newFile = tab.getAttribute('data-file');
            if (newFile !== currentFile) {
                currentFile = newFile;
                const lang = currentFile.endsWith('.json') ? 'json' : 'c';
                monaco.editor.setModelLanguage(editor.getModel(), lang);
                editor.setValue(files[currentFile]);
            }
        };
    });
};

const logEl = document.getElementById('log');
function log(msg, type='normal') {
    const div = document.createElement('div');
    div.textContent = msg;
    if (type === 'error') div.className = 'log-error';
    if (type === 'success') div.className = 'log-success';
    logEl.appendChild(div);
    logEl.scrollTop = logEl.scrollHeight;
}

// Assets UI
document.getElementById('asset-upload').onchange = async (e) => {
    for (const file of e.target.files) {
        const reader = new FileReader();
        reader.onload = (ev) => {
            const base64 = btoa(new Uint8Array(ev.target.result).reduce((data, byte) => data + String.fromCharCode(byte), ''));
            uploadedAssets[file.name] = base64;
            updateAssetList();
        };
        reader.readAsArrayBuffer(file);
    }
    e.target.value = '';
};

function updateAssetList() {
    const lst = document.getElementById('assets-list');
    lst.innerHTML = '';
    const keys = Object.keys(uploadedAssets);
    if (keys.length === 0) {
        lst.innerHTML = '<div style="color: #666; font-size: 12px; text-align: center; padding: 10px;">No assets added.</div>';
        return;
    }
    for (const name of keys) {
        const div = document.createElement('div');
        div.className = 'asset-item';
        div.innerHTML = `
            <span>${name}</span> 
            <div style="display:flex; gap: 8px; align-items: center;">
                <span style="cursor:pointer; color: #888;" onclick="renameAsset('${name}')" title="Rename">✎</span>
                <span class="asset-remove" onclick="removeAsset('${name}')" title="Remove">✕</span>
            </div>
        `;
        lst.appendChild(div);
    }
}

window.renameAsset = (oldName) => {
    const newName = prompt("Rename asset:", oldName);
    if (newName && newName !== oldName && newName.trim() !== '') {
        const cleanName = newName.trim();
        if (uploadedAssets[cleanName]) {
            alert("An asset with this name already exists!");
            return;
        }
        uploadedAssets[cleanName] = uploadedAssets[oldName];
        delete uploadedAssets[oldName];
        updateAssetList();
    }
};

window.removeAsset = (name) => {
    delete uploadedAssets[name];
    updateAssetList();
};

// Compilation
let worker = null;
function compileCode() {
    logEl.innerHTML = '';
    log("Starting build...", 'normal');
    
    let config;
    try {
        config = JSON.parse(files['wagner.json']);
    } catch(e) {
        log("Error parsing wagner.json: " + e.message, 'error');
        return;
    }

    if (worker) worker.terminate();
    worker = new Worker('xcc-worker.js', { type: "module" });
    
    worker.onmessage = (e) => {
        if (e.data.type === 'log') {
            log(e.data.data, 'normal');
        } else if (e.data.type === 'error') {
            log(e.data.data, 'error');
        } else if (e.data.type === 'done') {
            log("Build successful! Running...", 'success');
            compiledWasmBuffer = e.data.data;
            document.getElementById('btn-save').style.display = 'flex';
            runWasm();
        }
    };
    
    worker.postMessage({
        type: 'compile',
        code: files['main.c'],
        assets: uploadedAssets,
        config: config
    });
}

document.getElementById('btn-compile').onclick = compileCode;

document.getElementById('btn-save').onclick = () => {
    if (!compiledWasmBuffer) return;
    const blob = new Blob([compiledWasmBuffer], { type: 'application/wasm' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'main.wasm';
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
};

// Runner
function runWasm() {
    if (!compiledWasmBuffer) return;
    if (window.wagnosticLoadRomFromBuffer) {
        const bufferCopy = new Uint8Array(compiledWasmBuffer).buffer;
        window.wagnosticLoadRomFromBuffer(bufferCopy);
    } else {
        log("Runner script not loaded properly", 'error');
    }
}
