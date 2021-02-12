/*----------------------------------------------------------------
���ϸ� : nonblocking_client.c
��  �� : echo ���񽺸� �䱸�ϴ� ������ Ŭ���̾�Ʈ
������ : gcc -o nonblocking_client nonblocking_client.c
���� : nonblocking_client ip port
----------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
		printf("usage: %s ip_address port_number\n", argv[0]);
		exit(0);
	}
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Cant't create socket\n");
		exit(0);
	}
	/* server_addr�� '\0'���� �ʱ�ȭ */
	bzero((char *)&server_addr, sizeof(server_addr));
	/* echo ������ �����ּ� ����ü �ۼ� */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));
	/* �����û */
	if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("Can't connect.\n");
		exit(0);
	}
	/* Ű���� �Է��� ���� */
	while(1){
		printf("Input any string(bye to quit) : ");
		fgets(buf, BUF_LEN, stdin);
		/* �迭 buf�� bye\n�� ���ؼ� ���� ���ڿ��̸� break */
		if(!strcmp(buf, "bye\n")) break;
		/* echo ������ �޽��� �۽� */
		if((len_out=send(s, buf, strlen(buf), 0)) < 0) {
			printf("write error\n");
			exit(0);
		}
		/* ���ŵ� echo �޽��� ȭ�� ��� */
		printf("Echoed string : ");
		/* ���� �������� ũ�⸸ŭ �ޱ� ���� �ݺ��� ����*/
		for(len_in=0, n=0; len_in<len_out; len_in+=n) {
			if((n = recv(s, &buf[len_in], len_out - len_in, 0)) < 0) {
				printf("read error\n");
				exit(0);
			}
		}
		/* Server���� ���� ������ �������� NULL ���ڸ� �־� ���ڿ��� ���� ǥ��*/
		buf[len_in]='\0';
		/* Server���� ���� ���ڿ� ���*/
		printf("%s \n", buf);
	}
	close(s);
	return 0;
}
