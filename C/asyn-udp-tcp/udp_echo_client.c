#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUF_LEN 1024

int main(int argc, char *argv[]) {
	int s, n, addrsize;
	struct sockaddr_in server_addr;
	char buf[BUF_LEN];

	if(argc != 3) {
		printf("usage: %s ip_address port_number\n", argv[0]);
		exit(0);
	}

	if((s = socket(PF_INET, SOCK_DGRAM, 0)) <0) {
		printf("can't create socket\n");
		exit(0);
	}

	/* echo ������ �����ּ� ����ü �ۼ�*/
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	/*����ü�� ũ�⸦ ���Ѵ� */
	addrsize = sizeof(struct sockaddr);

	/*Ű���� �Է��� ���� */
	printf("Input any string : ");
	fgets(buf, BUF_LEN, stdin);
	
	/*echo ������ �޽��� �۽� */
	if(sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))<0) {
		printf("sendto error\n");
		exit(0);
	}

	/*���ŵ� echo �޽��� ȭ�� ��� */
	printf("Echoed string : ");
	if(recvfrom(s, buf, BUF_LEN, 0, (struct sockaddr *)&server_addr, &addrsize) < 0) {
		printf("recevfrom error\n");
		exit(0);
	}
	printf("%s \n", buf);
	close(s);
}
