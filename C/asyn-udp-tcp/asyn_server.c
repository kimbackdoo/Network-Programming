 /*----------------------------------------------------------------
파일명 : asyn_server.c
기  능 : udp와 tcp 서버가 동시에 존재
리눅스 : gcc -o asyn_server asyn_server.c
유닉스 : gcc -o asyn_server asyn_server.c -lsocket -lnsl
사용법 : asyn_server 3000 3001
----------------------------------------------------------------*/
// 첫 번째 port:tcp, 두 번째 port:udp인데 이걸 상관없이 만들필요 있음
// 현재 tcp는 tcp 소켓이라고 바로 출력되지만 udp는 udp 소켓이라고 나중에 출력됨. 수정 필요. 

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
#define MAXCHAR 1024

int main(int argc, char *argv[]){
	int  add_len, maxfdp1, msg_size, udp_socket, tcp_socket, client_fd;
	fd_set read_fds;        /* 읽기를 감지할 소켓번호 구조체 */
	char buf[100]; 
    struct sockaddr_in client_addr, server_addr;
	struct timeval tval; 
	
	if(argc != 3){
		printf("사용법 : 실행파일명, tcp-ser port, udp-ser-port\n");
		exit(0);
	}

	/* 초기소켓 생성 */
	if((tcp_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Server : Can't open stream socket.");
		exit(0);
	}
	if((udp_socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
		printf("Server : Can't open diagram socket.");
		exit(0);
	}
	
	/* server_addr 구조체의 내용 세팅 */
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	
	if(bind(tcp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("TCP Server : Can't bind local address.\n");
		exit(0);
	}

	server_addr.sin_port = htons(atoi(argv[2]));
	if(bind(udp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("UDP Server : Can't bind local address.\n");
		exit(0);
	}
	
    /* 클라이언트로부터 연결요청을 기다림 총 5개 대기 가능*/
	listen(tcp_socket, 5);
	// maxfdp1 = tcp_socket;    /* 최대 소켓번호 + 1 */
	add_len = sizeof(server_addr); 
	
	while(1){
		FD_ZERO(&read_fds);
		FD_SET(tcp_socket, &read_fds);
		FD_SET(udp_socket, &read_fds);
				
		if(udp_socket > tcp_socket) maxfdp1 = udp_socket+1; 
		else maxfdp1 = tcp_socket+1; 

		if(select(maxfdp1, &read_fds, (fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0) {
			printf("select error <= 0 \n");
			exit(0);
		}
		if(FD_ISSET(udp_socket, &read_fds)) {
			printf("udp-socket \n");
			msg_size = recvfrom(udp_socket,buf, MAXCHAR, 0, (struct sockaddr *)&client_addr, &add_len);
   			buf[msg_size-1] = '\0';
     		printf("msg : %s \n", buf);
     		sendto(udp_socket, buf, msg_size, 0, (struct sockaddr *)&client_addr,add_len);
        }
		if(FD_ISSET(tcp_socket, &read_fds)) {
			printf("tcp-socket \n"); 
			client_fd = accept(tcp_socket, (struct sockaddr *)&client_addr, &add_len);
			if(client_fd == -1){
				printf("accept error\n");
				exit(0);
			}
			if((msg_size=recv(client_fd, buf, MAXCHAR, 0)) <= 0) continue;
			buf[msg_size-1] = '\0';
     		printf("msg : %s \n", buf);
     		send(client_fd, buf, msg_size, 0);
		}
	}
	return 0;
}
// ----------------- end of file --------------------------
