#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int s, n, addrsize;
	struct sockaddr_in server_addr;
	char *haddr;
	char buf[]="test";

	if(argc != 3) {
		printf("usage: %s ip_address port_number\n", argv[0]);
		exit(0);
	}
	haddr = argv[1];

	if((s = socket(PF_INET, SOCK_DGRAM, 0)) <0) {
		printf("can't create socket\n");
		exit(0);
	}

	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(haddr);
	server_addr.sin_port = htons(atoi(argv[2]));

	addrsize = sizeof(struct sockaddr);

	if(sendto(s, buf, 5, 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))<0) {
		printf("sendto error\n");
		exit(0);
	}
	printf("Client : sendto complete\n");
	
	if(recvfrom(s, buf, 5, 0, (struct sockaddr *)&server_addr, &addrsize) < 0) {
		printf("recevfrom error\n");
		exit(0);
	}
	printf("Client : A server data received\n");
	
	close(s);
	return 0;
}

