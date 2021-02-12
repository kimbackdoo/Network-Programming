/* ---------------------------------------------------------------------
���ϸ� : ascii_ip.c
��  �� : ASCII (dotted edcimal)�� ǥ���� �ּҸ� 4����Ʈ IP �ּҷ� ��ȯ
������ : cc - o ascii_ip ascii_ip.c
���� : ascii_ip 220.69.219.228
--------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char * argv[]){
	char * haddr;
	struct in_addr host_ip;	/* 32bit IP �ּ� ����ü */
	if(argc < 2){
		printf("���� : %s IP �ּ�(dotted decimal)\n", argv[0]);
		exit(0);
	}
	haddr = argv[1];				/* dotted decimal �ּ� */
	host_ip.s_addr = inet_addr(haddr);
	
	/* IP �ּ� (hexa 4����Ʈ) ��� */
	printf("���� �� IP �ּ�(16����) : 0x%x\n", host_ip.s_addr);
	/* dotted decimal�� �ٽ� ��ȯ�Ͽ� ��� */
	printf("Dotted Decimal IP �ּҷ� �ٽ� ��ȯ : %s\n", inet_ntoa(host_ip));
	
	return 0;
}
