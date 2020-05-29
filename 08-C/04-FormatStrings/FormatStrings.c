#include<stdio.h>

int main(void)
{
	printf("\n\nShruti,\"Hello World!\"\n\n");
	
	//char str1[] = "SPK";
	//printf("%s\n", str1);

	char str[] = "Shruti Paresh Kulkarni\nRTR2020-003\n";
	printf("My Details:\n%s", str);

	printf("\n\nNumber System Conversion\n");

	int a, b, c;

	printf("\n\n");
	printf("Enter a Decimal Number\n");
	scanf("%d", &a);
	//printf("\nDecimal Value of the Integer is %d\n", a);
	printf("Octal Value of the Integer is %o\n", a);
	printf("Hexadecimal Value of the Integer is %x\n", a);
	printf("Hexadecimal Value of the Integer is %X\n\n", a);

	printf("Enter an Octal Number\n");
	scanf("%o", &b);
	//printf("\nOctal Value of the Integer is %o\n", b);
	printf("Decimal Value of the Integer is %d\n", b);
	printf("Hexadecimal Value of the Integer is %x\n", b);
	printf("Hexadecimal Value of the Integer is %X\n\n", b);

	printf("Enter a Hexadecimal number in caps\n");
	scanf("%X", &c);
	printf("\nHexadecimal Value of the Integer is %x\n", c);
	printf("Decimal Value of the Integer is %d\n", c);
	printf("Octal Value of the Integer is %o\n\n", c);

	long num = 17021970L;
	printf("Long Integer = %ld\n", num);

	unsigned int usg_i = 7;
	printf("Unsigned Integer = %u\n\n", usg_i);

	float f_num = 0710.1966f;
	printf("Floating Point Number With Just %%f 'f_num' = %f\n", f_num);
	printf("Floating Point Number With %%4.2f 'f_num' = %4.2f\n", f_num);
	printf("Floating Point Number With %%6.5f 'f_num' = %6.5f\n\n", f_num);

	double d_pi = 3.14159265358979323846;
	printf("Double Precision Floating Point Number Without Exponential = %g\n", d_pi);
	printf("Double Precision Floating Point Number With Exponential (Lower Case) = %e\n", d_pi);
	printf("Double Precision Floating Point Number With Exponential (Upper Case) = %E\n\n", d_pi);
	printf("Double Hexadecimal Value Of 'd_pi' (Hexadecimal Letters In Lower Case) = %a\n", d_pi);
	printf("Double Hexadecimal Value Of 'd_pi' (Hexadecimal Letters In Upper Case) = %A\n\n", d_pi);

	getch();
	return(0);

}
