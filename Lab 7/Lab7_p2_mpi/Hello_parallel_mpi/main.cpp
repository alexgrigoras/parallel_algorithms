/*
 ============================================================================
 Name        : lab7_p2_mpi.c
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : Odd-Even sorting algorithm
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define N 8
#define ROOT 0

void afisareVector(int *v, int n)
{
	int i;
	printf("[ ");
	for (i = 0; i < n - 1; i++)
	{
		printf("%d | ", v[i]);
	}
	printf("%d ]\n", v[n - 1]);
}

int main(int argc, char **argv)
{
	int count;
	int my_rank;
	int tag = 99;
	MPI_Status status;
	int faza;
	int *A;
	int dest, sursa;
	int valPrimita, valTrimisa;

	A = (int*)malloc(N * sizeof(int));
	if (!A)
	{
		fprintf(stderr, "Eroare la alocarea memoriei!\n");
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &count);

	if (my_rank == ROOT)
	{
		A[0] = 3; A[1] = 2; A[2] = 3; A[3] = 8; A[4] = 5; A[5] = 6; A[6] = 4; A[7] = 1;

		afisareVector(A, N);
	}

	MPI_Scatter(A, 1, MPI_INT, &valTrimisa, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

	// parcurgere toate fazele corespunzatoare cu nr de elem din vector
	for (faza = 1; faza <= N; faza++)
	{
		// daca faza este impara
		if (faza % 2 == 1 && my_rank >= 0 && my_rank <= 2 * (N / 2) - 1)
		{
			// daca rank-ul este par
			if (my_rank % 2 == 0)
			{
				dest = my_rank + 1;
				MPI_Send(&valTrimisa, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
				sursa = dest;
				MPI_Recv(&valPrimita, 1, MPI_INT, sursa, tag, MPI_COMM_WORLD, &status);
				if (valPrimita < valTrimisa)
				{
					valTrimisa = valPrimita;
				}
			}
			// daca rank-ul este impar
			else
			{
				sursa = my_rank - 1;
				MPI_Recv(&valPrimita, 1, MPI_INT, sursa, tag, MPI_COMM_WORLD, &status);
				dest = sursa;
				MPI_Send(&valTrimisa, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
				if (valPrimita > valTrimisa)
				{
					valTrimisa = valPrimita;
				}
			}
		}
		// daca faza este para
		else if (faza % 2 == 0 && my_rank >= 1 && my_rank <= 2 * ((N - 1) / 2))
		{
			// daca rank-ul este impar
			if (my_rank % 2 == 1)
			{
				dest = my_rank + 1;
				MPI_Send(&valTrimisa, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
				sursa = dest;
				MPI_Recv(&valPrimita, 1, MPI_INT, sursa, tag, MPI_COMM_WORLD, &status);
				if (valPrimita < valTrimisa)
				{
					valTrimisa = valPrimita;
				}
			}
			// daca rank-ul este par
			else
			{
				sursa = my_rank - 1;
				MPI_Recv(&valPrimita, 1, MPI_INT, sursa, tag, MPI_COMM_WORLD, &status);
				dest = sursa;
				MPI_Send(&valTrimisa, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
				if (valPrimita > valTrimisa)
				{
					valTrimisa = valPrimita;
				}
			}
		}
	}

	MPI_Gather(&valTrimisa, 1, MPI_INT, A, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
	
	if (my_rank == ROOT) 
	{
		afisareVector(A, N);

		if (A)
		{
			free(A);
		}
	}

	MPI_Finalize();

	return 0;
}