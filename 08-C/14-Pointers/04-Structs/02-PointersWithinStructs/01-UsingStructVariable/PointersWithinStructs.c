#include <stdio.h>

struct SData
{
	int* ptr_si;
	int si;
	float* ptr_sf;
	float sf;
	double* ptr_sd;
	double sd;
};

int main(void)
{
	//variable declarations
	struct SData data;

	//code
	data.si = 20;
	data.ptr_si = &data.si;

	data.sf = 20.03f;
	data.ptr_sf = &data.sf;

	data.sd = 18.32019;
	data.ptr_sd = &data.sd;
		
	printf("\n\ni = %d\n", *(data.ptr_si));
	printf("Adress of 'i' = %p\n", data.ptr_si);
	
	printf("\n\nf = %f\n", *(data.ptr_sf));
	printf("Adress of 'f' = %p\n", data.ptr_sf);
	
	printf("\n\nd = %lf\n", *(data.ptr_sd));
	printf("Adress of 'd' = %p\n", data.ptr_sd);

	getch();
	return(0);
}
