/* ---------------------------------------------------------------------
파일명 : ascii_ip.c
기  능 : ASCII (dotted edcimal)로 표현된 주소를 4바이트 IP 주소로 변환
컴파일 : cc - o ascii_ip ascii_ip.c
사용법 : ascii_ip 220.69.219.228
--------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char * argv[]){
	char * haddr;
	struct in_addr host_ip;	/* 32bit IP 주소 구조체 */
	if(argc < 2){
		printf("사용법 : %s IP 주소(dotted decimal)\n", argv[0]);
		exit(0);
	}
	haddr = argv[1];				/* dotted decimal 주소 */
	host_ip.s_addr = inet_addr(haddr);
	
	/* IP 주소 (hexa 4바이트) 출력 */
	printf("정수 값 IP 주소(16진수) : 0x%x\n", host_ip.s_addr);
	/* dotted decimal로 다시 변환하여 출력 */
	printf("Dotted Decimal IP 주소로 다시 변환 : %s\n", inet_ntoa(host_ip));
	
	return 0;
}
