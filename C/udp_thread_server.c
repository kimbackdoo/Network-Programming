#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
// ��Ƽ�����带 ���� ������� 
#include <pthread.h>

// udp������ tcp�ʹ� �ٸ��� �迭 ũ�⸦ ũ�� ������ �ȵ�. 
#define BUF_LEN 128
// udp�� tcp�� �ٸ��� ������ Ŭ���̾�Ʈ�� ����Ǵ� ���� �ƴ� ���� ������ �ְ� �ޱ� ������
// �����带 ����� ���� �ʿ��� �������� ��� ���������� ����. 
int s, len, msg_size;
char buf[BUF_LEN];
struct sockaddr_in server_addr, client_addr;
// udp ���� �����带 ����� ���� �Լ� ���� ����. 
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
		// recvfrom �Լ��� �����Ͱ� ���� ��� ��Ŵ. 
		if((msg_size=recvfrom(s, buf, BUF_LEN, 0, (struct sockaddr*)&client_addr, &len)) < 0) {
			printf("recvfrom error\n");
			exit(0);
		}
		// client_addr�� ���ڷ� �Ѱܼ� �����带 ����.
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
	/* Client ip, port ��� */
	printf("Server : [ip: %s] [port: %d] client data received.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	/* Server port ���*/
	printf("Server [port: %d]\n", ntohs(server_addr.sin_port));
	/* recvfrom �Լ��� ���� ���� ���ڱ��� �����͸� �ޱ� ������ ���� ���� �ڸ��� NULL�� �����Ͽ� ���ڿ��� ���� ǥ�� */
	buf[msg_size-1] = '\0';
	// �������� �ĺ���ȣ�� ���� ���ڿ� ���. 
	printf("Thread [%ud]-Rx Msg : %s\n", id, buf);
	printf("==================================================\n");
	// sendto �Լ��� ���� ���� �����͸� �ٽ� echo. 
	if(sendto(s, buf, msg_size, 0, (struct sockaddr*)&client_addr, len)<0) {
		printf("sendto error\n");
		exit(0);
	}
}
