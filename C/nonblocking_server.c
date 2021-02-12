/*----------------------------------------------------------------
���ϸ� : nonblocking_server.c
��  �� : echo ���񽺸� �䱸�ϴ� ������ nonblocing server
������ : gcc -o nonblocking_server nonblocking_server.c
���� : nonblocking_server port
----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
// /usr/include/errno.h�� ���� �߻��� ���� Ȯ���� ���� �������� int�� ���� errno�� ����Ǿ� ����
// ���� ���� errno�� �����ϱ� ���� errno.h ��� 
#include <errno.h> 
#include <fcntl.h>
// buf ����� 1024�� ��ũ�� ���� 
#define BUF_SIZE 1024

int main(int argc, char **argv) {
	int i, flag, read_rtn, on = 1;
	int server_sock, client_sock;
	char buf[BUF_SIZE];
	struct sockaddr_in server_addr, client_addr;
	int client_addr_size = sizeof(client_addr);
	// ���� ��� 
	if(argc != 2) {
		printf("Usage: %s [port]\n", argv[0]);
		exit(1);
	}
	// ������ TCP ���� ����
	// IPv4 ���ͳ� �������� ü��(PF_INET)���� ������(SOCK_STREAM)�� �ǹ��ϴ� ���� TCP �ۿ� �����Ƿ� protocol�� 0���� ����
	// PF_INET, AF_INET�� ������ ���������� �������� �йи����� PF_INET, �ּ� �йи����� AF_INET���� ��õǾ� �����Ƿ�
	// PF_INET ��� 
	server_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(server_sock < 0) {
		printf("socket create error\n");
		exit(1);
	}
	// server�� client�� ����� ���¿��� server ���α׷��� ������ �����ϰ� �Ǹ� Time-wait ���°� �߻��ϴµ�
	// �̷��� ������ �ذ��ϱ� ���� �׸��� ������ ��Ʈ��ȣ�� ���� ����ڰ� ����ϰ� �ϱ� ���� SO_REUSEADDR ��� 
	if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		printf("socket option set error\n");
		exit(1);
	}
	// ���������� �ּ� �йи��� AF_INET��� ��õǾ� �����Ƿ� AF_INET ��� 
	server_addr.sin_family = AF_INET;
	// INADDR_ANY�� ����Ͽ� IP �ּ� �ڵ����� �Ҵ�
	// ��Ʋ�����, �򿣵�� �ý��ۿ��� ��� ����ϱ� ���� ȣ��Ʈ ����Ʈ ������ ��Ʈ��ũ ����Ʈ ������ ��ȯ 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	/* bind() ȣ�� */
	if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		printf("bind error\n");
		exit(1);
	}
	/* ������ ���� ������ ���� */
	if(listen(server_sock, 5) < 0) {
		printf("listen error\n");
		exit(1);
	}
	// �迭 buf�� memset �Լ��� �̿��� �ʱ�ȭ 
	memset(buf, 0x00, sizeof(buf));
	/* iterative echo ���� ���� */
	// �����û ������ŭ�� for�� ����
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
			// ������ client�� ip�� port ��� 
			printf("Connected Client [ip: %s] [port: %d]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			/* Server port ���*/
			printf("Server [port: %d]\n", ntohs(server_addr.sin_port));
			printf("====================================\n");
		}
		// client ������ Non-Blocking ���� ����
		// ���� accept�� blocking ����̰� read �Լ��� nonbloking ���
		flag = fcntl(client_sock, F_GETFL, 0);
		fcntl(client_sock, F_SETFL, flag|O_NONBLOCK);
		// nonblocking ����̱� ������
		// ���ѷ����� ���鼭 �������� ���ſ� ������� ���� �����Ͱ� ������ �а� ������ �Ѿ 
		while(1) {
			printf("Read wait ...\n");
			// sleep �Լ��� ���� CPU �������� ����
			// client���� ���� �Է��� �ϴ� �ð��� ��ٸ��� ���� 3�ʷ� ����
			// ��, client���� 3�� �ȿ� ���� �Է��� ���ϸ� read_rtn�� -1�� �� 
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
				// read �Լ��� �� �̻� �о� ���� �����Ͱ� ���� �� -1�� ��ȯ�ϰ� �� �� errno�� ��� EAGAIN�� ����� 
				if(errno == EAGAIN) printf("Read no data\n\n");
				// �о� ���� �����Ͱ� ���� ���� ����ڰ� ���� �Է��� ���� �����̱� ������
				// break�� ���ѷ����� ������ ���� �ƴ϶� continue�� �ٽ� read �Լ��� ȣ�� 
				continue;
			}
			else {
				// ���� �������� ũ�� ��� 
				printf("- [Read Data_len : %d] -\n", read_rtn);
				// client���� ���� �������� ũ�Ⱑ read_rtn �̹Ƿ� read_rtn-1�� NULL ���ڸ� �־� ���ڿ��� ���� ǥ�� 
				buf[read_rtn-1]='\0';
				// read �Լ��� �����ϸ� �о� ���� �����͸� ��� 
				printf("---- [Read Data : %s] ---\n", buf); //---
				// write �Լ��� ���� �о� ���� ������ ��ŭ client�� echo 
				write(client_sock, buf, read_rtn);
				printf("Other routine processing\n\n");
				// read �Լ��� ������ ��� �迭 buf�� �ٽ� �ʱ�ȭ�Ͽ� �迭�� �����Ͱ� ������ ���� 
				memset(buf, 0x00, sizeof(buf));
			}
		}
	}
	return 0;
}
