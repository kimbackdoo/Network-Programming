/* -------------------------------------------------------------------
파일명 : ip_addr.c
기  능 : IP 주소변환 예제 프로그램
리눅스 : gcc -o ip_addr ip_addr.c
유닉스 : gcc -o ip_addr ip_addr.c -lsocket -lnsl
사용법 : ./ip_addr dcu.ac.kr
------------------------------------------------------------------- */

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char * argv[]){
	
	struct hostent *myhost;		/* 호스트의 각종 정보를 저장할 구조체 */
	struct in_addr myinaddr;	/* IP 주소를 저장할 구조체 */
	int i;
	
	if(argc < 2){
		printf("사용법 : %s host_name(도메인 네임) \n", argv[0]);
		exit(0);
	}
	
	/* 입력한 도메인을 출력 */
	printf("입력한 도메인: %s\n\n", argv[1]);
	
	/* hostent 구조체 구하기 */
	myhost = gethostbyname(argv[1]);
	
	if(myhost == 0){
		printf("error at gethostbyname \n");
		exit(0);
	}
	
	/* 호스트 주소 변환 부분 */
	i = 0;
	while(myhost->h_addr_list[i] != NULL){
		myinaddr.s_addr = *( (u_long *)(myhost->h_addr_list[i]) );
		
		/* 호스트 주소를 이진수 IP (출력은 16진수) 형태로 출력 */
		printf("IP 주소(2진수, ntohl X): 0x%0x(16 진수)\n", myinaddr.s_addr);
		printf("IP 주소(2진수, ntohl O): 0x%0x(16 진수)\n\n", ntohl(myinaddr.s_addr));
		
		/* 호스트 주소를 dotted decimal 형태로 출력 */
		printf("IP address : %s\n", inet_ntoa(myinaddr));
		i++;
	}
	
	return 0;
}
