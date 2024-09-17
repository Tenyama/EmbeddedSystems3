#--------------------------------------Makefile-------------------------------------

CFILES = $(wildcard ./kernel/*.c) $(wildcard ./game/*.c)
OFILES = $(CFILES:./kernel/%.c=./build/kernel/%.o)
OFILES := $(OFILES:./game/%.c=./build/game/%.o)
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib

all: clean uart1_build kernel8.img run1
uart1: clean uart1_build kernel8.img run1
uart0: clean uart0_build kernel8.img run0

uart1_build: ./uart/uart1.c
	aarch64-none-elf-gcc $(GCCFLAGS) -c ./uart/uart1.c -o ./build/uart.o

uart0_build: ./uart/uart0.c
	aarch64-none-elf-gcc $(GCCFLAGS) -c ./uart/uart0.c -o ./build/uart.o

./build/boot.o: ./kernel/boot.S
	aarch64-none-elf-gcc $(GCCFLAGS) -c ./kernel/boot.S -o ./build/boot.o

# For kernel and game files
./build/kernel/%.o: ./kernel/%.c
	mkdir -p ./build/kernel
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

./build/game/%.o: ./game/%.c
	mkdir -p ./build/game
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

kernel8.img: ./build/boot.o ./build/uart.o $(OFILES)
	aarch64-none-elf-ld -nostdlib ./build/boot.o ./build/uart.o $(OFILES) -T ./kernel/link.ld -o ./build/kernel8.elf
	aarch64-none-elf-objcopy -O binary ./build/kernel8.elf kernel8.img

clean:
	rm -f ./build/kernel8.elf ./build/*.o ./build/kernel/*.o ./build/game/*.o *.img

# Run emulation with QEMU
run1: 
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial null -serial stdio

run0: 
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial stdio
