//=================================================
//	gcc -o myping myping.c 
//	./myping domain ping_number
//=================================================
#include <stdlib.h>
#include <string.h>
#include <netdb.h> // 도메인을 32비트 IP 주소로 변환하기 위해 헤더파일 추가 
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h> // gettimeofday() 함수를 사용하기 위해 헤더파일 추가
#include <stdio.h>
#include <unistd.h>

int in_cksum(u_short *p, int n);

int main(int argc, char **argv) {
	int icmp_socket;
	/* for문을 실행하기 위해 그리고 ping을 통해 보낸 수와 받은 수를 계산하기 위해 변수 추가 */
	int ret, i, sendcnt=atoi(argv[2]), recvcnt=1;
	struct icmp *p, *rp;
	struct sockaddr_in addr, from;
	struct ip *ip;
	/* 왕복지연 시간 즉, RTT를 구하기 위해 구조체 timeval 변수 추가 */
	struct timeval sendtv, recvtv, gap;
	/* 도메인 정보를 저장하기 위해 구조체 hostent 변수 추가 */
	struct hostent *myhost;
	char buffer[1024];
	int sl;
	int hlen, icmplen; // icmp 메시지의 크기를 구하기 위해 변수 icmplen 추가 
	int rtt[sendcnt]; // 최소, 최대, 평균 RTT를 구하기 위해 argv[2] 수만큼의 배열 추가 
	int max_rtt, min_rtt, sum_rtt=0; // 최소, 최대, 평균 RTT를 구하기 위한 변수 추가 
	
	if(argc != 3) {
		printf("usage: %s host_name ping_number\n", argv[0]);
		exit(0);
	}
	
	// ICMP에대응하는 RAW 소켓을 생성한다.
	icmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(icmp_socket < 0) {
		perror("socket error : ");
		exit(0);
	}
	/* argv[1]의 도메인의 정보를 저장 */
	myhost = gethostbyname(argv[1]);
	if(myhost == 0) {
		perror("error at gethostbyname : ");
		exit(0);
	}
	memset(&addr, 0, sizeof(addr));
	// 도메인에 여러 IP가 있을 수 있지만 첫 번째 IP만 주소 정보에 저장
	addr.sin_addr.s_addr = *((u_long *)myhost->h_addr_list[0]);
	addr.sin_family = AF_INET;
	/* 요청 개수만큼 for문을 실행 */
	for(i=0; i<sendcnt; i++){
		memset(buffer, 0x00, 1024);
		// icmp 패킷을 만든다.
		p = (struct icmp *)buffer;
		p->icmp_type = ICMP_ECHO;
		p->icmp_code = 0;
		p->icmp_cksum = 0;
		p->icmp_seq = recvcnt++; // icmp_seq를 1부터 차례대로 증가시킴 
		p->icmp_id = getpid();
		//p->icmp_cksum = in_cksum((u_short *)p, 1000);
		p->icmp_cksum = in_cksum((u_short *)p, sizeof(*p));
		
		gettimeofday(&sendtv, NULL); // sendto 하기 전에 gettimeofday 함수를 통해 현재 시간 정보를 저장 
		// icmp 패킷을 상대 호스트로 전송한다.
		ret = sendto(icmp_socket,p,sizeof(*p), 0, (struct sockaddr *)&addr, sizeof(addr));
		if (ret< 0) {
			perror("sendto error : ");
			exit(0);
		}
		// 상대 호스트로 부터 icmp 응답을 기다린다.
		sl = sizeof(from);
		// ret이 ping에서  
		ret = recvfrom(icmp_socket, buffer, 1024, 0, (struct sockaddr *)&from, &sl);
		gettimeofday(&recvtv, NULL); // recvfrom 후에 gettimeofday 함수를 통해 현재 시간 정보를 저장
		
		if (ret < 0) {
			//printf("%d %d %d\n", ret, errno, EAGAIN);
			perror("recvfrom error : ");
			exit(0);
		}
		// 왕복 시간을 구하기 위해 요청 응답시간 차를 구함
		gap.tv_sec = recvtv.tv_sec - sendtv.tv_sec;
		gap.tv_usec = recvtv.tv_usec - sendtv.tv_usec;
		// 만약 마이크로 초가 0보다 작을 경우
		// 초에서 1초를 빼고 마이크로 초를 1초 만큼 증가시켜 음수를 없앰
		if(gap.tv_usec < 0) { 
			gap.tv_sec -= 1;
			gap.tv_usec += 1000000;
		}
		// 최소, 최대, 평균 RTT를 구하기 위해 배열에 저장
		// 밀리초(ms)는 초*1000, 마이크로 초/1000 
		rtt[i] = gap.tv_sec*1000 + gap.tv_usec/1000;
		// 응답을 받았다면
		// 해당 호스트의 IP정보와
		// icmp 패킷 정보를 출력한다.
		ip = (struct ip *)buffer;
		hlen = ip->ip_hl*4;
		//recvfrom 할때 ip 헤더 크기도 포함되어 있기때문에
		//전체 크기에서 ip 헤더크기를 빼서 icmp 메시지 크기를 구함
		icmplen = ret - hlen;
		rp = (struct icmp *)(buffer+hlen);
		if(i==0) printf("PING %s [%s] %d bytes of data.\n", myhost->h_name, inet_ntoa(from.sin_addr), icmplen);
		printf("Reply from %s: \n", inet_ntoa(from.sin_addr));
		printf("Type : %d ", rp->icmp_type);
		printf("Code : %d ", rp->icmp_code);
		printf("Seq  : %d ", rp->icmp_seq);
		printf("Iden : %d ", rp->icmp_id);
		printf("Byte=%d, Time=%dms, TTL=%d\n", icmplen, rtt[i], ip->ip_ttl); // byte, rtt, ttl를 출력 
		sleep(1); // 1초마다 ping을 통해 주고 받기 위해 sleep() 함수 사용 
	}
	
	max_rtt = min_rtt = rtt[0];
	// 배열에 담긴 rtt[]에서 최소, 최대, 총합 rtt를 구함 
	for(i=0; i<recvcnt-1; i++) {
		if(max_rtt < rtt[i]) max_rtt = rtt[i];
		if(min_rtt > rtt[i]) min_rtt = rtt[i];
		sum_rtt += rtt[i];
	}
	/* ping 결과 출력 */
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
