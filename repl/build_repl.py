import os
import json
import base64
import urllib.request
import shutil

print("Building Wagner Web REPL...")

# 1. Fetch cc.wasm if missing
if not os.path.exists("cc.wasm"):
    print("Downloading cc.wasm...")
    url = "https://raw.githubusercontent.com/jardimdanificado/wagnostic/master/runners/web/cc.wasm"
    try:
        urllib.request.urlretrieve(url, "cc.wasm")
        print("cc.wasm downloaded successfully.")
    except Exception as e:
        print("Could not download from raw.githubusercontent.com. Falling back to local copy if available.")
        if os.path.exists("../../runners/web/cc.wasm"):
            shutil.copy("../../runners/web/cc.wasm", "cc.wasm")
            print("Copied cc.wasm from runners/web")
        else:
            print("WARNING: cc.wasm not found. REPL will not work.")

# 2. Load base sysroot.json
base_sysroot = {}
if os.path.exists("../../runners/web/sysroot.json"):
    print("Loading base sysroot.json...")
    with open("../../runners/web/sysroot.json", "r") as f:
        base_sysroot = json.load(f)
else:
    print("WARNING: Base sysroot.json not found in ../../runners/web/")

# 3. Add Wagner files
def add_directory_to_sysroot(sysroot_dict, disk_path, sysroot_prefix):
    if not os.path.exists(disk_path): return
    for root, dirs, files in os.walk(disk_path):
        for f in files:
            if f.endswith(".h") or f.endswith(".c"):
                full = os.path.join(root, f)
                rel = os.path.relpath(full, disk_path)
                # Ensure forward slashes for WASI
                sysroot_path = os.path.join(sysroot_prefix, rel).replace('\\', '/')
                with open(full, "rb") as file_data:
                    sysroot_dict[sysroot_path] = base64.b64encode(file_data.read()).decode("utf-8")
                print(f"Added {sysroot_path}")

print("Adding Wagner headers and shim...")
add_directory_to_sysroot(base_sysroot, "../include", "sysroot/wagner/include")
add_directory_to_sysroot(base_sysroot, "../lib/include", "sysroot/wagner/lib/include")
add_directory_to_sysroot(base_sysroot, "../lib/decoders", "sysroot/wagner/lib/decoders")
add_directory_to_sysroot(base_sysroot, "../lib/shim", "sysroot/wagner/lib/shim")

# 4. Save new sysroot.json
print("Saving repl sysroot.json...")
with open("sysroot.json", "w") as f:
    json.dump(base_sysroot, f)

# 5. Copy runner.js from wagnostic web runner
if os.path.exists("../../runners/web/runner.js"):
    shutil.copy("../../runners/web/runner.js", "runner.js")
    print("Copied runner.js")
else:
    print("WARNING: runner.js not found.")

print("Done! You can now start 'python3 serve.py'")
