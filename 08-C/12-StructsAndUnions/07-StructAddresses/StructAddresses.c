#include<stdio.h>

struct S_Data
{
	int si;
	float sf;
	double sd;
	char sc;
};

int main(void)
{
	//variable declaration
	struct S_Data data;

	//code
	data.si = 17;
	data.sf = 7.1066;
	data.sd = 3.1097;
	data.sc = 'S';

	printf("\n\nData Members of 'struct S_Data' are : \n\n");
	printf("i = %d\n", data.si);
	printf("f = %f\n", data.sf);
	printf("d = %lf\n", data.sd);
	printf("c = %c\n", data.sc);

	printf("\n\nAddresses of Data Members of 'struct S-Data' variable 'data' are :\n\n");
	printf("'i' occupies address from %p\n", &data.si);
	printf("'f' occupies address from %p\n", &data.sf);
	printf("'d' occupies address from %p\n", &data.sd);
	printf("'c' occupies address from %p\n", &data.sc);

	getch();
	return(0);

}
