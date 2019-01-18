/*
 ============================================================================
 Name        : Lab12_mpi.cpp
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : Choosing the leader (2): the synchronous FloodMax algorithm
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define NP 9								// numarul de procese
#define ROOT 0								// radacina
#define DIAMETRU 3							// diametrul grafului

int main(int argc, char* argv[]) {
	int my_rank;							// rank of process 
	int count;								// number of processes  
	MPI_Status status;						// return status for receive 
	int tag = 0;
	int i, j, k;
	int index[] = { 4, 6, 9, 12, 15, 18, 21, 25, 28 };
	int edges[] = {
		1, 4, 5, 6,
		0, 6,
		4, 7, 8,
		5, 6, 8,
		0, 2, 7,
		0, 3, 7,
		0, 1, 3,
		2, 4, 5, 8,
		2, 3, 7
	};
	int nrNeighbors;
	int *neighbors;
	int runda;
	int maxRank;
	int receivedRank;
	MPI_Comm graphCart;						// construim o topologie de tip graf

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &count);

	MPI_Graph_create(MPI_COMM_WORLD, count, index, edges, 0, &graphCart);

	MPI_Graph_neighbors_count(graphCart, my_rank, &nrNeighbors);

	neighbors = (int*)malloc(nrNeighbors * sizeof(int));

	MPI_Graph_neighbors(graphCart, my_rank, nrNeighbors, neighbors);

	maxRank = my_rank;

	for (runda = 0; runda < DIAMETRU; ++runda)
	{
		for (i = 0; i < nrNeighbors; i++)
		{
			MPI_Send(&maxRank, 1, MPI_INT, neighbors[i], tag, graphCart);
		}
		for (i = 0; i < nrNeighbors; i++) 
		{
			MPI_Recv(&receivedRank, 1, MPI_INT, neighbors[i], tag, graphCart, &status);
			if (receivedRank > maxRank) 
			{
				maxRank = receivedRank;
			}
		}
	}
	
	if (maxRank == my_rank) 
	{
		printf("%d - eu sunt lider", my_rank);
	}
	else 
	{
		printf("%d - subordonat", my_rank);
	}

	MPI_Comm_free(&graphCart);

	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
