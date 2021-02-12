/*----------------------------------------------------------------
���ϸ� : tcp_echo_client.c
��  �� : echo ���񽺸� �䱸�ϴ� TCP(������) Ŭ���̾�Ʈ
������ : gcc -o tcpc tcp_echo_client.c
���� : ./tcpc 220.69.244.51 port-��ȣ
----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUF_LEN 1024

int main(int argc, char *argv[]){
	int s, n, len_in, len_out;
	struct sockaddr_in server_addr;
	char buf[BUF_LEN];
	
	if(argc != 3){
		printf("���� : �������ϸ�, tcp-ser ddr, ser-port\n");
		exit(0);
	}
	
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Cant't create socket\n");
		exit(0);
	}
	/* echo ������ �����ּ� ����ü �ۼ� */
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));
	/* �����û */
	if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("Can't connect.\n");
		exit(0);
	}
	
	/* Ű���� �Է��� ���� */
	printf("Input any string : ");
	fgets(buf, BUF_LEN, stdin);
	
	/* echo ������ �޽��� �۽� */
	if((len_out=send(s, buf, strlen(buf), 0)) < 0) {
		printf("write error\n");
		exit(0);
	}
	
	/* ���ŵ� echo �޽��� ȭ�� ��� */
	printf("Echoed string : ");
	for(len_in=0, n=0; len_in < len_out; len_in += n) {
		if((n = recv(s, &buf[len_in], len_out - len_in, 0)) < 0) {
			printf("read error\n");
			exit(0);
		}
	}
	buf[len_in] = '\0';
	printf("%s \n", buf);
	close(s);
	return 0;
}
