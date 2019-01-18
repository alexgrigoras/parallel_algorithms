/*
 ============================================================================
 Name        : Lab8_mpi.cpp
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : Bitonic sort on hypercube
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

#define NP 16
#define NF 4
#define ROOT 0

void afisareVector(int *v, int n)
{
	int i;
	printf("[ ");
	for (i = 0; i < n-1; i++)
	{
		printf("%d | ", v[i]);
	}
	printf("%d ]\n", v[n - 1]);
}

int main(int argc, char* argv[]) {
	/* variabile */
	int my_rank;			// rank of process 
	int count;				// number of processes 
	int tag = 0;			// tag for messages 
	MPI_Status status;		// return status for receive 
	// int vector[] = { 10, 20, 5, 9, 3, 8, 12, 14, 90, 0, 60, 40, 23, 35, 95, 18 };
	int *vector;
	int mesaj;				// message to be sent
	int mesajPrimit;		// message received
	int faza;				// faza curenta
	int k;					// etapele fazei
	int masca, masca2;		// masca pt desinatar
	int destinatie;			// destinatarul trimiterii

	vector = (int*)malloc(NP * sizeof(int));
	for (k = NP - 1; k >= 0; k--)
	{
		vector[k] = 15-k;
	}

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &count);
	
	if (my_rank == ROOT)
	{
		printf("Vectorul initial: ");
		afisareVector(vector, NP);
	}

	mesaj = vector[my_rank];

	// transformarea in secventa bitona
	for (faza = 0; faza < NF-1; ++faza)
	{
		masca2 = (1 << (faza + 1));
		for (k = faza; k >= 0; --k)
		{
			masca = (1 << k);
			destinatie = (my_rank ^ masca);	

			// minimul este primul element
			if ((my_rank & masca2) == 0) 
			{
				// daca este primul
				if ((my_rank & masca) == 0)
				{					
					MPI_Send(&mesaj, 1, MPI_INT, destinatie, tag, MPI_COMM_WORLD);
					MPI_Recv(&mesajPrimit, 1, MPI_INT, destinatie, tag, MPI_COMM_WORLD, &status);
					if (mesajPrimit < mesaj)
					{
						mesaj = mesajPrimit;
					}
				}
				// daca este al doilea
				else 
				{
					MPI_Send(&mesaj, 1, MPI_INT, destinatie, tag, MPI_COMM_WORLD);
					MPI_Recv(&mesajPrimit, 1, MPI_INT, destinatie, tag, MPI_COMM_WORLD, &status);
					if (mesajPrimit > mesaj)
					{
						mesaj = mesajPrimit;
					}
				}
			}
			// maximul este primul element
			else 
			{
				// daca este primul
				if ((my_rank & masca) == 0)
				{
					MPI_Send(&mesaj, 1, MPI_INT, destinatie, tag, MPI_COMM_WORLD);
					MPI_Recv(&mesajPrimit, 1, MPI_INT, destinatie, tag, MPI_COMM_WORLD, &status);
					if (mesajPrimit > mesaj)
					{
						mesaj = mesajPrimit;
					}
				}
				// daca este al doilea
				else
				{
					MPI_Send(&mesaj, 1, MPI_INT, destinatie, tag, MPI_COMM_WORLD);
					MPI_Recv(&mesajPrimit, 1, MPI_INT, destinatie, tag, MPI_COMM_WORLD, &status);
					if (mesajPrimit < mesaj)
					{
						mesaj = mesajPrimit;
					}
				}
			}
		}
	}

	MPI_Gather(&mesaj, 1, MPI_INT, vector, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

	if (my_rank == ROOT)
	{
		printf("Vectorul bitonic: ");
		afisareVector(vector, NP);
	}
	
	// sortarea secventei bitone
	for (faza = NF - 1; faza >= 0; --faza)
	{
		masca = (1 << faza);
		destinatie = (my_rank ^ masca);
		
		// daca este primul
		if ((my_rank & masca) == 0)
		{
			MPI_Send(&mesaj, 1, MPI_INT, destinatie, tag, MPI_COMM_WORLD);
			MPI_Recv(&mesajPrimit, 1, MPI_INT, destinatie, tag, MPI_COMM_WORLD, &status);
			if (mesajPrimit < mesaj)
			{
				mesaj = mesajPrimit;
			}
		}
		// daca este al doilea
		else
		{
			MPI_Recv(&mesajPrimit, 1, MPI_INT, destinatie, tag, MPI_COMM_WORLD, &status);
			MPI_Send(&mesaj, 1, MPI_INT, destinatie, tag, MPI_COMM_WORLD);
			if (mesajPrimit > mesaj)
			{
				mesaj = mesajPrimit;
			}
		}
		
	}

	MPI_Gather(&mesaj, 1, MPI_INT, vector, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

	if (my_rank == ROOT)
	{
		printf("Vectorul sortat: ");
		afisareVector(vector, NP);
	}

	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
