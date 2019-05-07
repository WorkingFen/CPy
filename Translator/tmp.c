//Funkcja silnia
int silnia(int x) {
	if(x == 0)
		return 1;
	else
		return x * silnia(x - 1);
}


/*Funkcja Hello World */
#include <stdio.h>

int main() {
	printf("Hello, world!");
	return 0;
}