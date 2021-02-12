#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUF_LEN 1024

int main(int argc, char *argv[]) {
	int s, n, addrsize;
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

	/* echo 서버의 소켓주소 구조체 작성*/
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	/*구조체의 크기를 구한다 */
	addrsize = sizeof(struct sockaddr);

	/*키보드 입력을 받음 */
	printf("Input any string : ");
	fgets(buf, BUF_LEN, stdin);
	
	/*echo 서버로 메시지 송신 */
	if(sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))<0) {
		printf("sendto error\n");
		exit(0);
	}

	/*수신된 echo 메시지 화면 출력 */
	printf("Echoed string : ");
	if(recvfrom(s, buf, BUF_LEN, 0, (struct sockaddr *)&server_addr, &addrsize) < 0) {
		printf("recevfrom error\n");
		exit(0);
	}
	printf("%s \n", buf);
	close(s);
}
