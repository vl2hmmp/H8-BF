// ****************************************
// *** �t�@�C�����o�͂̓���m�F         ***
// ***    produced by Y. Mori           ***
// ***       special thanks to A. Ruike ***
// ****************************************
// type data.txt
// �ŁCH8�̃t�@�C���𒆂������D
//
// tftp - 192.168.1.1 data.txt 
// �ŁCPC�̃v���O�����̂���t�H���_�Ƀt�@�C�����]�������D

#include <mes2.h>

/* ************ */
/* *** main *** */
/* ************ */
int
main(void)
{
	static int   fd;
	static char  buf[256];  // static �����Ȃ��ƁC�T�C�Y���傫���Ƃ��ɃR���p�C���G���[���N����
	int  i;

	fd= open("data.txt", OptWrite);

	if( fd == -1 ){
		printf(" File open error! \n");
		exit(-1);
	}
	
	for( i=0; i<=10; i++ ){
		sprintf(buf, "%3d %3d\r", i, i*10);
		write( fd, buf, strlen(buf) );
	}
	
	close(fd);
}
