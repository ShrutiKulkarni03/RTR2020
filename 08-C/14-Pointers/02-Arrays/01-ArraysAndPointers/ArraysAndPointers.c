#include<stdio.h>

int main(void)
{
	//variable declaration
	int iArray[] = { 3,6,9,12,15,18,21,24,27,30 };
	float fArray[] = { 1.1f, 2.2f, 3.3f, 4.4f, 5.5f};
	double dArray[] = { 1.111111, 2.222222, 3.333333};
	char cArray[] = { 'R', 'T', 'R', '\0' };

	//code
	printf("\n\nInteger Array Elements and the Addresses they Occupy are as Follows : \n\n");
	printf("iArray[0] = %d at Address : %p\n", *(iArray + 0), (iArray + 0));
	printf("iArray[1] = %d at Address : %p\n", *(iArray + 1), (iArray + 1));
	printf("iArray[2] = %d at Address : %p\n", *(iArray + 2), (iArray + 2));
	printf("iArray[3] = %d at Address : %p\n", *(iArray + 3), (iArray + 3));
	printf("iArray[4] = %d at Address : %p\n", *(iArray + 4), (iArray + 4));
	printf("iArray[5] = %d at Address : %p\n", *(iArray + 5), (iArray + 5));
	printf("iArray[6] = %d at Address : %p\n", *(iArray + 6), (iArray + 6));
	printf("iArray[7] = %d at Address : %p\n", *(iArray + 7), (iArray + 7));
	printf("iArray[8] = %d at Address : %p\n", *(iArray + 8), (iArray + 8));
	printf("iArray[9] = %d at Address : %p\n", *(iArray + 9), (iArray + 9));

	printf("\n\nFloat Array Elements and the Addresses they Occupy are as Follows : \n\n");
	printf("fArray[0] = %f at Address : %p\n", *(fArray + 0), (fArray + 0));
	printf("fArray[1] = %f at Address : %p\n", *(fArray + 1), (fArray + 1));
	printf("fArray[2] = %f at Address : %p\n", *(fArray + 2), (fArray + 2));
	printf("fArray[3] = %f at Address : %p\n", *(fArray + 3), (fArray + 3));
	printf("fArray[4] = %f at Address : %p\n", *(fArray + 4), (fArray + 4));

	printf("\n\n'Double' Array Elements and the Addresses they Occupy are as Follows : \n\n");
	printf("dArray[0] = %lf at Address : %p\n", *(dArray + 0), (dArray + 0));
	printf("dArray[1] = %lf at Address : %p\n", *(dArray + 1), (dArray + 1));
	printf("dArray[2] = %lf at Address : %p\n", *(dArray + 2), (dArray + 2));
	
	printf("\n\nCharacter Array Elements and the Addresses they Occupy are as Follows : \n\n");
	printf("cArray[0] = %c at Address : %p\n", *(cArray + 0), (cArray + 0));
	printf("cArray[1] = %c at Address : %p\n", *(cArray + 1), (cArray + 1));
	printf("cArray[2] = %c at Address : %p\n", *(cArray + 2), (cArray + 2));
	printf("cArray[3] = %c at Address : %p\n", *(cArray + 3), (cArray + 3));
	

	getch();
	return(0);

}
