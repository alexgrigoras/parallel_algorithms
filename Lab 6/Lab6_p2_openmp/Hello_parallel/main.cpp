/*
 ============================================================================
 Name        : lab6_p2_openmp.c
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : Compression (Reduction) in OpenMP
 ============================================================================
 */
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

	int A[16] = { 0,0,0,0,0,0,0,0,4,8,5,2,10,6,9,11 };
	int n = sizeof(A) / sizeof(int);
	int m = 3;
	int k, j;
	int i;
	int tid;

	omp_set_num_threads(16);

	for (k = m - 1; k >= 0; k--)
	{
		int doi_la_k = 1 << k;
		int doi_la_k_plus_unu = 1 << (k + 1);
#pragma omp parallel for private(j) shared(A, doi_la_k, doi_la_k_plus_unu)
		for (j = doi_la_k; j <= doi_la_k_plus_unu - 1; j++)
		{
			tid = omp_get_thread_num();
			A[j] = A[2 * j] + A[2 * j + 1];
			//printf("thread %d\n", tid);
		}
	}

	for (i = 0; i < n; i++)
	{
		printf("A[%d]= %d\n", i, A[i]);
	}

	return 0;
}


