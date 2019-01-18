/*
 ============================================================================
 Name        : Lab1_openmp.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello OpenMP World in C
 ============================================================================
 */
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int i;
	int n = 8;

#pragma omp parallel for
	for (i = 0; i < n; i++)
	{
		printf("%d\n", i);
	}

	return 0;
}


