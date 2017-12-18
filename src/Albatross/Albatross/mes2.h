typedef struct {
	int	length;
	char	name[12];
	unsigned char	attr;
} DirEnt;

#define OptRead 1
#define OptWrite 2
#define OptRemove 3
#define OptAppend 4

struct sockaddr {
	unsigned int sin_addr;
	unsigned short sin_port;
};

#if defined(__H8300H__) || defined(__sh3__)
  typedef volatile unsigned char IOREG;
#endif
#if defined(__sh2__)
  typedef volatile unsigned short IOREG;
#endif

typedef struct {
	int (*open_dev)(int, int);
	int (*close_dev)(int);
	int (*write_dev)(int, char*, int);
	int (*read_dev)(int, char*, int);
	int (*seek_dev)(int, int);
	int (*ioctl_dev)(int, int, int);
	void (*poll_dev)();
	char name[8];
} Functions;

#if defined(__H8300H__)
  #define WAITCONST 1
#endif
#if defined(__sh__) || defined(__arm__)
  #define WAITCONST 12
#endif

#define SuperTask 1

#define USBNUM 8
#define SLNUM 2

#define SECSIZ 512

#define IPADDR(a,b,c,d) (unsigned int)a*0x1000000+b*0x10000+c*0x100+d
#define NOWEP 0
#define WEP64 1
#define WEP128 2
#define stderr 2

struct process {
	int address;
	char *cwd;
	char **argv;
	char flag;
	int stack;
	int pc;
	int tcb;
};

#define PS_WAIT 0x01

struct _packet{
	volatile struct _packet *next;
	int	stamp;
	char	data[4];
};
typedef volatile struct _packet Packet;

struct _packet2{
	volatile struct _packet2 *next;
	int	stamp, size, seq, retries;
	char	data[4];
};
typedef volatile struct _packet2 Packet2;

struct wi_usb_ident {
	unsigned short	vendor;
	unsigned short	product;
	char		*product_name;
};

#define	MALLOC_IP	0x4001
#define	MALLOC_WI	0x4002
#define	MALLOC_TCP	0x4003
#define	MALLOC_UDP	0x4004
#define	MALLOC_FAT	0x4005

#define	REGIST_STRING	1
#define	REGIST_DATA	2


#define DEV_INFO 0x01
#define GET_MAC 0x02
#define SET_HANDLE 0x03
#define HANDLE_ARG 0x04
#define MAXPACKET 0x05
#define DISK_CACHE 0x06
#define MEDIA_SIZE 0x07
#define DEV_RESET 0x08
#define DEV_INTERVAL 0x09
#define DEV_START 0x0a
#define DEV_STOP 0x0b
#define DEV_FLUSH 0x0c
#define DEV_BLOCK 0x0d
#define SCI_FREQ 0x101
#define SCI_SPEED 0x102
#define SCI_POLL 0x103
#define WI_GETBSSID 0x201
#define WI_SETWEPKEY 0x202
#define WI_GETWEPKEY 0x203
#define WI_SETWEP64 0x204
#define WI_SETWEP128 0x205
#define WI_WEPON 0x206
#define WI_SETSSID 0x207
#define WI_GETSSID 0x208
#define WI_PMGTON 0x209
#define WI_SETWPA 0x20a
#define WI_USBDOWN 0x20b
#define MMC_CSD 0x301
#define MMC_CID 0x302
#define USB_EPSIZE 0x401
#define USB_INFO 0x402
#define USB_SETCONF 0x403
#define USB_SETUP 0x404
#define USB_RESET 0x405
#define USB_GETSTATUS 0x406
#define USB_TIMEOUT 0x407
#define USB_INTERRUPT 0x408
#define USB_VENDOR 0x409
#define USB_PRODUCT 0x40a
#define USB_SET_EPSIZE 0x40b
#define USB_INTR_EP 0x40c
#define LCD_CLEAR 0x501
#define LCD_SETFONT 0x502


#define HW_CONFIG_TURBO 0x01

#define printf __printf
#define fprintf __fprintf
#define sprintf __sprintf
#define scanf __scanf
#define fscanf __fscanf
#define sscanf __sscanf
#define fputc __fputc
#define fgetc __fgetc
#define putchar __putchar
#define getchar __getchar
#define exec(argc, argv) execute(argc, argv, 0)
#define execfd(argc, argv, fd) execute(argc, argv, fd)
#define set_regist_string(name, str) set_regist(REGIST_STRING, name, str)
#define set_regist_data(name, value) set_regist(REGIST_DATA, name, (char*)value)
#define get_regist_string(name, str, maxsize) get_regist(REGIST_STRING, name, str, maxsize)
#define get_regist_data(name, valueptr) get_regist(REGIST_DATA, name, (char*)valueptr, 0)
#define delete_regist(name) set_regist(REGIST_STRING, name, 0)
#define list_regist_string(index, name) list_regist(REGIST_STRING, index, name)
#define list_regist_data(index, name) list_regist(REGIST_DATA, index, name)

void __printf(char*, ...);
void __fprintf(int, char*, ...);
void __sprintf(char*, char*, ...);
void __scanf(char*, ...);
void __fscanf(int, char*, ...);
void __sscanf(char*, char*, ...);
int __fputc(int, int);
int __fgetc(int);
int __putchar(int);
int __getchar(void);

int tftp_save(int, char*, void (*)(), int);
int tftp_load(int, char*, void (*)(), int);
int cgi_value(char*, char*, char*, int);
int gethostbyname(char*);
int strtoip(char*, int*);
#define	 open 	 __open
int	__open(char*, int);
#define	 close 	 __close
int	__close(int);
#define	 read 	 __read
int	__read(int, char*, int);
#define	 write 	 __write
int	__write(int, char*, int);
#define	 ioctl 	 __ioctl
int	__ioctl(int, int, int);
#define	 seek 	 __seek
int	__seek(int, int);
int	get_pid(void);
void	int_enable(void);
void	int_disable(void);
int	SetDirent(char*, DirEnt*);
int	NextRecord(int, DirEnt*);
int	DeleteFile(char*);
int	set_handler(int, void(*)());
int	execute(int, char**, int);
int	wait(int);
#define	 exit 	 __exit
int	__exit(int);
int	cd(char*);
#define	 cwd 	 __cwd
char*	__cwd(void);
#define	 malloc 	 __malloc
char*	__malloc(int);
#define	 free 	 __free
int	__free(char*);
int	sleep(int);
int	ifconfig(char*, int, int);
int	ifdown(int);
int	net_ioctl(int, int, int);
int	set_gateway(int);
int	ping(int, int);
int	udp_socket(void);
int	udp_free(int);
int	udp_bind(int, struct sockaddr*);
int	sendto(int, char*, int, struct sockaddr*);
int	recvfrom(int, char*, int, struct sockaddr*);
int	get_ipinfo(int, int*, int*, char*);
int	udp_timeout(int, int);
int	tcp_socket(int);
int	tcp_free(int);
int	tcp_bind(int, struct sockaddr*);
int	tcp_connect(int, struct sockaddr*);
int	tcp_listen(int, int);
int	tcp_accept(int, struct sockaddr*);
int	tcp_read(int, char*, int);
int	tcp_write(int, char*, int);
int	tcp_timeout(int, int);
int	shm_get(int, int);
char*	shm_at(int);
int	shm_dt(int);
int	mount(char*);
int	eject(char*);
int	format(char*);
int	setip(int, int, int);
int	GetFileSize(int);
int	tcp_change_taskid(int);
int	tcp_close(int);
int	dns_server(int);
int	get_netnum(char*);
int	dhcp_server(int, int);
int	random(void);
int	usb_mount(int);
int	usb_eject(int);
int	ps_info(int, struct process*);
#define	 kill 	 __kill
int	__kill(int);
int	mesver(void);
int	usb_setup(int, char*, char*);
int	usb_write(int, char*, int, int);
int	usb_read(int, char*, int, int);
int	tcp_status(int);
int	fat_status(int, int*, int*, char*);
int	ret_value(void);
void	set_buswidth(int, int, int);
int	add_device(void*);
void	ddr_set(int, int);
void	ddr_reset(int, int);
int	set_regist(int, char*, char*);
int	get_regist(int, char*, char*, int);
int	list_regist(int, int, char*);
int	hw_config(int, int, int);
