/*
 ============================================================================
 Name        : Lab6_p3_mpi.c
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : Calculating Prefixes (Scan) in MPI
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define ROOT 1

void afisareTablou(int *v, int dim)
{
	int i;
	printf("[");
	for (i = 0; i < dim - 1; ++i)
	{
		printf("%d | ", v[i]);
	}
	printf("%d]\n", v[dim - 1]);
}

int main(int argc, char* argv[]) {
	int my_rank;			/* rank of process */
	int p;					/* number of processes */
	int tag = 0;			/* tag for messages */
	MPI_Status status;		/* return status for receive */
	int lungime = 16;
	//int A[16] = { 0,0,0,0,0,0,0,0,4,8,5,2,10,6,9,11 };
	int A[16] = { 0,0,0,0,0,0,0,0,3,2,1,6,5,4,2,9 };
	int *B;

	B = new int(16);

	for (int i = 0; i < 16; i++)
	{
		B[i] = 0;
	}

	/* start up MPI */

	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	// commprimare

	if (my_rank != 0)
	{
		if (my_rank >= lungime / 2)
		{
			MPI_Send(A + my_rank, 1, MPI_INT, my_rank / 2, tag, MPI_COMM_WORLD);
			//printf("%d is sending %d to %d", my_rank, A[my_rank], my_rank / 2);
		}
		else
		{
			int fiu_stang, fiu_drept = 0;
			MPI_Recv(&fiu_stang, 1, MPI_INT, my_rank * 2, tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&fiu_drept, 1, MPI_INT, my_rank * 2 + 1, tag, MPI_COMM_WORLD, &status);
			A[my_rank] = fiu_stang + fiu_drept;

			// daca nu e radacina
			if (my_rank != ROOT)
			{
				MPI_Send(A + my_rank, 1, MPI_INT, my_rank / 2, tag, MPI_COMM_WORLD);
			}
			//printf("A[%d]: %d\n", my_rank, A[my_rank]);
		}
	}

	MPI_Gather(A+my_rank, 1, MPI_INT, A, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

	if (my_rank == ROOT) // procesul ROOT face afisarea
	{
		printf("\nTabloul, dupa comprimare:\n");
		afisareTablou(A, lungime);
		printf("\n");
	}

	// calcul prefixe
	if (my_rank == ROOT)
	{
		B[1] = A[1];
		MPI_Send(B + my_rank, 1, MPI_INT, 2 * my_rank, tag, MPI_COMM_WORLD);
		MPI_Send(B + my_rank, 1, MPI_INT, 2 * my_rank + 1, tag, MPI_COMM_WORLD);
		printf("%d is sending %d to %d and %d", my_rank, B[my_rank], 2 * my_rank, 2 * my_rank + 1);
	}
	else if (my_rank != 0)
	{
		MPI_Recv(B + my_rank, 1, MPI_INT, my_rank / 2, tag, MPI_COMM_WORLD, &status);
		printf("%d is receiving %d from %d", my_rank, B[my_rank], my_rank / 2);

		if (my_rank & 1 == 1)
		{
			MPI_Send(A + my_rank, 1, MPI_INT, my_rank - 1, tag, MPI_COMM_WORLD);
			printf("%d is sending %d to %d", my_rank, B[my_rank], my_rank - 1);
		}
		else
		{
			int receivedValue = 0;
			MPI_Recv(&receivedValue, 1, MPI_INT, my_rank + 1, tag, MPI_COMM_WORLD, &status);
			B[my_rank] -= receivedValue;
			printf("%d is receiving %d from %d", my_rank, B[my_rank], my_rank + 1);
		}

		if (my_rank < lungime / 2)
		{
			MPI_Send(B + my_rank, 1, MPI_INT, 2 * my_rank, tag, MPI_COMM_WORLD);
			MPI_Send(B + my_rank, 1, MPI_INT, 2 * my_rank + 1, tag, MPI_COMM_WORLD);
			printf("%d is sending %d to %d and %d", my_rank, B[my_rank], 2 * my_rank, my_rank + 1);
		}
	}

	MPI_Gather(B + my_rank, 1, MPI_INT, B, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

	if (my_rank == ROOT) // procesul ROOT face afisarea
	{
		printf("\nTabloul, dupa comprimare:\n");
		afisareTablou(B, lungime);
		printf("\n");
	}

	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
