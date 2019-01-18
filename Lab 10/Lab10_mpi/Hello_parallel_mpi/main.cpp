/*
 ============================================================================
 Name        : Lab10_mpi.cpp
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : Solving linear systems equations
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

#define NP 3
#define ROOT 0

void AfisareVector(double v[], int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		printf("%lf ", v[i]);
	}
}

void AfisareMatrice(double v[][NP], int n)
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			printf("%lf ", v[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char* argv[]) {
	/* variabile */
	int rank;				// rank of process
	int count;				// number of processes
	int tag = 0;			// tag for messages
	MPI_Status status;		// return status for receive
	int k, j;
	double A[NP][NP] = { 
		{1, 3, 4}, 
		{6, 2, 8}, 
		{5, 7, 9} 
	};
	double B[NP] = {9, 3, 14};
	double *a, b = 0;
	double aBuffer[NP] = { 0, 0, 0 };
	double bBuffer = 0;

	MPI_Init(&argc, &argv);						// start up MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);		// find out process rank
	MPI_Comm_size(MPI_COMM_WORLD, &count);		// find out number of processes

	/*
	if (rank == ROOT) 
	{
		AfisareMatrice(A, NP);
		AfisareVector(B, NP);
	}
	*/

	// etapa de transformare in diagonala superioara

	a = A[rank];
	b = B[rank];
	
	for (int k = 0; k < NP; k++)
	{
		if (rank == k)
		{
			for (j = k + 1; j < NP; j++)
			{
				a[j] = a[j] / a[k];
			}
			b = b / a[k];
			a[k] = 1;
			for (j = 0; j < NP; j++)
			{
				aBuffer[j] = a[j];
			}
			bBuffer = b;
		}

		MPI_Bcast(aBuffer, NP, MPI_DOUBLE, k, MPI_COMM_WORLD);
		MPI_Bcast(&bBuffer, 1, MPI_DOUBLE, k, MPI_COMM_WORLD);

		if (rank > k) 
		{
			for (j = k + 1; j < NP; j++)
			{
				a[j] = a[j] - a[k] * aBuffer[j];
			}
			b = b - a[k] * bBuffer;
			a[k] = 0;
		}
	}

	printf("%d -> ", rank);
	AfisareVector(a, NP);
	printf("= %lf", b);

	// etapa de calculare a valorilor
	// nu ar trebui sa dea
	
	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
