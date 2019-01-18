/*
 ============================================================================
 Name        : lab6_p2_mpi.c
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : Compression (Reduction) in MPI
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define ROOT 1

int main(int argc, char* argv[]) {
	int my_rank;			/* rank of process */
	int p;					/* number of processes */
	int tag = 0;			/* tag for messages */
	MPI_Status status;		/* return status for receive */
	int lungime = 16;
	int A[16] = { 0,0,0,0,0,0,0,0,4,8,5,2,10,6,9,11 };

	/* start up MPI */

	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

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
			if (my_rank != ROOT)
			{
				MPI_Send(A + my_rank, 1, MPI_INT, my_rank / 2, tag, MPI_COMM_WORLD);
			}
			printf("A[%d]: %d\n", my_rank, A[my_rank]);
		}
	}

	/* shut down MPI */
	MPI_Finalize();


	return 0;
}
