/*******************************************
  - Non Blocking Sniffing recvfrom with DONWAITFLAG
  - ./sniffing protocol_name
  - #script ��ɾ �̿��Ͽ� ���Ͽ� ��� ��� ���� 
********************************************/
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/if_ether.h> // ETH_P_ALL�� ethhdr ����ü�� ���� ������� 
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "packet.h"
// buffer ������ ���� 
#define BUF_SIZE 65535
// ���������� C��� Ư������ ���� �Լ� ���� ���� 
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
	// ip�� �迭 buffer�� �����ּҿ� ���� �� 
	// �̴��ݿ��� ��� ��Ŷ�� �������� ������ ip��� �κ��� ����Ű�� �Ϸ��� �̴����� ����� ũ�⸸ŭ ��������� 
	struct ip_header_t *ip = (struct ip_header_t *)(buffer + sizeof(struct ethhdr));
	char *interface = "enp2s0"; // �ý��� ���� ����Ǿ�� �� �������̽�
	 
	if(argc != 2) {
		printf("usage: %s protocol_name\n", argv[0]);
		exit(0);
	} 
	// argv[1]�� ���ڿ��� icmp, tcp, udp ���ڿ��� ���Ͽ� �� ���ڿ��� ������
	// ���� ptl_nul�� icmp=1, tcp=6, udp=17, all=0���� �ʱ�ȭ
	// ���� icmp, tcp, udp, all ���ڿ� �̿��� ���ڿ��� ������ �����޽��� ����ϰ� ���α׷� ���� 
	if(strcmp(argv[1], "icmp") == 0) ptl_num = 1;
	else if(strcmp(argv[1], "tcp") == 0) ptl_num = 6;
	else if(strcmp(argv[1], "udp") == 0) ptl_num = 17;
	else if(strcmp(argv[1], "all") == 0) ptl_num = 0;
	else {
		printf("Input only icmp or tcp or udp or all\n");
		exit(0);
	}
	// socket �Լ����� doamin:PF_PACKET, protocol:ETH_P_ALL�� �����Ͽ� �̴��ݿ� ��� ��Ŷ���� ������ 
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
	// argv[1] ���ڿ��� ���� protocol number�� ������ ptl_num�� ���� ���͸��Ͽ� ��������
	while(1) {
		// recvfrom �Լ��� ȣ��Ǳ� ���� memset �Լ��� ���� buffer�� ũ�⸸ŭ 0x00���� buffer �ʱ�ȭ
		memset(buffer, 0x00, sizeof(buffer));
		// �迭 buffer�� ũ�⸸ŭ nonblocking���� ������ 
		ret = recvfrom(sock_eth, buffer, sizeof(buffer), MSG_DONTWAIT, (struct sockaddr *)&from, &len);
		// recvfrom�Լ��� ������ ��� continue 
		if(ret<0) continue;
		/* recvfrom�Լ��� ������ ��� icmp, tcp, udp ��Ŷ�鿡 ���Ͽ� ����ϰ� ���� icmp, tcp, udp ��Ŷ�� �ƴ϶�� continue */
		// ����ڰ� �Է��� icmp, tcp, udp, all�� ���� ����ڰ� ���ϴ� ��Ŷ �������� ��� 
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
		// ������ ��Ŷ�� ICMP, TCP, UDP�� �ƴҰ�� continue 
		else continue;
		// script ��ɾ ���� ���Ͽ� ������ ��Ŷ������ �����ϱ� ������ 1�ʸ��� ������ �� �ְ��� 
		sleep(1);
	}
	close(sock_eth);
	return 0;
}
// ip ��� ����ϴ� �Լ� 
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
	printf("PROTOCOL = %s\n", ptl_name); // ����ڰ� ���� ���� protocol�� ���ڰ� �ƴ� ���ڿ��� ���
	printf("Checksum = 0x%X\n", ntohs(ip->checksum));
}
// icmp ����� �����͸� ����ϴ� �Լ�
void icmp_print(struct ip_header_t *ip, char *buffer) {
	// icmp ����� �̴��� ��� ���� ip ��� ������ �ֱ� ������ 
	// buffer�� �̴��ݰ� ip����� ũ�⸸ŭ ������ 
	struct icmp_header_t *icmp = (struct icmp_header_t *)(buffer + sizeof(struct ethhdr) + (4 * ip->hlen));
	// �����͸� ����ϱ� ���� icmp ��� ũ�⸦ ����
	int h_size = sizeof(struct icmp_header_t);
	printf("=============== ICMP header ==============\n");
	printf("Type = %d\n", icmp->type);
	printf("Code = %d\n", icmp->code);
	printf("Seq = %d\n", ntohs(icmp->seq));
	printf("Iden = %d\n", ntohs(icmp->id));
	printf("Checksum = 0x%X\n", ntohs(icmp->checksum));
	data_print(ip, buffer, h_size);
}
// tcp ����� �����͸� ����ϴ� �Լ�
void tcp_print(struct ip_header_t *ip, char *buffer) {
	// tcp ����� �̴��� ��� ���� ip ��� ������ �ֱ� ������ 
	// buffer�� �̴��ݰ� ip����� ũ�⸸ŭ ������
	struct tcp_header_t *tcp = (struct tcp_header_t *)(buffer + sizeof(struct ethhdr) + (4 * ip->hlen));
	// �����͸� ����ϱ� ���� tcp ��� ũ�⸦ ����
	int h_size = sizeof(4 * tcp->hlen);
	printf("=============== TCP header ==============\n");
	printf("Src port = %d\n", ntohs(tcp->src_port));
	printf("Dst port = %d\n", ntohs(tcp->dst_port));
	printf("Sequence number = 0x%X\n", ntohl(tcp->seqnum));
	printf("Acknowledgment number = 0x%X\n", ntohl(tcp->acknum));
	printf("Checksum = 0x%X\n", ntohs(tcp->checksum));
	data_print(ip, buffer, h_size);
}
// udp ����� �����͸� ����ϴ� �Լ�
void udp_print(struct ip_header_t *ip, char *buffer) {
	// udp ����� �̴��� ��� ���� ip ��� ������ �ֱ� ������ 
	// buffer�� �̴��ݰ� ip����� ũ�⸸ŭ ������ 
	struct udp_header_t *udp = (struct udp_header_t *)(buffer + sizeof(struct ethhdr) + (4 * ip->hlen));
	// �����͸� ����ϱ� ���� udp ��� ũ�⸦ ����
	int h_size = sizeof(struct udp_header_t);
	printf("=============== UDP header ==============\n");
	printf("Src port = %d\n", ntohs(udp->src_port));
	printf("Dst port = %d\n", ntohs(udp->dst_port));
	printf("length = %d\n", ntohs(udp->length));
	printf("Checksum = 0x%X\n", ntohs(udp->checksum));
	data_print(ip, buffer, h_size);
}
// �� ��Ŷ�� �����ͺκ��� ����ϴ� �Լ� 
void data_print(struct ip_header_t *ip, char *buffer, int h_size) {
	printf("=============== DATA field ==============\n");
	// �� ��Ŷ�� �����ͺκ��� �̴��� ��� ���� ip ��� ���� �� ��Ŷ�� ��� ������ �ֱ� ������
	// buffer�� �̴������, ip���ũ��, �� ��Ŷ�� ���ũ�⸦ ������ 
	char *data = (char *)(buffer + sizeof(struct ethhdr) + (4 * ip->hlen) + h_size);
	while(isprint(*data) || isspace(*data)) {
		printf("%c", *data);
		data++;
	}
	printf("\n=========================================\n\n\n");
}
/******************   eof  ***********************/
