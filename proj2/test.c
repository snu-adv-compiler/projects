#include <stdio.h>
#include <stdlib.h>

int main(){
	int a, c =1;
	int b = 1;
	int e = 0;
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
			while(1){
				if(b > 3) break;
			}
		}

		else{
			c = c * 2;
		}
	}


	for(int i = 0 ; i < 10; i++){
		printf("%d", d[i]);
		while(1){
			e++;
			if(e > 10){
				printf("%d", e);
				break;
			}
			else{
				a = b*c;
				printf("%d", a);
			}
		}
	}

	printf("%d%d%d", a, b, c);

	free(d);

	return 0;
}
