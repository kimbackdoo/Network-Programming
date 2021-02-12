/*----------------------------------------------------------------
파일명 : nonblocking_server.c
기  능 : echo 서비스를 요구하는 연결형 nonblocing server
리눅스 : gcc -o nonblocking_server nonblocking_server.c
사용법 : nonblocking_server port
----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
// /usr/include/errno.h에 오류 발생시 오류 확인을 위해 전역으로 int형 변수 errno가 선언되어 있음
// 따라서 변수 errno에 접근하기 위해 errno.h 사용 
#include <errno.h> 
#include <fcntl.h>
// buf 사이즈를 1024인 매크로 정의 
#define BUF_SIZE 1024

int main(int argc, char **argv) {
	int i, flag, read_rtn, on = 1;
	int server_sock, client_sock;
	char buf[BUF_SIZE];
	struct sockaddr_in server_addr, client_addr;
	int client_addr_size = sizeof(client_addr);
	// 사용법 출력 
	if(argc != 2) {
		printf("Usage: %s [port]\n", argv[0]);
		exit(1);
	}
	// 연결형 TCP 소켓 개설
	// IPv4 인터넷 프로토콜 체계(PF_INET)에서 연결형(SOCK_STREAM)을 의미하는 것은 TCP 밖에 없으므로 protocol에 0으로 설정
	// PF_INET, AF_INET은 같지만 형식적으로 프로토콜 패밀리에는 PF_INET, 주소 패밀리에느 AF_INET으로 명시되어 있으므로
	// PF_INET 사용 
	server_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(server_sock < 0) {
		printf("socket create error\n");
		exit(1);
	}
	// server와 client가 연결된 상태에서 server 프로그램을 강제로 종료하게 되면 Time-wait 상태가 발생하는데
	// 이러한 문제를 해결하기 위해 그리고 동일한 포트번호로 여러 사용자가 사용하게 하기 위해 SO_REUSEADDR 사용 
	if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		printf("socket option set error\n");
		exit(1);
	}
	// 형식적으로 주소 패밀리는 AF_INET라고 명시되어 있으므로 AF_INET 사용 
	server_addr.sin_family = AF_INET;
	// INADDR_ANY를 사용하여 IP 주소 자동으로 할당
	// 리틀엔디안, 빅엔디안 시스템에서 모두 사용하기 위해 호스트 바이트 순서를 네트워크 바이트 순서로 변환 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	/* bind() 호출 */
	if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		printf("bind error\n");
		exit(1);
	}
	/* 소켓을 수동 대기모드로 세팅 */
	if(listen(server_sock, 5) < 0) {
		printf("listen error\n");
		exit(1);
	}
	// 배열 buf를 memset 함수를 이용해 초기화 
	memset(buf, 0x00, sizeof(buf));
	/* iterative echo 서비스 수행 */
	// 연결요청 개수만큼만 for문 실행
	for(i=0; i<5; i++) {
		printf("====================================\n");
		printf("accept wait ...\n");
		client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
		if(client_sock < 0) {
			printf("accept error\n");
			break;
		}
		else {
			printf("=========== accept client ==========\n");
			// 접속한 client의 ip와 port 출력 
			printf("Connected Client [ip: %s] [port: %d]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			/* Server port 출력*/
			printf("Server [port: %d]\n", ntohs(server_addr.sin_port));
			printf("====================================\n");
		}
		// client 소켓을 Non-Blocking 모드로 설정
		// 따라서 accept은 blocking 모드이고 read 함수만 nonbloking 모드
		flag = fcntl(client_sock, F_GETFL, 0);
		fcntl(client_sock, F_SETFL, flag|O_NONBLOCK);
		// nonblocking 모드이기 때문에
		// 무한루프를 돌면서 데이터의 수신에 관계없이 읽은 데이터가 있으면 읽고 없으면 넘어감 
		while(1) {
			printf("Read wait ...\n");
			// sleep 함수를 통해 CPU 점유율을 낮춤
			// client에서 문자 입력을 하는 시간을 기다리기 위해 3초로 설정
			// 즉, client에서 3초 안에 문자 입력을 안하면 read_rtn은 -1이 됨 
			sleep(3);
			read_rtn = read(client_sock, buf, BUF_SIZE);
			if(read_rtn == 0) {
				printf("====================================\n");
				printf("Closed Client [ip: %s] [port: %d]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
				printf("====================================\n\n");
				close(client_sock);
				break;
			}
			else if(read_rtn < 0) {
				// read 함수는 더 이상 읽어 들일 데이터가 없을 때 -1을 반환하고 이 때 errno에 상수 EAGAIN이 저장됨 
				if(errno == EAGAIN) printf("Read no data\n\n");
				// 읽어 들일 데이터가 없는 것은 사용자가 아직 입력을 안한 상태이기 때문에
				// break로 무한루프를 끝내는 것이 아니라 continue로 다시 read 함수를 호출 
				continue;
			}
			else {
				// 받은 데이터의 크기 출력 
				printf("- [Read Data_len : %d] -\n", read_rtn);
				// client에서 받은 데이터의 크기가 read_rtn 이므로 read_rtn-1에 NULL 문자를 넣어 문자열의 끝을 표시 
				buf[read_rtn-1]='\0';
				// read 함수에 성공하면 읽어 들인 데이터를 출력 
				printf("---- [Read Data : %s] ---\n", buf); //---
				// write 함수를 통해 읽어 들인 데이터 만큼 client로 echo 
				write(client_sock, buf, read_rtn);
				printf("Other routine processing\n\n");
				// read 함수에 성공할 경우 배열 buf를 다시 초기화하여 배열에 데이터가 쌓이지 않음 
				memset(buf, 0x00, sizeof(buf));
			}
		}
	}
	return 0;
}
