#include<stdio.h>

int main(void)
{
	//variable declaration
	int iArray[] = { 3,6,9,12,15,18,21,24,27,30 };
	int* ptr_iArray = NULL;

	//code
	printf("\n\nUsing Array Name as Pointer i.e. : Value of X Element of iArray : *(iArray + X) and Address of X Elements of iArray : (iArray + X)\n\n");
	printf("Integer Array Elements and their Addresses : \n\n");
	printf("*(iArray + 0) = %d at Address (iArray + 0) : %p\n", *(iArray + 0), (iArray + 0));
	printf("*(iArray + 1) = %d at Address (iArray + 1) : %p\n", *(iArray + 1), (iArray + 1));
	printf("*(iArray + 2) = %d at Address (iArray + 2) : %p\n", *(iArray + 2), (iArray + 2));
	printf("*(iArray + 3) = %d at Address (iArray + 3) : %p\n", *(iArray + 3), (iArray + 3));
	printf("*(iArray + 4) = %d at Address (iArray + 4) : %p\n", *(iArray + 4), (iArray + 4));
	printf("*(iArray + 5) = %d at Address (iArray + 5) : %p\n", *(iArray + 5), (iArray + 5));
	printf("*(iArray + 6) = %d at Address (iArray + 6) : %p\n", *(iArray + 6), (iArray + 6));
	printf("*(iArray + 7) = %d at Address (iArray + 7) : %p\n", *(iArray + 7), (iArray + 7));
	printf("*(iArray + 8) = %d at Address (iArray + 8) : %p\n", *(iArray + 8), (iArray + 8));
	printf("*(iArray + 9) = %d at Address (iArray + 9) : %p\n", *(iArray + 9), (iArray + 9));

	ptr_iArray = iArray;

	printf("\n\nUsing Pointer as Array Name i.e. : Value of X Element of iArray : ptr_iArray[X] and Address of X Element of iArray : &ptr_iArray[X]\n\n");
	printf("Integer Array Elements and their Addresses : \n\n");
	printf("ptr_iArray[0] = %d at Address &ptr_iArray[0] : %p\n", ptr_iArray[0], &ptr_iArray[0]);
	printf("ptr_iArray[1] = %d at Address &ptr_iArray[1] : %p\n", ptr_iArray[1], &ptr_iArray[1]);
	printf("ptr_iArray[2] = %d at Address &ptr_iArray[2] : %p\n", ptr_iArray[2], &ptr_iArray[2]);
	printf("ptr_iArray[3] = %d at Address &ptr_iArray[3] : %p\n", ptr_iArray[3], &ptr_iArray[3]);
	printf("ptr_iArray[4] = %d at Address &ptr_iArray[4] : %p\n", ptr_iArray[4], &ptr_iArray[4]);
	printf("ptr_iArray[5] = %d at Address &ptr_iArray[5] : %p\n", ptr_iArray[5], &ptr_iArray[5]);
	printf("ptr_iArray[6] = %d at Address &ptr_iArray[6] : %p\n", ptr_iArray[6], &ptr_iArray[6]);
	printf("ptr_iArray[7] = %d at Address &ptr_iArray[7] : %p\n", ptr_iArray[7], &ptr_iArray[7]);
	printf("ptr_iArray[8] = %d at Address &ptr_iArray[8] : %p\n", ptr_iArray[8], &ptr_iArray[8]);
	printf("ptr_iArray[9] = %d at Address &ptr_iArray[9] : %p\n", ptr_iArray[9], &ptr_iArray[9]);

	getch();
	return(0);

}
