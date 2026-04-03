# sw-co24-yocto-ed -- Minimal Modal Text Editor for COR24

set quiet

tc24r := "tc24r"
cor24_run := "cor24-run"

build:
    mkdir -p build
    {{tc24r}} src/main.c src/editor.c src/buffer.c src/command.c src/render.c src/platform_uart.c -Iinclude -o build/swye.s

run: build
    {{cor24_run}} --run build/swye.s --terminal --echo --speed 0

test:
    reg-rs run -p yocto_ed

clean:
    rm -rf build
