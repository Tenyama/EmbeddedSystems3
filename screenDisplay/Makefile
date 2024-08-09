#--------------------------------------Makefile-------------------------------------

CFILES = $(wildcard *.c)
OFILES = $(CFILES:%.c=%.o)
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib

all: clean kernel8.img run1
uart1: clean uart1_build kernel8.img run1
# uart0: clean uart0_build kernel8.img run0

#./build/uart.o: ./uart/uart1.c
#	aarch64-none-elf-gcc $(GCCFLAGS) -c ./uart/uart1.c -o ./build/uart.o

# uart1_build: uart1.c
# 	aarch64-none-elf-gcc $(GCCFLAGS) -c uart1.c -o uart.o

# uart0_build: ./uart/uart0.c
# 	aarch64-none-elf-gcc $(GCCFLAGS) -c ./uart/uart0.c -o ./build/uart.o

boot.o: boot.S
	aarch64-none-elf-gcc $(GCCFLAGS) -c boot.S -o boot.o

%.o: %.c
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

kernel8.img: boot.o $(OFILES)
	aarch64-none-elf-ld -nostdlib boot.o $(OFILES) -T link.ld -o kernel8.elf
	aarch64-none-elf-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm -f kernel8.elf *.o *.img

# Run emulation with QEMU
run1: 
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial null -serial stdio

run0: 
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial stdio
