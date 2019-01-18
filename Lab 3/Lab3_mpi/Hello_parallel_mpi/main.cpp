/*
 ============================================================================
 Name        : Lab3_mpi.c
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : Hello MPI World in C
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define ROOT 0

int main(int argc, char* argv[]) {
	/* variabile */
	int my_rank;			/* rank of process */
	int count;				/* number of processes */

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &count);
	
	if (my_rank == ROOT)
	{
		printf("I (%d) am root", my_rank);
	}
	else 
	{
		printf("I (%d) am NOT root, just a process", my_rank);
	}

	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
