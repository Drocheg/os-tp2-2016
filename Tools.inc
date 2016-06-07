#Where your cross C compiler is (target = x86_64-elf)
GCC=gcc
#Where your cross linker is
LD=ld
#Where your cross AR thingy is
AR=ar
#Where your NASM is (the cross AS assembler doesn't seem to work)
ASM=nasm
#ASM=/home/opt/xc/bin/x86_64-elf-as
#Where your QEMU image making program is
QEMU_IMG=qemu-img

#No need to change these
GCCFLAGS=-m64 -fno-exceptions -fno-asynchronous-unwind-tables -mno-mmx -mno-sse -mno-sse2 -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc -mno-red-zone -Wall -ffreestanding -nostdlib -fno-common -std=c99
ARFLAGS=rvs
ASMFLAGS=-felf64
#Note: -z option fails under Cygwin if not using a cross-compiler with an ELF target, see https://cygwin.com/ml/cygwin/2010-01/msg00719.html
LDFLAGS=--warn-common -z max-page-size=0x1000