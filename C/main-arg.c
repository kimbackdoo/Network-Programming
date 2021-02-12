/* -----------------------------------------------------------���ϸ� : 8-1-main-arg.c
��  �� : ���ڸ� ���� ���� �Լ��� ���� ���� ���α׷�  
������ : gcc -o arg main-arg.c
���� : ./arg ȫ�浿 55
-----------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char name[20];
	char **arg_string; 
	int age;
	
	if(argc != 3)
		return; 

	arg_string = argv; 
	printf("arg1 = %s, arg2 = %s\n",argv[1], argv[2]);
	printf("string1 = %s, string2 = %s\n",arg_string[1], arg_string[2]);
	
	strcpy(name, argv[1]);
	age = atoi(argv[2]);
	
	printf("name = %s, age = %d\n", name, age);
}
