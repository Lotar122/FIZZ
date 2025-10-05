#!/usr/bin/env python3
import os
import subprocess
import sys
from pathlib import Path
import sys

# === CONFIGURATION ===

CC = os.environ.get("CC", "arm-none-eabi-gcc")
CXX = os.environ.get("CXX", "arm-none-eabi-g++")
OBJCOPY = os.environ.get("OBJCOPY", "arm-none-eabi-objcopy")

os.makedirs("build", exist_ok=True)
os.makedirs("build/obj", exist_ok=True)

PROJECT_ROOT = Path(__file__).parent.resolve()
SRC_DIR = PROJECT_ROOT / "src"
LINKER_SCRIPT = PROJECT_ROOT / "linker_STM32L476RGTX.ld"
STARTUP_FILE = PROJECT_ROOT / "startup_STM32L476RGTX.s"
OUTPUT_ELF = PROJECT_ROOT / "./build/firmware.elf"
OUTPUT_BIN = PROJECT_ROOT / "./build/firmware.bin"
OUTPUT_HEX = PROJECT_ROOT / "./build/firmware.hex"

COMMON_FLAGS = [
    "-mcpu=cortex-m4",
    "-mthumb",
    "-mfpu=fpv4-sp-d16",
    "-mfloat-abi=hard",
    "-ffreestanding",
    "-nostdlib",
    "-Wall",
    "-O2",
    f"-I{SRC_DIR}",
    f"-I{SRC_DIR}/CMSISHeaders"
]

CXX_FLAGS = [
    "-fno-rtti",
    "-fno-exceptions"
]

# === DISCOVER SOURCES ===

def find_sources(suffix):
    return [str(path) for path in SRC_DIR.rglob(suffix)]

# === BUILD ===

def compile_c_objects(sources):
    objects = []
    for src in sources:
        obj = Path(src).with_suffix(".o")
        directory = Path(STARTUP_FILE.name).resolve().parent
        if sys.platform.startswith("win"):
            cmd = [CC, *COMMON_FLAGS, "-c", src, "-o", f"{str(directory)}\\build\\obj\\{str(Path(obj.name).name)}"]
        else:
            cmd = [CC, *COMMON_FLAGS, "-c", src, "-o", f"{str(directory)}/build/obj/{str(Path(obj.name).name)}"]
        print("Compiling:", " ".join(cmd))
        subprocess.check_call(cmd)
        if sys.platform.startswith("win"):
            objects.append(f"{str(directory)}\\build\\obj\\{str(Path(obj.name).name)}")
        else:
            objects.append(f"{str(directory)}/build/obj/{str(Path(obj.name).name)}")
    return objects

def compile_cxx_objects(sources):
    objects = []
    for src in sources:
        obj = Path(src).with_suffix(".o")
        directory = Path(STARTUP_FILE.name).resolve().parent
        if sys.platform.startswith("win"):
            cmd = [CXX, *COMMON_FLAGS, *CXX_FLAGS, "-c", src, "-o", f"{str(directory)}\\build\\obj\\{str(Path(obj.name).name)}"]
        else:
            cmd = [CXX, *COMMON_FLAGS, *CXX_FLAGS, "-c", src, "-o", f"{str(directory)}/build/obj/{str(Path(obj.name).name)}"]
        print("Compiling:", " ".join(cmd))
        subprocess.check_call(cmd)
        if sys.platform.startswith("win"):
            objects.append(f"{str(directory)}\\build\\obj\\{str(Path(obj.name).name)}")
        else:
            objects.append(f"{str(directory)}/build/obj/{str(Path(obj.name).name)}")
    return objects

def compile_startup():
    obj = STARTUP_FILE.with_suffix(".o")
    directory = Path(STARTUP_FILE.name).resolve().parent
    if sys.platform.startswith("win"):
        cmd = [CC, *COMMON_FLAGS, "-c", str(STARTUP_FILE), "-o", f"{str(directory)}\\build\\obj\\{str(Path(obj.name).name)}"]
    else:
        cmd = [CC, *COMMON_FLAGS, "-c", str(STARTUP_FILE), "-o", f"{str(directory)}/build/obj/{str(Path(obj.name).name)}"]
    print("Compiling startup:", " ".join(cmd))
    subprocess.check_call(cmd)
    if sys.platform.startswith("win"):
        return f"{str(directory)}\\build\\obj\\{str(Path(obj.name).name)}"
    else:
        return f"{str(directory)}/build/obj/{str(Path(obj.name).name)}"

def link(objects):
    cmd = [CXX, *COMMON_FLAGS, *CXX_FLAGS, "-T", str(LINKER_SCRIPT), *objects, "-o", str(OUTPUT_ELF)]
    print("Linking:", " ".join(cmd))
    subprocess.check_call(cmd)

def convert_outputs():
    subprocess.check_call([OBJCOPY, "-O", "binary", str(OUTPUT_ELF), str(OUTPUT_BIN)])
    subprocess.check_call([OBJCOPY, "-O", "ihex", str(OUTPUT_ELF), str(OUTPUT_HEX)])
    print("✅ Built", OUTPUT_BIN, OUTPUT_HEX)

# === MAIN ===

def main():
    c_sources = find_sources("*.c")
    c_objects = compile_c_objects(c_sources)
    cxx_sources = find_sources("*.cpp")
    cxx_objects = compile_cxx_objects(cxx_sources)
    objects = c_objects
    objects.append(compile_startup())
    objects.extend(cxx_objects)
    link(objects)
    convert_outputs()

if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as e:
        print("❌ Build failed:", e)
        sys.exit(1)
