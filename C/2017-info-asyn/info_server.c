 /*----------------------------------------------------------------
파일명 : info_server.c
기  능 : 정보 분석 서버 기능
리눅스 : gcc -o info_server info_server.c
유닉스 : gcc -o info_server info_server.c -lsocket -lnsl
사용법 : ./info_server 3000
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
int maxfdp1;                 /* 최대 소켓번호 +1 */
int num_client = 0;          /* 클라이언트 연결 수 */
int client_s[MAX_SOCK];      /* 연결된 클라이언트 소켓번호 목록 */
struct sockaddr_in client_addr, server_addr;

int getmax(int);
void removeClient(int);      /* 클라이언트 탈퇴 처리 함수 */

int main(int argc, char *argv[]){
	char rline[MAXLINE], my_msg[MAXLINE], *respond = "ACK";
	int i, j, n;
	int s, client_fd, clilen;
	fd_set read_fds;        /* 읽기를 감지할 소켓번호 구조체 */
	
	if(argc != 2){
		printf("사용법 : %s port\n", argv[0]);
		exit(0);
	}
	
	/* 초기소켓 생성 */
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Server : Can't open stream socket.");
		exit(0);
	}
	
	/* server_addr 구조체의 내용 세팅 */
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	
	if(bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("Server : Can't bind local address.\n");
		exit(0);
	}
	/* 클라이언트로부터 연결요청을 기다림, 총 5번 대기 가능*/
	listen(s, 5);
	maxfdp1 = s + 1; /* 최대 소켓번호 + 1 */
	
	while(1){
		FD_ZERO(&read_fds);
		FD_SET(s, &read_fds);
		for(i=0; i<num_client; i++)
			FD_SET(client_s[i], &read_fds);
		maxfdp1 = getmax(s) + 1;     /* maxfdp1 재 계산 */
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
		
		/* 클라이언트가 보낸 메시지를 처리후 클라이언트에게 확인 메시지 반송 */
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
/* 정보 수집 클라이언트 탈퇴 처리 */
void removeClient(int i){
	close(client_s[i]);
	printf("Close client(sock_num = %d, ", client_s[i]);
	if(i != num_client-1) client_s[i] = client_s[num_client-1];
	num_client--;
	printf("IP address = %s), ", inet_ntoa(client_addr.sin_addr));
	printf("Total Client number = %d\n", num_client);
}
/* client_s[] 내의 최대 소켓번호 얻기(초기치는 k) */
int getmax(int k){
	int r, max = k;
	for(r=0; r<num_client; r++)
		if(client_s[r] > max)
			max = client_s[r];
	return max;
}
