/*----------------------------------------------------------------
파일명 : nonblocking_client.c
기  능 : echo 서비스를 요구하는 연결형 클라이언트
리눅스 : gcc -o nonblocking_client nonblocking_client.c
사용법 : nonblocking_client ip port
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
	/* server_addr을 '\0'으로 초기화 */
	bzero((char *)&server_addr, sizeof(server_addr));
	/* echo 서버의 소켓주소 구조체 작성 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));
	/* 연결요청 */
	if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("Can't connect.\n");
		exit(0);
	}
	/* 키보드 입력을 받음 */
	while(1){
		printf("Input any string(bye to quit) : ");
		fgets(buf, BUF_LEN, stdin);
		/* 배열 buf와 bye\n을 비교해서 같은 문자열이면 break */
		if(!strcmp(buf, "bye\n")) break;
		/* echo 서버로 메시지 송신 */
		if((len_out=send(s, buf, strlen(buf), 0)) < 0) {
			printf("write error\n");
			exit(0);
		}
		/* 수신된 echo 메시지 화면 출력 */
		printf("Echoed string : ");
		/* 보낸 데이터의 크기만큼 받기 위해 반복문 실행*/
		for(len_in=0, n=0; len_in<len_out; len_in+=n) {
			if((n = recv(s, &buf[len_in], len_out - len_in, 0)) < 0) {
				printf("read error\n");
				exit(0);
			}
		}
		/* Server에서 받은 데이터 마지막에 NULL 문자를 넣어 문자열의 끝을 표시*/
		buf[len_in]='\0';
		/* Server에서 받은 문자열 출력*/
		printf("%s \n", buf);
	}
	close(s);
	return 0;
}
