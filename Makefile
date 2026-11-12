GPPPARAMS = -m32 -Iinclude -O2 -Wall -Wextra \
            -fno-use-cxa-atexit -nostdlib -fno-builtin \
            -fno-rtti -fno-exceptions -fno-leading-underscore \
            -Wno-write-strings -fno-stack-protector \
            -fno-pie -fno-pic \
            -mno-sse -mno-sse2 -mno-mmx -mno-80387

ASPARAMS = --32
LDPARAMS = -m elf_i386 -z max-page-size=0x1000


objects = build/obj/boot/loader.o \
          build/obj/common/string.o \
          build/obj/hardware/port.o \
          build/obj/hardware/gdt.o \
          build/obj/hardware/interruptstubs.o \
          build/obj/hardware/interrupts.o \
          build/obj/hardware/pci.o \
          build/obj/hardware/timer.o \
          build/obj/memory/heap.o \
          build/obj/drivers/driver.o \
          build/obj/drivers/vesa.o \
          build/obj/drivers/keyboard.o \
          build/obj/drivers/mouse.o \
          build/obj/gui/widget.o \
          build/obj/gui/window.o \
          build/obj/gui/taskbar.o \
          build/obj/gui/desktop.o \
          build/obj/apps/terminal.o \
          build/obj/apps/notepad.o \
          build/obj/apps/paint.o \
          build/obj/apps/sysmon.o \
          build/obj/kernel.o

all: build/myos.iso

build/obj/%.o: src/%.cpp
	mkdir -p $(@D)
	g++ $(GPPPARAMS) -c -o $@ $<


build/obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<


build/mykernel.bin: linker.ld $(objects)
	mkdir -p build
	ld $(LDPARAMS) -T $< -o $@ $(objects)


build/myos.iso: build/mykernel.bin
	mkdir -p build/isodir/boot/grub
	cp build/mykernel.bin build/isodir/boot/mykernel.bin
	echo 'set timeout=0' > build/isodir/boot/grub/grub.cfg
	echo 'set default=0' >> build/isodir/boot/grub/grub.cfg
	echo '' >> build/isodir/boot/grub/grub.cfg
	echo 'menuentry "My Custom OS (1024x768x32)" {' >> build/isodir/boot/grub/grub.cfg
	echo '    set gfxpayload=1024x768x32' >> build/isodir/boot/grub/grub.cfg
	echo '    multiboot /boot/mykernel.bin' >> build/isodir/boot/grub/grub.cfg
	echo '    boot' >> build/isodir/boot/grub/grub.cfg
	echo '}' >> build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o build/myos.iso build/isodir


run: build/myos.iso
	qemu-system-i386 -cdrom build/myos.iso -m 256M

clean:
	rm -rf build/obj build/isodir build/mykernel.bin build/myos.iso

.PHONY: all run clean