/* -----------------------------------------------------
���ϸ� : byte_order.c
��  �� : ȣ��Ʈ ����Ʈ ���� �׽�Ʈ ���α׷�
������ : gcc -o byte_order byte_order.c
���н� : gcc -o byte_order byte_order.c -lsocket
���� : ./byte_order
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
		printf( "���� ������ ���� �� ����. \n\n");
		exit(0);
	}
	
	printf("Port number of 'echo', 'udp' without ntohs() : %d \n", pmyservent->s_port);
	printf("Port number of 'ehco', 'udp' with ntohs() : %d \n", ntohs(pmyservent->s_port));
	
	return 0;
}
