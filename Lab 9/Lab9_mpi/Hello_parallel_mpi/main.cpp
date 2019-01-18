/*
 ============================================================================
 Name        : Lab9_mpi.cpp
 Author      : Alexnadru Grigoras
 Version     : 0.1
 Copyright   : Alexnadru Grigoras
 Description : Matrix multiplication
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define NP 16				// numarul de procese
#define ROOT 0				// procesul radacina

void AfisareMatrice(int **a, int n, int m)
{
	int i, j;

	for (i = 0; i < n; ++i)
	{
		for (j = 0; j < m; ++j)
		{
			printf("%2d ", a[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void AfisareVector(int v[], int n)
{
	int i;

	for (i = 0; i < n; i++)
	{
		printf("%d ", v[i]);
	}

	printf("\n");
}

int main(int argc, char* argv[]) {
	/* variabile */
	int rank;				// rank of process
	int count;				// number of processes
	int tag = 0;			// tag for messages
	MPI_Status status;		// return status for receive
	MPI_Comm cartezianComm;	// topologie de tip cartezian

	int dims[2];		
	int period[2];
	int coordonate[2];
	int reorder;
	int i, j;				// coordonatele procesului curent in topologia de tip cartezian
	int **A, **B, **C;
	int *mem_A, *mem_B, *mem_C;
	int M1, M2, M3;			// cele 3 partitii pe care lucreaza fiecare proces
	int nrElemDim = (int)sqrt(NP);
	int vecin_sus;			// rank-urile proceselor vecine
	int vecin_jos;
	int vecin_stanga;
	int vecin_dreapta;
	int k;
	int rez;

	dims[0] = nrElemDim; dims[1] = nrElemDim;
	period[0] = 1; period[1] = 1;
	coordonate[0] = -1; coordonate[1] = -1;
	reorder = false;

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &count);

	if (count != NP) // daca numarul de procese nu este cel definit pentru matricea din program (4 x 4)
	{
		fprintf(stderr, "Numarul de procese trebuie sa fie egal cu %d!\n", NP);
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}

	// creare topologie carteziana
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, reorder, &cartezianComm);

	A = (int**)malloc(nrElemDim * sizeof(int));
	B = (int**)malloc(nrElemDim * sizeof(int));
	C = (int**)malloc(nrElemDim * sizeof(int));
		
	mem_A = (int*)malloc(nrElemDim * nrElemDim * sizeof(int));
	mem_B = (int*)malloc(nrElemDim * nrElemDim * sizeof(int));
	mem_C = (int*)malloc(nrElemDim * nrElemDim * sizeof(int));

	A[0] = mem_A;
	B[0] = mem_B;
	C[0] = mem_C;

	for (i = 1; i < nrElemDim; ++i)
	{
		A[i] = mem_A + i * nrElemDim;
		B[i] = mem_B + i * nrElemDim;
		C[i] = mem_C + i * nrElemDim;
	}
	
	if (rank == ROOT)
	{
		for (i = 0; i < nrElemDim; i++)
		{
			for (j = 0; j < nrElemDim; j++)
			{
				A[i][j] = i + j;

				if (i == j)
				{
					B[i][j] = 1;
				}
				else
				{
					B[i][j] = 0;
				}

				C[i][j] = 0;
			}
		}
		
		printf("Matricea A:\n");
		AfisareMatrice(A, nrElemDim, nrElemDim);
		printf("Matricea B:\n");
		AfisareMatrice(B, nrElemDim, nrElemDim);
	}

	MPI_Scatter(mem_A, 1, MPI_INT, &M1, 1, MPI_INT, ROOT, cartezianComm);
	MPI_Scatter(mem_B, 1, MPI_INT, &M2, 1, MPI_INT, ROOT, cartezianComm);

	// coordonatele procesului curent
	MPI_Cart_coords(cartezianComm, rank, 2, coordonate);
	i = coordonate[0];
	j = coordonate[1];

	// alinierea initiala -----------------------------

	// aflam vecinii pentru shiftarea pe linie
	// dimensiunea pe care se shifteaza este 1 = axa Ox (shiftare stanga-dreapta)
	// displacement (distanta) este 1 = aflam vecinii la distanta 1 fata de procesul curent
	MPI_Cart_shift(cartezianComm, 1, 1, &vecin_stanga, &vecin_dreapta);
	for (k = 1; k < i; k++)
	{
		MPI_Send(&M1, 1, MPI_INT, vecin_stanga, tag, cartezianComm);
		MPI_Recv(&M1, 1, MPI_INT, vecin_dreapta, tag, cartezianComm, &status);
	}

	// aflam vecinii pentru shiftarea pe coloana
	// dimensiunea pe care se shifteaza este 0 = axa Oy (shiftare sus-jos)
	// displacement (distanta) este 1 = aflam vecinii la distanta 1 fata de procesul curent
	// rezulta, dupa apel, vecinul de sus (destinatia) si vecinul de jos (sursa)
	MPI_Cart_shift(cartezianComm, 0, 1, &vecin_sus, &vecin_jos);
	for (k = 1; k < j; k++)
	{
		MPI_Send(&M2, 1, MPI_INT, vecin_sus, tag, cartezianComm);
		MPI_Recv(&M2, 1, MPI_INT, vecin_jos, tag, cartezianComm, &status);
	}

	// algoritmul lui canon ------------------------------------
	
	M3 = 0;
	for (k = 0; k < nrElemDim - 1; k++)
	{
		M3 += M1 * M2;
		MPI_Send(&M1, 1, MPI_INT, vecin_stanga, tag, cartezianComm);
		MPI_Recv(&M1, 1, MPI_INT, vecin_dreapta, tag, cartezianComm, &status);
		MPI_Send(&M2, 1, MPI_INT, vecin_sus, tag, cartezianComm);
		MPI_Recv(&M2, 1, MPI_INT, vecin_jos, tag, cartezianComm, &status);
	}
	rez = M3 + M1 * M2;

	MPI_Gather(&rez, 1, MPI_INT, mem_C, 1, MPI_INT, ROOT, cartezianComm);
	
	// afisare finala

	if (rank == ROOT) // procesul ROOT face afisarea si dealocarea
	{
		printf("\nMatricea rezultat C:\n");
		AfisareMatrice(C, nrElemDim, nrElemDim);
		printf("\n");

		if (mem_A)
		{
			free(A[0]);
			free(A);
		}
		if (mem_B)
		{
			free(B[0]);
			free(B);
		}
		if (mem_C)
		{
			free(C[0]);
			free(C);
		}
	}

	MPI_Comm_free(&cartezianComm);

	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
