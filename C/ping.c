//=================================================
//	gcc -o myping myping.c 
//	./myping domain ping_number
//=================================================
#include <stdlib.h>
#include <string.h>
#include <netdb.h> // �������� 32��Ʈ IP �ּҷ� ��ȯ�ϱ� ���� ������� �߰� 
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h> // gettimeofday() �Լ��� ����ϱ� ���� ������� �߰�
#include <stdio.h>
#include <unistd.h>

int in_cksum(u_short *p, int n);

int main(int argc, char **argv) {
	int icmp_socket;
	/* for���� �����ϱ� ���� �׸��� ping�� ���� ���� ���� ���� ���� ����ϱ� ���� ���� �߰� */
	int ret, i, sendcnt=atoi(argv[2]), recvcnt=1;
	struct icmp *p, *rp;
	struct sockaddr_in addr, from;
	struct ip *ip;
	/* �պ����� �ð� ��, RTT�� ���ϱ� ���� ����ü timeval ���� �߰� */
	struct timeval sendtv, recvtv, gap;
	/* ������ ������ �����ϱ� ���� ����ü hostent ���� �߰� */
	struct hostent *myhost;
	char buffer[1024];
	int sl;
	int hlen, icmplen; // icmp �޽����� ũ�⸦ ���ϱ� ���� ���� icmplen �߰� 
	int rtt[sendcnt]; // �ּ�, �ִ�, ��� RTT�� ���ϱ� ���� argv[2] ����ŭ�� �迭 �߰� 
	int max_rtt, min_rtt, sum_rtt=0; // �ּ�, �ִ�, ��� RTT�� ���ϱ� ���� ���� �߰� 
	
	if(argc != 3) {
		printf("usage: %s host_name ping_number\n", argv[0]);
		exit(0);
	}
	
	// ICMP�������ϴ� RAW ������ �����Ѵ�.
	icmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(icmp_socket < 0) {
		perror("socket error : ");
		exit(0);
	}
	/* argv[1]�� �������� ������ ���� */
	myhost = gethostbyname(argv[1]);
	if(myhost == 0) {
		perror("error at gethostbyname : ");
		exit(0);
	}
	memset(&addr, 0, sizeof(addr));
	// �����ο� ���� IP�� ���� �� ������ ù ��° IP�� �ּ� ������ ����
	addr.sin_addr.s_addr = *((u_long *)myhost->h_addr_list[0]);
	addr.sin_family = AF_INET;
	/* ��û ������ŭ for���� ���� */
	for(i=0; i<sendcnt; i++){
		memset(buffer, 0x00, 1024);
		// icmp ��Ŷ�� �����.
		p = (struct icmp *)buffer;
		p->icmp_type = ICMP_ECHO;
		p->icmp_code = 0;
		p->icmp_cksum = 0;
		p->icmp_seq = recvcnt++; // icmp_seq�� 1���� ���ʴ�� ������Ŵ 
		p->icmp_id = getpid();
		//p->icmp_cksum = in_cksum((u_short *)p, 1000);
		p->icmp_cksum = in_cksum((u_short *)p, sizeof(*p));
		
		gettimeofday(&sendtv, NULL); // sendto �ϱ� ���� gettimeofday �Լ��� ���� ���� �ð� ������ ���� 
		// icmp ��Ŷ�� ��� ȣ��Ʈ�� �����Ѵ�.
		ret = sendto(icmp_socket,p,sizeof(*p), 0, (struct sockaddr *)&addr, sizeof(addr));
		if (ret< 0) {
			perror("sendto error : ");
			exit(0);
		}
		// ��� ȣ��Ʈ�� ���� icmp ������ ��ٸ���.
		sl = sizeof(from);
		// ret�� ping����  
		ret = recvfrom(icmp_socket, buffer, 1024, 0, (struct sockaddr *)&from, &sl);
		gettimeofday(&recvtv, NULL); // recvfrom �Ŀ� gettimeofday �Լ��� ���� ���� �ð� ������ ����
		
		if (ret < 0) {
			//printf("%d %d %d\n", ret, errno, EAGAIN);
			perror("recvfrom error : ");
			exit(0);
		}
		// �պ� �ð��� ���ϱ� ���� ��û ����ð� ���� ����
		gap.tv_sec = recvtv.tv_sec - sendtv.tv_sec;
		gap.tv_usec = recvtv.tv_usec - sendtv.tv_usec;
		// ���� ����ũ�� �ʰ� 0���� ���� ���
		// �ʿ��� 1�ʸ� ���� ����ũ�� �ʸ� 1�� ��ŭ �������� ������ ����
		if(gap.tv_usec < 0) { 
			gap.tv_sec -= 1;
			gap.tv_usec += 1000000;
		}
		// �ּ�, �ִ�, ��� RTT�� ���ϱ� ���� �迭�� ����
		// �и���(ms)�� ��*1000, ����ũ�� ��/1000 
		rtt[i] = gap.tv_sec*1000 + gap.tv_usec/1000;
		// ������ �޾Ҵٸ�
		// �ش� ȣ��Ʈ�� IP������
		// icmp ��Ŷ ������ ����Ѵ�.
		ip = (struct ip *)buffer;
		hlen = ip->ip_hl*4;
		//recvfrom �Ҷ� ip ��� ũ�⵵ ���ԵǾ� �ֱ⶧����
		//��ü ũ�⿡�� ip ���ũ�⸦ ���� icmp �޽��� ũ�⸦ ����
		icmplen = ret - hlen;
		rp = (struct icmp *)(buffer+hlen);
		if(i==0) printf("PING %s [%s] %d bytes of data.\n", myhost->h_name, inet_ntoa(from.sin_addr), icmplen);
		printf("Reply from %s: \n", inet_ntoa(from.sin_addr));
		printf("Type : %d ", rp->icmp_type);
		printf("Code : %d ", rp->icmp_code);
		printf("Seq  : %d ", rp->icmp_seq);
		printf("Iden : %d ", rp->icmp_id);
		printf("Byte=%d, Time=%dms, TTL=%d\n", icmplen, rtt[i], ip->ip_ttl); // byte, rtt, ttl�� ��� 
		sleep(1); // 1�ʸ��� ping�� ���� �ְ� �ޱ� ���� sleep() �Լ� ��� 
	}
	
	max_rtt = min_rtt = rtt[0];
	// �迭�� ��� rtt[]���� �ּ�, �ִ�, ���� rtt�� ���� 
	for(i=0; i<recvcnt-1; i++) {
		if(max_rtt < rtt[i]) max_rtt = rtt[i];
		if(min_rtt > rtt[i]) min_rtt = rtt[i];
		sum_rtt += rtt[i];
	}
	/* ping ��� ��� */
	printf("\n--- %s[%s] ping statistics ---\n", myhost->h_name, inet_ntoa(from.sin_addr));
	printf("%d packets transmitted, %d received, %d%% packet loss\n", sendcnt, recvcnt-1, (sendcnt-recvcnt)/sendcnt*100);
	printf("Time(ms):\n");
	printf("min = %d, max = %d, avg = %d\n\n", min_rtt, max_rtt, sum_rtt/(recvcnt-1));
	return 1;
}

int in_cksum( u_short *p, int n ) {
	register u_short answer;
	register long sum = 0;
	u_short odd_byte = 0;

	while( n > 1 ) {
		sum += *p++;
		n -= 2;
	}

	if( n == 1 ) {
		*( u_char* )( &odd_byte ) = *( u_char* )p;
		sum += odd_byte;
	}

	sum = ( sum >> 16 ) + ( sum & 0xffff );
	sum += ( sum >> 16 );
	answer = ~sum;
	return ( answer );
}
// --------------  end of file ----------------
