/*
 ============================================================================
 Name        : Lab5_p2_mpi.c
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : All-to-one Diffusion on trees
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define ROOT 4

void ConcatMessage(char *s, char *d) {
	strcat(s, " ");
	strcat(s, d);
}

int main(int argc, char* argv[]) {
	/* variabile */
	int my_rank;					/* rank of process */
	int count;						/* number of processes */
	int tag = 0;					/* tag for messages */
	MPI_Status status;				/* return status for receive */
	char message[1000] = "";		/* message to be sent from root*/
	char tempMessage[1000] = "";	/* temporary message buffer */

	MPI_Comm tree;
	int nnodes = 9;
	int ncount;
	int index[] = {1, 2, 5, 7, 10, 11, 14, 15, 16};
	int edges[] = {4, 6, 8, 7, 4, 4, 6, 2, 3, 0, 6, 1, 3, 5, 2, 2};
	int neighbors[9];
	int i;
	int sumaVecini = 0;

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &count);
	
	MPI_Graph_create(MPI_COMM_WORLD, nnodes, index, edges, 0, &tree);

	MPI_Graph_neighbors_count(tree, my_rank, &ncount);

	MPI_Graph_neighbors(tree, my_rank, ncount, neighbors);
	
	//printf("Pornire: %d cu nr vecini: %d\n", my_rank, ncount);

	//ConcatMessage(message, tempMessage);
	//printf(message);
	
	// daca este radacina
	if (my_rank == ROOT)
	{
		//printf("---- radacina\n");
		
		for (i = 0; i < ncount; ++i)
		{
			MPI_Recv(tempMessage, 1000, MPI_CHAR, neighbors[i], tag, tree, &status);
			ConcatMessage(message, tempMessage);
		}
		
		printf("Am primit: %s\n", message);
	}
	else
	{
		// daca este frunza
		if (ncount == 1) 
		{
			//printf("---- frunza\n");
			sprintf(message, "mesaj(%d)", my_rank);
			printf("frunza %d a trimis %s catre %d\n", my_rank, message, neighbors[0]);
			MPI_Send(message, strlen(message) + 1, MPI_CHAR, neighbors[0], tag, tree);
		}
		// daca este nod interior
		else if (ncount > 1)
		{
			//printf("---- interior\n");
			for (i = 0; i < ncount; ++i)
			{
				sumaVecini += neighbors[i];
			}
			//printf("sumaVecini = %d\n", sumaVecini);
			
			for (i = 0; i < ncount - 1; ++i)
			{
				MPI_Recv(tempMessage, 1000, MPI_CHAR, MPI_ANY_SOURCE, tag, tree, &status);
				ConcatMessage(message, tempMessage);
				sumaVecini -= status.MPI_SOURCE;
			}

			printf("interiorul %d a primit %s\n", my_rank, message);
			
			//printf("sumaVecini = %d\n", sumaVecini);
			printf("interiorul %d a trimis %s catre %d\n", my_rank, message, sumaVecini);
			MPI_Send(message, strlen(message) + 1, MPI_CHAR, sumaVecini, tag, tree);
		}
	}
	
	
	MPI_Comm_free(&tree);
	
	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
