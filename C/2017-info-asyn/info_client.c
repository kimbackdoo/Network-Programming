/*----------------------------------------------------------------
���ϸ� : info_client.c, 2014-04-07
��  �� : ���� ���� Ŭ���̾�Ʈ ���
������ : gcc -o info_client info_client.c
���н� : gcc -o info_client info_client.c -lsocket -lnsl
���� : info_client 220.69.240.149 3000
----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>	//for exit()
#include <string.h>	//for bzero(), ...
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUF_LEN 120

int main(int argc, char *argv[]){
	int s, n, len_in, len_out;
	struct sockaddr_in server_addr;
	char *haddr, *escapechar = "exit";
	char buf[BUF_LEN+1], recv_buf[BUF_LEN];
	
	if(argc != 3){
		printf("usage : %s ip_address port_number\n", argv[0]);
		exit(0);
	}
	haddr = argv[1];
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Can't create socket\n");
		exit(0);
	}
	
	/* ���� �м� ������ �����ּ� ����ü �ۼ� */	
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(haddr);
	server_addr.sin_port = htons(atoi(argv[2]));
	
	/* ���� ��û */
	if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("Can't connect.\n");
		exit(0);
	}
	
	/* Ű���� �Է��� ���� */
	while(1){
		printf("Info Collection : ");
 		//bzero(buf, BUF_LEN); 
		fgets(buf, BUF_LEN, stdin);
		if(strstr(buf, escapechar) != 0){
			printf("Good bye.\n");
			close(s);
			exit(0);
		}
		buf[BUF_LEN] = '\0';
		len_out = strlen(buf);
		/* ���� �м� ������ �޽��� �۽� */
        //printf("len_ou = %d \n", len_out);
		if(send(s, buf, len_out, 0) < 0){
			printf("send error\n");
			exit(0);
		}
		/* ���ŵ� Ȯ�� �޽��� ȭ����� */
		printf("ACK response : ");
		/******************
		for(len_in=0, n=0; len_in < len_out; len_in += n){ 
                   *******************/
			if((n = recv(s, recv_buf, BUF_LEN, 0)) < 0){
				printf("recv error\n");
				exit(0);
			}
                   /******************
		}
		******************/
		recv_buf[n] = '\0';
		printf("%s\n", recv_buf);
	}
}
