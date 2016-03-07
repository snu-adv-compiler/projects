#include <stdio.h>
#include <stdlib.h>

int main(){
	int a, b, c =0;
	int* d = (int *) malloc(11);	

	for(int a = 0; a < 10; a++){
		if(a >= 4){
			for(int i = 0; i < 10 ; i++){
				c = c * 3;
				d[i] = c;
			}
		}

		else if(a >= 8){
			c = c + 1;
			b = b * c;
		}

		else{
			c = c * 2;
		}
	}

	for(int i = 0 ; i < 10; i++){
		printf("%d", d[i]);
	}

	printf("%d%d%d", a, b, c);

	free(d);

	return 0;
}
