/*----------------------------------------------------------------
파일명 : tcp_echo_client.c
기  능 : echo 서비스를 요구하는 TCP(연결형) 클라이언트
리눅스 : gcc -o tcpc tcp_echo_client.c
사용법 : ./tcpc 220.69.244.51 port-번호
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
		printf("사용법 : 실행파일명, tcp-ser ddr, ser-port\n");
		exit(0);
	}
	
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Cant't create socket\n");
		exit(0);
	}
	/* echo 서버의 소켓주소 구조체 작성 */
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));
	/* 연결요청 */
	if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("Can't connect.\n");
		exit(0);
	}
	
	/* 키보드 입력을 받음 */
	printf("Input any string : ");
	fgets(buf, BUF_LEN, stdin);
	
	/* echo 서버로 메시지 송신 */
	if((len_out=send(s, buf, strlen(buf), 0)) < 0) {
		printf("write error\n");
		exit(0);
	}
	
	/* 수신된 echo 메시지 화면 출력 */
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
