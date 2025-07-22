# 
#   Created by Matt Hartley on 20/07/2025.
#   Copyright 2025 GiantJelly. All rights reserved.
# 

mkdir -p ./build
rm ./build/*

compile=/opt/libdragon/bin/mips64-elf-gcc
link=/opt/libdragon/bin/mips64-elf-ld
objcopy=/opt/libdragon/bin/mips64-elf-objcopy

n64tool=/opt/libdragon/bin/n64tool
n64crc=/opt/libdragon/bin/chksum64
n64elfcompress=/opt/libdragon/bin/n64elfcompress

flags="-c -g3 -G 0 -nostdlib -nostartfiles -ffreestanding -nodefaultlibs -fno-builtin-memset"
linkFlags="-T linker.ld -nostdlib"

$compile $flags main.c -o ./build/main.o
$compile $flags boot.s -o ./build/boot.o

$link $linkFlags ./build/main.o ./build/boot.o -o ./build/main.elf -Map ./build/build.map

# $objcopy -O binary ./build/main.elf ./build/main.bin

# $n64elfcompress -o ./build/ -c 1 ./build/main.elf

# $n64tool -l 0x80000400 -o ./build/main.z64 ./build/main.bin
$n64tool --toc --output ./build/main.z64 --align 256 ./build/main.elf --align 8

$objcopy -I binary -O binary --reverse-bytes=2 ./build/main.z64 ./build/main.v64

echo "\ndone."


# $n64crc main.z64 main.crc.z64


# $objcopy -I binary -O binary --reverse-bytes=2 ./build/main.bin ./build/main.v64

# $link 

# /opt/libdragon/bin/mips64-elf-objdump -d -h -f ./build/main.elf
# /opt/libdragon/bin/mips64-elf-readelf ./build/main.elf -l

# echo "\n---";
# /opt/libdragon/bin/mips64-elf-objdump -f ../libdragon/examples/test/build/test.elf
# /opt/libdragon/bin/mips64-elf-readelf ../libdragon/examples/test/build/test.elf -l
