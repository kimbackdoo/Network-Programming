#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
// 멀티스레드를 위한 헤더파일 
#include <pthread.h>

// udp에서는 tcp와는 다르게 배열 크기를 크게 잡으면 안됨. 
#define BUF_LEN 128
// udp는 tcp와 다르게 서버와 클라이언트가 연결되는 것이 아닌 단지 정보를 주고 받기 때문에
// 스레드를 만들기 위해 필요한 변수들을 모두 전역변수로 선언. 
int s, len, msg_size;
char buf[BUF_LEN];
struct sockaddr_in server_addr, client_addr;
// udp 관련 스레드를 만들기 위함 함수 원형 선언. 
void *udpRxThread(void *);

int main(int argc, char **argv) {
	pthread_t udp_s_thread;

	if(argc != 2) {
		printf("usage: %s port thread_num\n", argv[0]);
		exit(0);
	}

	if((s=socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Server: Can't open udp socket.");
		exit(0);
	}

	bzero((char *)&server_addr, sizeof(server_addr));
	bzero((char *)&client_addr, sizeof(client_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if(bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) <0) {
		printf("Server: Can't bind local address.\n");
		exit(0);
	}

	len = sizeof(struct sockaddr);
	/* Iterative echo */
	while(1) {
		printf("Server : waiting request.\n");
		// recvfrom 함수는 데이터가 오면 대기 시킴. 
		if((msg_size=recvfrom(s, buf, BUF_LEN, 0, (struct sockaddr*)&client_addr, &len)) < 0) {
			printf("recvfrom error\n");
			exit(0);
		}
		// client_addr을 인자로 넘겨서 스레드를 생성.
		if(pthread_create(&udp_s_thread, NULL, udpRxThread, (void *)&client_addr) != 0) {
			printf("pthread_create() error\n");
			continue;
		}
		printf("Sendto complete\n\n");
	}
	close(s);
	return 0;
}

void *udpRxThread(void *arg) {
	pthread_t id;
	id = pthread_self();
	
	printf("==================================================\n");
	/* Client ip, port 출력 */
	printf("Server : [ip: %s] [port: %d] client data received.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	/* Server port 출력*/
	printf("Server [port: %d]\n", ntohs(server_addr.sin_port));
	/* recvfrom 함수를 통해 개행 문자까지 데이터를 받기 때문에 개행 문자 자리에 NULL을 저장하여 문자열의 끝을 표시 */
	buf[msg_size-1] = '\0';
	// 스레드의 식별번호와 받은 문자열 출력. 
	printf("Thread [%ud]-Rx Msg : %s\n", id, buf);
	printf("==================================================\n");
	// sendto 함수를 통해 받은 데이터를 다시 echo. 
	if(sendto(s, buf, msg_size, 0, (struct sockaddr*)&client_addr, len)<0) {
		printf("sendto error\n");
		exit(0);
	}
}
