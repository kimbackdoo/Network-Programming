#include <stdio.h>

typedef struct{
	int myint;
	char mychar;
}myst;

typedef struct{
	int myint;
	char mychar;
}__attribute__((packed)) mysta;

int main(void)
{
	myst smyst;
	mysta smysta;

	printf("size :  %d\n", sizeof(smyst));
	printf("__attribute__((packed)) size : %d\n", sizeof(smysta));

	return 0;
}
