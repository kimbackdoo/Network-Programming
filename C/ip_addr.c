/* -------------------------------------------------------------------
���ϸ� : ip_addr.c
��  �� : IP �ּҺ�ȯ ���� ���α׷�
������ : gcc -o ip_addr ip_addr.c
���н� : gcc -o ip_addr ip_addr.c -lsocket -lnsl
���� : ./ip_addr dcu.ac.kr
------------------------------------------------------------------- */

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char * argv[]){
	
	struct hostent *myhost;		/* ȣ��Ʈ�� ���� ������ ������ ����ü */
	struct in_addr myinaddr;	/* IP �ּҸ� ������ ����ü */
	int i;
	
	if(argc < 2){
		printf("���� : %s host_name(������ ����) \n", argv[0]);
		exit(0);
	}
	
	/* �Է��� �������� ��� */
	printf("�Է��� ������: %s\n\n", argv[1]);
	
	/* hostent ����ü ���ϱ� */
	myhost = gethostbyname(argv[1]);
	
	if(myhost == 0){
		printf("error at gethostbyname \n");
		exit(0);
	}
	
	/* ȣ��Ʈ �ּ� ��ȯ �κ� */
	i = 0;
	while(myhost->h_addr_list[i] != NULL){
		myinaddr.s_addr = *( (u_long *)(myhost->h_addr_list[i]) );
		
		/* ȣ��Ʈ �ּҸ� ������ IP (����� 16����) ���·� ��� */
		printf("IP �ּ�(2����, ntohl X): 0x%0x(16 ����)\n", myinaddr.s_addr);
		printf("IP �ּ�(2����, ntohl O): 0x%0x(16 ����)\n\n", ntohl(myinaddr.s_addr));
		
		/* ȣ��Ʈ �ּҸ� dotted decimal ���·� ��� */
		printf("IP address : %s\n", inet_ntoa(myinaddr));
		i++;
	}
	
	return 0;
}
