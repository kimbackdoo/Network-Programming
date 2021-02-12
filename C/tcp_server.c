/*----------------------------------------------------------------
���ϸ� : myecho_server.c
��  �� : echo ���񽺸� �����ϴ� ����
������ : gcc -o myecho_server myecho_server.c
���н� : gcc -o myecho_server myecho_server.c -lsocket -lnsl
���� : myecho_server port
----------------------------------------------------------------*/
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#define BUF_LEN 128

int main(int argc, char *argv[]) {
	struct sockaddr_in server_addr, client_addr;
	int server_fd, client_fd;   /* ���� ��ȣ */
	int i, idx, len, msg_size;
	char buf[BUF_LEN+1];
	
	if(argc != 2){
		printf("usage: %s port\n", argv[0]);
		exit(0);
	}
	/* ���� ���� */
	if((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Server: Can't open stream socket.");
		exit(0);
	}
	
	/* server_addr�� '\0'���� �ʱ�ȭ */
	bzero((char *)&server_addr, sizeof(server_addr));
	/* server_addr ���� */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	
	/* bind() ȣ�� */
	if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("Server: Can't bind local address.\n");
		exit(0);
	}
	
	/* ������ ���� ������ ���� */
	if(listen(server_fd, 5) < 0){
		printf("Server: Can't listen().\n");
		exit(0);
	}
	
	len = sizeof(client_addr); 
	/* iterative echo ���� ���� */
	for(i=0; i<5; i++) {
		printf("Server : waiting connection request.\n");
		/* �����û�� ��ٸ� */
		client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
		if(client_fd < 0) {
			printf("Server: accept failed.\n");
			exit(0);
		}
		/* ������ Ŭ���̾�Ʈ�� ip�ּҿ� port��ȣ�� ���*/
		/* ������ Ŭ���̾�Ʈ�� port�ּҴ� bind()�ϱ� ���� ����� ���� port ��ȣ�� �ٸ�*/
		else {
			printf("Connected Client [ip: %s] [port: %d]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			/* Server port ���*/
			printf("Server [port: %d]\n", ntohs(server_addr.sin_port));
		}
		/* Ŭ���̾�Ʈ�κ��� ���� ������ ��ŭ �ٽ� ������ ���� �ݺ��� ����*/
		while((msg_size=recv(client_fd, buf, BUF_LEN, 0))!=0){
			/* Ŭ���̾�Ʈ�κ��� ���� ���ڿ��� �ҹ��ڴ� �빮�ڷ� �빮�ڴ� �ҹ��ڷ� ��ȯ*/
			for(idx=0; idx<strlen(buf); idx++){
				if('a'<=buf[idx] && buf[idx]<='z')
					buf[idx] -= 32;
				else if('A'<=buf[idx] && buf[idx]<='Z')
					buf[idx] += 32;
			}
			send(client_fd, buf, msg_size, 0);
		}
		printf("bye\n\n");
		close(client_fd);
	}
	close(server_fd);
	return 0;
}
