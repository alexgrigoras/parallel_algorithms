/*
 ============================================================================
 Name        : Lab4_p1_mpi.c
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : One-to-all Diffusion on HyperCube - root any node
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
	char message[100];		/* message to be sent from root */
	int dim;				/* dimensiune hipercub */
	int masca;				/* masca */
	int k;					/* iterator */
	int dest, sursa;		/* destinatie / sursa */
	int id_virt;			/* id virtual */
	int s;					/* radacina reala a comunicatiei */

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &count);

	/* asignare valori variabile*/
	dim = 3;
	s = 0;
	id_virt = my_rank ^ s;
	masca = (1 << dim) - 1;
	
	if(my_rank == s)
	{
		strcpy(message, "Hello World from root");
	}

	for (k = dim - 1; k >= 0; k--)
	{
		masca = masca ^ (1 << k);
		if ((id_virt & masca) == 0)
		{
			if ((id_virt & (1 << k)) == 0)
			{
				dest = id_virt ^ (1 << k);
				MPI_Send(message, strlen(message) + 1, MPI_CHAR, dest ^ s, tag, MPI_COMM_WORLD);
				//printf("%d trimite catre %d mesajul %s\n", my_rank, dest, message);
			}
			else {
				sursa = id_virt ^ (1 << k);
				MPI_Recv(message, strlen(message) + 1, MPI_CHAR, sursa ^ s, tag, MPI_COMM_WORLD, &status);
				printf("%d primeste de la %d mesajul %s\n", my_rank, sursa, message);
			}
		}
	}

	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
