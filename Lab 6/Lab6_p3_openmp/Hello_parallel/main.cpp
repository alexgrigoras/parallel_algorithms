/*
 ============================================================================
 Name        : Lab6_p3_openmp.c
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : Calculating Prefixes (Scan) in OpenMP
 ============================================================================
 */
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

	//int A[16] = { 0,0,0,0,0,0,0,0,4,8,5,2,10,6,9,11 };
	int A[16] = { 0,0,0,0,0,0,0,0,3,2,1,6,5,4,2,9 };
	int B[16];
	int n = sizeof(A) / sizeof(int);
	int m = 3;
	int k, j;
	int i;
	int tid;

	for (i = 0; i < 16; i++)
	{
		B[i] = 0;
	}

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
			printf("thread %d\n", tid);
		}
	}

	for (i = 0; i < n; i++)
	{
		printf("A[%d]= %d\n", i, A[i]);
	}

	printf("\n");

	B[1] = A[1];

	for (k = 1; k <= m; k++)
	{
		int doi_la_k = 1 << k;
		int doi_la_k_plus_unu = 1 << (k + 1);
#pragma omp parallel for private(j) shared(k, A, B, doi_la_k, doi_la_k_plus_unu)
		for (j = doi_la_k; j <= doi_la_k_plus_unu - 1; ++j)
		{
			if (j & 1 == 1)
			{
				B[j] = B[j / 2];
			}
			else 
			{
				B[j] = B[j / 2] - A[j + 1];
			}
		}
	}

	for (i = 0; i < n; i++)
	{
		printf("B[%d]= %d\n", i, B[i]);
	}

	return 0;
}
