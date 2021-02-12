 /*----------------------------------------------------------------
���ϸ� : info_server.c
��  �� : ���� �м� ���� ���
������ : gcc -o info_server info_server.c
���н� : gcc -o info_server info_server.c -lsocket -lnsl
���� : ./info_server 3000
----------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 512
#define MAX_SOCK 64
// char *escapechar = "exit";
int maxfdp1;                 /* �ִ� ���Ϲ�ȣ +1 */
int num_client = 0;          /* Ŭ���̾�Ʈ ���� �� */
int client_s[MAX_SOCK];      /* ����� Ŭ���̾�Ʈ ���Ϲ�ȣ ��� */
struct sockaddr_in client_addr, server_addr;

int getmax(int);
void removeClient(int);      /* Ŭ���̾�Ʈ Ż�� ó�� �Լ� */

int main(int argc, char *argv[]){
	char rline[MAXLINE], my_msg[MAXLINE], *respond = "ACK";
	int i, j, n;
	int s, client_fd, clilen;
	fd_set read_fds;        /* �б⸦ ������ ���Ϲ�ȣ ����ü */
	
	if(argc != 2){
		printf("���� : %s port\n", argv[0]);
		exit(0);
	}
	
	/* �ʱ���� ���� */
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Server : Can't open stream socket.");
		exit(0);
	}
	
	/* server_addr ����ü�� ���� ���� */
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	
	if(bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("Server : Can't bind local address.\n");
		exit(0);
	}
	/* Ŭ���̾�Ʈ�κ��� �����û�� ��ٸ�, �� 5�� ��� ����*/
	listen(s, 5);
	maxfdp1 = s + 1; /* �ִ� ���Ϲ�ȣ + 1 */
	
	while(1){
		FD_ZERO(&read_fds);
		FD_SET(s, &read_fds);
		for(i=0; i<num_client; i++)
			FD_SET(client_s[i], &read_fds);
		maxfdp1 = getmax(s) + 1;     /* maxfdp1 �� ��� */
		if(select(maxfdp1, &read_fds, (fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0){
			printf("select error <= 0 \n");
			exit(0);
		}
		if(FD_ISSET(s, &read_fds)) {
			clilen = sizeof(client_addr);
			client_fd = accept(s, (struct sockaddr *)&client_addr, &clilen);
			if(client_fd == -1) {
				printf("accept error\n");
				exit(0);
			}
			client_s[num_client] = client_fd;
			printf("Connect client(sock_num = %d, ", client_s[num_client]);
			printf("IP address = %s), ", inet_ntoa(client_addr.sin_addr));
			num_client++;
			printf("Total Client number = %d\n", num_client);
		}
		
		/* Ŭ���̾�Ʈ�� ���� �޽����� ó���� Ŭ���̾�Ʈ���� Ȯ�� �޽��� �ݼ� */
		for(i=0; i<num_client; i++){
			if(FD_ISSET(client_s[i], &read_fds)) {
				if((n = recv(client_s[i], rline, MAXLINE, 0)) <= 0){
					removeClient(i);
					continue;
				}
				rline[n-1] = '\0';
				send(client_s[i], respond, strlen(respond), 0);
				printf("Info_analysis : %s from(sock_num = %d, ", rline, client_s[i]);
				printf("IP address = %s)\n", inet_ntoa(client_addr.sin_addr));
			}
		}
	}
}
/* ���� ���� Ŭ���̾�Ʈ Ż�� ó�� */
void removeClient(int i){
	close(client_s[i]);
	printf("Close client(sock_num = %d, ", client_s[i]);
	if(i != num_client-1) client_s[i] = client_s[num_client-1];
	num_client--;
	printf("IP address = %s), ", inet_ntoa(client_addr.sin_addr));
	printf("Total Client number = %d\n", num_client);
}
/* client_s[] ���� �ִ� ���Ϲ�ȣ ���(�ʱ�ġ�� k) */
int getmax(int k){
	int r, max = k;
	for(r=0; r<num_client; r++)
		if(client_s[r] > max)
			max = client_s[r];
	return max;
}
