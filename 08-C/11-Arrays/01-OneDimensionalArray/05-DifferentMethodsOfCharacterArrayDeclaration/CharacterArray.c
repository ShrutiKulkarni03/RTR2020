#include<stdio.h>

int main(void)
{
	char cArray1[] = { 'S', 'H', 'R', 'U', 'T', 'I', '\0' };
	char cArray2[7] = { 'P', 'A', 'R', 'E', 'S', 'H', '\0' };
	char cArray3[] = { 'K', 'U', 'L', 'K', 'A', 'R', 'N', 'I', '\0' };
	char cArray4[] = "RTR";
	char cArray5[] = "003";
	char cArray_WithoutNullTerminator[] = { 'H', 'e', 'l', 'l', 'o' };

	printf("\n\nSize of cArray1 is %lu\n", sizeof(cArray1));
	printf("Size of cArray2 is %lu\n", sizeof(cArray2));
	printf("Size of cArray3 is %lu\n", sizeof(cArray3));
	printf("Size of cArray4 is %lu\n", sizeof(cArray4));
	printf("Size of cArray5 is %lu\n", sizeof(cArray5));

	printf("\n\nThe Strings are\n");
	printf("cArray1 : %s\n", cArray1);
	printf("cArray2 : %s\n", cArray2);
	printf("cArray3 : %s\n", cArray3);
	printf("cArray4 : %s\n", cArray4);
	printf("cArray5 : %s\n", cArray5);

	printf("\nSize of cArray_WithoutNullTerminator is %lu\n", sizeof(cArray_WithoutNullTerminator));
	printf("cArray_WithoutNullTerminator is %s\n", cArray_WithoutNullTerminator);

	getch();
	return(0);

}

