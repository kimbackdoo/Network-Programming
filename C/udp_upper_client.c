#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUF_LEN 128

int main(int argc, char *argv[]) {
	int s, n, len_out, addrsize;
	struct sockaddr_in server_addr;
	char *haddr;
	char buf[BUF_LEN+1];

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

	printf("Input any string : ");
	if(fgets(buf, BUF_LEN, stdin)!=NULL) {
		len_out = strlen(buf);
		buf[len_out-1] = '\0';
	} else {
		printf("fgets error\n");
		exit(0);
	}

	if(sendto(s, buf, len_out, 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))<0) {
		printf("sendto error\n");
		exit(0);
	}

	printf("Upper string : ");
	if(recvfrom(s, buf, BUF_LEN, 0, (struct sockaddr *)&server_addr, &addrsize) < 0) {
		printf("recevfrom error\n");
		exit(0);
	}
	printf("%s\n", buf);
	close(s);
	return 0;
}

