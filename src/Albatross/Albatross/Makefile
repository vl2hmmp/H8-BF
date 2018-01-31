## motor_controller.c
## main.c
## utils.c
## parking.c
## line_trace.c
## press_button.c
All : alb.elf
alb.elf : motor_controller.o main.o utils.o parking.o line_trace.o press_button.o
	h8300-elf-gcc -Wl,--section-start=.stack=0x400 -B C:/wingcc/app/../local/h8300-elf -LC:/wingcc/app/ -mint32 -mh -T C:/wingcc/app/h8pic.x -nostartfiles C:/wingcc/app/h8piccrt0.S -o alb.elf  motor_controller.o main.o utils.o parking.o line_trace.o press_button.o -lmes_h8e
motor_controller.o : motor_controller.c
	h8300-elf-gcc -B C:/wingcc/app/../local/h8300-elf -IC:/wingcc/app/. -mint32 -mh -fPIC -O2 -c motor_controller.c
main.o : main.c
	h8300-elf-gcc -B C:/wingcc/app/../local/h8300-elf -IC:/wingcc/app/. -mint32 -mh -fPIC -O2 -c main.c
utils.o : utils.c
	h8300-elf-gcc -B C:/wingcc/app/../local/h8300-elf -IC:/wingcc/app/. -mint32 -mh -fPIC -O2 -c utils.c
parking.o : parking.c
	h8300-elf-gcc -B C:/wingcc/app/../local/h8300-elf -IC:/wingcc/app/. -mint32 -mh -fPIC -O2 -c parking.c
line_trace.o : line_trace.c
	h8300-elf-gcc -B C:/wingcc/app/../local/h8300-elf -IC:/wingcc/app/. -mint32 -mh -fPIC -O2 -c line_trace.c
press_button.o : press_button.c
	h8300-elf-gcc -B C:/wingcc/app/../local/h8300-elf -IC:/wingcc/app/. -mint32 -mh -fPIC -O2 -c press_button.c
