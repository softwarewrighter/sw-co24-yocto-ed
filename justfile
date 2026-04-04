# sw-co24-yocto-ed -- Minimal Modal Text Editor for COR24

set quiet

tc24r := "tc24r"
cor24_run := "cor24-run"
testfile := "tests/testfile.txt"

build:
    mkdir -p build
    {{tc24r}} src/swye.c -o build/swye.s

run: build
    {{cor24_run}} --run build/swye.s --load-binary {{testfile}}@0x10000 --terminal --echo --speed 0

demo-cursor: build
    {{cor24_run}} --run build/swye.s --load-binary {{testfile}}@0x10000 -u $'\x1b8 right\n' --speed 0 -n 100000000

demo-cmd-mode: build
    {{cor24_run}} --run build/swye.s --load-binary {{testfile}}@0x10000 -u $'\x1b8 right\n' --speed 0 -n 100000000

demo-edit-mode: build
    {{cor24_run}} --run build/swye.s --load-binary {{testfile}}@0x10000 -u $'\x1b8 right\n\x1b' --speed 0 -n 200000000

test:
    reg-rs run -p yocto_ed

test:
    reg-rs run -p yocto_ed

clean:
    rm -rf build
