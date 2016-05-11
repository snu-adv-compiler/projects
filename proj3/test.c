#include <stdio.h>
#include <stdlib.h>


int foo(int a, int f)
{

	int b = -100, c, d, e;
	int y = 1, z=-1, x = 1;
	if(f > b)
	{
		if(a > 5)
		{
			c = 33;
			if (a < 10)
				b += -5;
			else
				b /= 3;

			if (b > -40)
				e = 2001;
			else e = -2;
		}
		else
		{
			b = -1;
			c = -1;
			e = -1;
		}
	}

	else
	{
		c = 0;
		e = 19;	
		b = -11111/a;
	}
	d = b + c * e;
	x = 39 / (y + z + 2);
	return x / 10 / d / c / b;
}


int main(){
	int a, c = 1;
	int b = 1;
	int* d = (int *) malloc(11);	
	int e = -1;
	int f = 0;

	for(int a = 0; a < 10; a++)
	{
		if(a >= 4)
		{
			for(int i = 0; i < 10 ; i++)
			{
				c = c * 3;
				d[i] = c;
			}
		}

		else if(a >= 8){
			c = c + 1;
			b = b * c;
			while(1){
				if(b > 3) break;
			}
		}

		else
			c = c * 2;
	}
	a = 1; 
	c = 1;
	foo(c, a);	
	return 0;
}



