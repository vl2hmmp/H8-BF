// ****************************************
// *** ファイル入出力の動作確認         ***
// ***    produced by Y. Mori           ***
// ***       special thanks to A. Ruike ***
// ****************************************
// type data.txt
// で，H8のファイルを中を見れる．
//
// tftp - 192.168.1.1 data.txt 
// で，PCのプログラムのあるフォルダにファイルが転送される．

#include <mes2.h>

/* ************ */
/* *** main *** */
/* ************ */
int
main(void)
{
	static int   fd;
	static char  buf[256];  // static をつけないと，サイズが大きいときにコンパイルエラーが起きる
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
