/* -----------------------------------------------------
파일명 : byte_order.c
기  능 : 호스트 바이트 순서 테스트 프로그램
리눅스 : gcc -o byte_order byte_order.c
유닉스 : gcc -o byte_order byte_order.c -lsocket
사용법 : ./byte_order
----------------------------------------------------- */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	struct servent *pmyservent;
	
	pmyservent = getservbyname("echo", "udp");
	
	if( pmyservent == NULL ) {
		printf( "서비스 정보를 얻을 수 없을. \n\n");
		exit(0);
	}
	
	printf("Port number of 'echo', 'udp' without ntohs() : %d \n", pmyservent->s_port);
	printf("Port number of 'ehco', 'udp' with ntohs() : %d \n", ntohs(pmyservent->s_port));
	
	return 0;
}
