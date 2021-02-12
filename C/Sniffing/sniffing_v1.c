/*******************************************
  - Non Blocking Sniffing recvfrom with DONWAITFLAG
  - ./sniffing protocol_name
  - #script 명령어를 이용하여 파일에 출력 결과 저장 
********************************************/
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/if_ether.h> // ETH_P_ALL과 ethhdr 구조체를 위한 헤더파일 
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "packet.h"
// buffer 사이즈 정의 
#define BUF_SIZE 65535
// 절차지향인 C언어 특성으로 인한 함수 원형 정의 
void ip_header_print(struct ip_header_t *, int , char *);
void icmp_print(struct ip_header_t *, char *);
void tcp_print(struct ip_header_t *, char *);
void udp_print(struct ip_header_t *, char *);
void data_print(struct ip_header_t *, char *, int );

int main(int argc, char **argv) {
	int sock_eth, ret, len, ptl_num; // ptl_num : protocol number
	char buffer[BUF_SIZE];
	struct sockaddr_in from;
	struct ifreq ifr; 
	// ip를 배열 buffer의 시작주소와 같게 함 
	// 이더넷에서 모든 패킷을 가져오기 때문에 ip헤더 부분을 가리키게 하려면 이더넷의 헤더의 크기만큼 더해줘야함 
	struct ip_header_t *ip = (struct ip_header_t *)(buffer + sizeof(struct ethhdr));
	char *interface = "enp2s0"; // 시스템 마다 변경되어야 할 인터페이스
	 
	if(argc != 2) {
		printf("usage: %s protocol_name\n", argv[0]);
		exit(0);
	} 
	// argv[1]의 문자열과 icmp, tcp, udp 문자열을 비교하여 각 문자열과 같으면
	// 변수 ptl_nul에 icmp=1, tcp=6, udp=17, all=0으로 초기화
	// 만약 icmp, tcp, udp, all 문자열 이외의 문자열이 들어오면 에러메시지 출력하고 프로그램 종료 
	if(strcmp(argv[1], "icmp") == 0) ptl_num = 1;
	else if(strcmp(argv[1], "tcp") == 0) ptl_num = 6;
	else if(strcmp(argv[1], "udp") == 0) ptl_num = 17;
	else if(strcmp(argv[1], "all") == 0) ptl_num = 0;
	else {
		printf("Input only icmp or tcp or udp or all\n");
		exit(0);
	}
	// socket 함수에서 doamin:PF_PACKET, protocol:ETH_P_ALL로 설정하여 이더넷에 모든 패킷들을 가져옴 
	sock_eth = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sock_eth < 0) {
		perror("Socket Error : ");
		exit(1);
	}
	
	strncpy(ifr.ifr_name, interface, strlen(interface));
	
	if(ioctl(sock_eth, SIOCGIFFLAGS, &ifr) < 0) {
		perror("SIOCGIFFLAGS error : ");
		exit(1);
	}

	ifr.ifr_flags |= IFF_PROMISC;

	if(ioctl(sock_eth, SIOCSIFFLAGS, &ifr) < 0) {
		perror("Could not set the PROMISC flag : ");
		exit(1);
	}

	len = sizeof(from);
	// argv[1] 문자열에 따라 protocol number을 저장한 ptl_num을 통해 필터링하여 스니핑함
	while(1) {
		// recvfrom 함수가 호출되기 전에 memset 함수를 통해 buffer의 크기만큼 0x00으로 buffer 초기화
		memset(buffer, 0x00, sizeof(buffer));
		// 배열 buffer의 크기만큼 nonblocking으로 수신함 
		ret = recvfrom(sock_eth, buffer, sizeof(buffer), MSG_DONTWAIT, (struct sockaddr *)&from, &len);
		// recvfrom함수가 실패할 경우 continue 
		if(ret<0) continue;
		/* recvfrom함수가 성공할 경우 icmp, tcp, udp 패킷들에 대하여 출력하고 만약 icmp, tcp, udp 패킷이 아니라면 continue */
		// 사용자가 입력한 icmp, tcp, udp, all에 따라서 사용자가 원하는 패킷 정보들을 출력 
		if((ptl_num==1 || ptl_num==0) && (ip->protocol == PROTO_ICMP)) {
			ip_header_print(ip, ret, argv[1]);
			icmp_print(ip, buffer);
		}
		else if((ptl_num==6 || ptl_num==0) && (ip->protocol == PROTO_TCP)) {
			ip_header_print(ip, ret, argv[1]);
			tcp_print(ip, buffer);
		}
		else if((ptl_num==17 || ptl_num==0) && (ip->protocol==PROTO_UDP)) {
			ip_header_print(ip, ret, argv[1]);
			udp_print(ip, buffer);
		}
		// 수신한 패킷이 ICMP, TCP, UDP가 아닐경우 continue 
		else continue;
		// script 명령어를 통해 파일에 수신한 패킷정보를 저장하기 때문에 1초마다 수신할 수 있게함 
		sleep(1);
	}
	close(sock_eth);
	return 0;
}
// ip 헤더 출력하는 함수 
void ip_header_print(struct ip_header_t *ip, int ret, char *ptl_name) {
	struct in_addr src, dst;
	src.s_addr = ip->src_ip;
	dst.s_addr = ip->dst_ip;
	printf("==========================================\n");
	printf("rx data length from socket = %d\n", ret);
	printf("================ IP header ===============\n");
	printf("Src ip = %s\n", inet_ntoa(src));
	printf("Dst ip = %s\n", inet_ntoa(dst));
	printf("Total length = %d\n", ntohs(ip->length));
	printf("Identification = %d\n", ntohs(ip->id));
	printf("TTL = %d\n", ip->ttl);
	printf("PROTOCOL = %s\n", ptl_name); // 사용자가 보기 쉽게 protocol을 숫자가 아닌 문자열로 출력
	printf("Checksum = 0x%X\n", ntohs(ip->checksum));
}
// icmp 헤더와 데이터를 출력하는 함수
void icmp_print(struct ip_header_t *ip, char *buffer) {
	// icmp 헤더는 이더넷 헤더 다음 ip 헤더 다음에 있기 때문에 
	// buffer에 이더넷과 ip헤더의 크기만큼 더해줌 
	struct icmp_header_t *icmp = (struct icmp_header_t *)(buffer + sizeof(struct ethhdr) + (4 * ip->hlen));
	// 데이터를 출력하기 위해 icmp 헤더 크기를 구함
	int h_size = sizeof(struct icmp_header_t);
	printf("=============== ICMP header ==============\n");
	printf("Type = %d\n", icmp->type);
	printf("Code = %d\n", icmp->code);
	printf("Seq = %d\n", ntohs(icmp->seq));
	printf("Iden = %d\n", ntohs(icmp->id));
	printf("Checksum = 0x%X\n", ntohs(icmp->checksum));
	data_print(ip, buffer, h_size);
}
// tcp 헤더와 데이터를 출력하는 함수
void tcp_print(struct ip_header_t *ip, char *buffer) {
	// tcp 헤더는 이더넷 헤더 다음 ip 헤더 다음에 있기 때문에 
	// buffer에 이더넷과 ip헤더의 크기만큼 더해줌
	struct tcp_header_t *tcp = (struct tcp_header_t *)(buffer + sizeof(struct ethhdr) + (4 * ip->hlen));
	// 데이터를 출력하기 위해 tcp 헤더 크기를 구함
	int h_size = sizeof(4 * tcp->hlen);
	printf("=============== TCP header ==============\n");
	printf("Src port = %d\n", ntohs(tcp->src_port));
	printf("Dst port = %d\n", ntohs(tcp->dst_port));
	printf("Sequence number = 0x%X\n", ntohl(tcp->seqnum));
	printf("Acknowledgment number = 0x%X\n", ntohl(tcp->acknum));
	printf("Checksum = 0x%X\n", ntohs(tcp->checksum));
	data_print(ip, buffer, h_size);
}
// udp 헤더와 데이터를 출력하는 함수
void udp_print(struct ip_header_t *ip, char *buffer) {
	// udp 헤더는 이더넷 헤더 다음 ip 헤더 다음에 있기 때문에 
	// buffer에 이더넷과 ip헤더의 크기만큼 더해줌 
	struct udp_header_t *udp = (struct udp_header_t *)(buffer + sizeof(struct ethhdr) + (4 * ip->hlen));
	// 데이터를 출력하기 위해 udp 헤더 크기를 구함
	int h_size = sizeof(struct udp_header_t);
	printf("=============== UDP header ==============\n");
	printf("Src port = %d\n", ntohs(udp->src_port));
	printf("Dst port = %d\n", ntohs(udp->dst_port));
	printf("length = %d\n", ntohs(udp->length));
	printf("Checksum = 0x%X\n", ntohs(udp->checksum));
	data_print(ip, buffer, h_size);
}
// 각 패킷의 데이터부분을 출력하는 함수 
void data_print(struct ip_header_t *ip, char *buffer, int h_size) {
	printf("=============== DATA field ==============\n");
	// 각 패킷의 데이터부분은 이더넷 헤더 다음 ip 헤더 다음 각 패킷의 헤더 다음에 있기 때문에
	// buffer에 이더넷헤더, ip헤더크기, 각 패킷의 헤더크기를 더해줌 
	char *data = (char *)(buffer + sizeof(struct ethhdr) + (4 * ip->hlen) + h_size);
	while(isprint(*data) || isspace(*data)) {
		printf("%c", *data);
		data++;
	}
	printf("\n=========================================\n\n\n");
}
/******************   eof  ***********************/
