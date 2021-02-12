/*----------------------------------------------------------------
파일명 : myecho_server.c
기  능 : echo 서비스를 수행하는 서버
리눅스 : gcc -o myecho_server myecho_server.c
유닉스 : gcc -o myecho_server myecho_server.c -lsocket -lnsl
사용법 : myecho_server port
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
	int server_fd, client_fd;   /* 소켓 번호 */
	int i, idx, len, msg_size;
	char buf[BUF_LEN+1];
	
	if(argc != 2){
		printf("usage: %s port\n", argv[0]);
		exit(0);
	}
	/* 소켓 생성 */
	if((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Server: Can't open stream socket.");
		exit(0);
	}
	
	/* server_addr을 '\0'으로 초기화 */
	bzero((char *)&server_addr, sizeof(server_addr));
	/* server_addr 세팅 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	
	/* bind() 호출 */
	if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("Server: Can't bind local address.\n");
		exit(0);
	}
	
	/* 소켓을 수동 대기모드로 세팅 */
	if(listen(server_fd, 5) < 0){
		printf("Server: Can't listen().\n");
		exit(0);
	}
	
	len = sizeof(client_addr); 
	/* iterative echo 서비스 수행 */
	for(i=0; i<5; i++) {
		printf("Server : waiting connection request.\n");
		/* 연결요청을 기다림 */
		client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
		if(client_fd < 0) {
			printf("Server: accept failed.\n");
			exit(0);
		}
		/* 접속한 클라이언트의 ip주소와 port번호를 출력*/
		/* 접속한 클라이언트의 port주소는 bind()하기 위해 사용한 서버 port 번호와 다름*/
		else {
			printf("Connected Client [ip: %s] [port: %d]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			/* Server port 출력*/
			printf("Server [port: %d]\n", ntohs(server_addr.sin_port));
		}
		/* 클라이언트로부터 받은 데이터 만큼 다시 보내기 위해 반복문 실행*/
		while((msg_size=recv(client_fd, buf, BUF_LEN, 0))!=0){
			/* 클라이언트로부터 받은 문자열을 소문자는 대문자로 대문자는 소문자로 변환*/
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
