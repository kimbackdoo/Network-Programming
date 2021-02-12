#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
// udp에서는 tcp와는 다르게 배열 크기를 크게 잡으면 안됨.
#define BUF_LEN 128

int main(int argc, char *argv[]) {
	int s, n, str_len, addrsize;
	struct sockaddr_in server_addr;
	char buf[BUF_LEN];

	if(argc != 3) {
		printf("usage: %s ip_address port_number\n", argv[0]);
		exit(0);
	}

	if((s = socket(PF_INET, SOCK_DGRAM, 0)) <0) {
		printf("can't create socket\n");
		exit(0);
	}

	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));
	
	addrsize = sizeof(struct sockaddr);
	
	while(1) {
		printf("Input any string(bye to quit) : ");
		fgets(buf, BUF_LEN, stdin);
		/* 배열 buf와 bye\n을 비교해서 같은 문자열이면 break */
		if(!strcmp(buf, "bye\n")) break;
		if(sendto(s, buf, strlen(buf), 0, (struct sockaddr*)&server_addr, addrsize)<0) {
			printf("sendto error\n");
			exit(0);
		}
		printf("Echoed string : ");
		if((str_len=recvfrom(s, buf, BUF_LEN, 0, (struct sockaddr *)&server_addr, &addrsize)) < 0) {
			printf("recevfrom error\n");
			exit(0);
		}
		/* 받은 문자열의 끝에 NULL을 넣어 문자열의 끝을 표시 */
		buf[str_len] = '\0';
		printf("%s\n", buf);
	}
	close(s);
	return 0;
}
