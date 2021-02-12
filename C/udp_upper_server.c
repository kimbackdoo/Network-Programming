#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define BUF_LEN 128

int main(int argc, char *argv[]){
	struct sockaddr_in server_addr, client_addr;
	int s, i;
	int len, msg_size;
	char buf[BUF_LEN+1];

	if(argc != 2) {
		printf("usage: %s port\n", argv[0]);
		exit(0);
	}

	if((s=socket(PF_INET, SOCK_DGRAM, 0))<0) {
		printf("Server: Can't open stream socket.");
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

	while(1) {
		printf("Server : waiting request.\n");

		if(recvfrom(s, buf, BUF_LEN, 0,(struct sockaddr *)&client_addr, &len)<0){
			printf("recvfrom error\n");
			exit(1);
		}
		printf("Server : A client data received.\n");

		msg_size = strlen(buf);
		for(i=0; i<msg_size; i++)
			if('a'<=buf[i] || buf[i]<='z')
				buf[i] -= 32;
		if(sendto(s, buf, msg_size, 0, (struct sockaddr *)&client_addr, len)<0) {
			printf("sendto error\n");
			exit(1);
		}
		printf("sendto complete\n");
	}
	return 0;
}


