/*
 ============================================================================
 Name        : Lab4_p1_mpi.c
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : One-to-all Diffusion on HyperCube - root node 0
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char* argv[]) {
	/* variabile */
	int my_rank;			/* rank of process */
	int count;				/* number of processes */
	int tag = 0;			/* tag for messages */
	MPI_Status status;		/* return status for receive */
	char message[100];		/* message to be sent from root*/
	int dim;				/* dimensiune hipercub*/
	int masca;				/* masca */
	int k;					/* iterator */
	int dest, sursa;		/* destinatie / sursa*/

	/* asignare valori variabile*/
	dim = 3;
	masca = (1 << dim) - 1;

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &count);
	
	if(my_rank == 0)
	{
		strcpy(message, "Hello World from root");
	}

	for (k = dim - 1; k >= 0; k--)
	{
		masca = masca ^ (1 << k);
		if ((my_rank & masca) == 0)
		{
			if ((my_rank & (1 << k)) == 0)
			{
				dest = my_rank ^ (1 << k);
				MPI_Send(message, strlen(message) + 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
				//printf("%d trimite catre %d mesajul %s\n", my_rank, dest, message);
			}
			else {
				sursa = my_rank ^ (1 << k);
				MPI_Recv(message, strlen(message) + 1, MPI_CHAR, sursa, tag, MPI_COMM_WORLD, &status);
				printf("%d primeste de la %d mesajul %s\n", my_rank, sursa, message);
			}
		}
	}

	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
