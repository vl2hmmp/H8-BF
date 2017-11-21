## reset.c
All : reset.elf
reset.elf : reset.o
	h8300-elf-gcc -Wl,--section-start=.stack=0x400 -B C:/wingcc/app/../local/h8300-elf -LC:/wingcc/app/ -mint32 -mh -T C:/wingcc/app/h8pic.x -nostartfiles C:/wingcc/app/h8piccrt0.S -o reset.elf  reset.o -lmes_h8e
reset.o : reset.c
	h8300-elf-gcc -B C:/wingcc/app/../local/h8300-elf -IC:/wingcc/app/. -mint32 -mh -fPIC -O2 -c reset.c
