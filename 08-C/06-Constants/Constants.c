#include<stdio.h>
#define My_PI 3.141592653
#define AMC_STRING "Real Time Rendering 3.0"

enum Boolean
{
	TRUE = 1,
	FALSE = 0
};

enum WEEK
{
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
	SUNDAY
};

enum MONTH
{
	JANUARY = 1,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
};

enum NUMBERS
{
	ONE,
	TWO,
	THREE,
	FOUR = 4,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	TEN
};

int main(void)
{
	printf("\n\n");

	printf("Boolean\n");
	printf("Value of True  = %d\n", TRUE);
	printf("Value of False = %d\n\n", FALSE);

	printf("Week\n");
	printf("Monday    = %d\n", MONDAY);
	printf("Tuesday   = %d\n", TUESDAY);
	printf("Wednesday = %d\n", WEDNESDAY);
	printf("Thursday  = %d\n", THURSDAY);
	printf("Friday    = %d\n", FRIDAY);
	printf("Saturday  = %d\n", SATURDAY);
	printf("Sunday    = %d\n\n", SUNDAY);

	printf("Month\n");
	printf("January   = %d\n", JANUARY);
	printf("February  = %d\n", FEBRUARY);
	printf("March     = %d\n", MARCH);
	printf("April     = %d\n", APRIL);
	printf("May       = %d\n", MAY);
	printf("June      = %d\n", JUNE);
	printf("July      = %d\n", JULY);
	printf("August    = %d\n", AUGUST);
	printf("September = %d\n", SEPTEMBER);
	printf("October   = %d\n", OCTOBER);
	printf("November  = %d\n", NOVEMBER);
	printf("December  = %d\n\n", DECEMBER);

	printf("Numbers\n");
	printf("ONE is numeric    = %d\n", ONE);
	printf("TWO in numeric    = %d\n", TWO);
	printf("THREE in numeric  = %d\n", THREE);
	printf("FOUR in numeric   = %d\n", FOUR);
	printf("FIVE in numeric   = %d\n", FIVE);
	printf("SIX in numeric    = %d\n", SIX);
	printf("SEVEN in numeric  = %d\n", SEVEN);
	printf("EIGHT in numeric  = %d\n", EIGHT);
	printf("NINE in numeric   = %d\n", NINE);
	printf("TEN in numeric    = %d\n\n", TEN);

	printf(AMC_STRING);
	printf("\nAMC_Batch - %s\n\n", AMC_STRING);
	
	float r;

	printf("My_PI Macro value is %.10lf\n", My_PI);

	printf("Enter radius of circle:\n");
	scanf("%f", &r);

	printf("Area of circle with radius %f = %f", r, My_PI * r * r);
	printf("\n\n");

	getch();
	return(0);

}
