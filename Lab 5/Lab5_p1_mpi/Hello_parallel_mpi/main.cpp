/*
 ============================================================================
 Name        : Lab5_p1_mpi.c
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : One-to-all Diffusion on trees
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define ROOT 4

int main(int argc, char* argv[]) {
	/* variabile */
	int my_rank;			/* rank of process */
	int count;				/* number of processes */
	int tag = 0;			/* tag for messages */
	MPI_Status status;		/* return status for receive */
	char message[100];		/* message to be sent from root*/

	MPI_Comm tree;
	int nnodes = 9;
	int ncount;
	int index[] = {1, 2, 5, 7, 10, 11, 14, 15, 16};
	int edges[] = {4, 6, 8, 7, 4, 4, 6, 2, 3, 0, 6, 1, 3, 5, 2, 2};
	int neighbors[9];
	int i;

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &count);
	
	MPI_Graph_create(MPI_COMM_WORLD, nnodes, index, edges, 1, &tree);

	MPI_Graph_neighbors_count(tree, my_rank, &ncount);

	MPI_Graph_neighbors(tree, my_rank, ncount, neighbors);

	if (my_rank == ROOT)
	{
		strcpy(message, "Hello world");
		for (i = 0; i < ncount; ++i)
		{
			MPI_Send(message, strlen(message) + 1, MPI_CHAR, neighbors[i], tag, tree);
		}
	}
	else
	{
		MPI_Recv(message, strlen(message) + 1, MPI_CHAR, MPI_ANY_SOURCE, tag, tree, &status);
		int parent = status.MPI_SOURCE;
		printf("%d is receiving message: %s from node: %d\n", my_rank, message, parent);
		for (i = 0; i < ncount; ++i) 
		{
			if (neighbors[i] != parent)
			{
				MPI_Send(message, strlen(message) + 1, MPI_CHAR, neighbors[i], tag, tree);
			}
		}
	}

	MPI_Comm_free(&tree);
	
	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
