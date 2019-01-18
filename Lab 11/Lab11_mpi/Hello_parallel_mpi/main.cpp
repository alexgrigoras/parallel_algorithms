/*
 ================================================================================
 Name        : Lab11_mpi.cpp
 Author      : Alexandru Grigoras
 Version     : 0.1
 Copyright   : Alexandru Grigoras
 Description : Choosing the leader (1): LCR algorithm (LeLann, Chang and Roberts)
 ================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define NP 8								// numarul de procese
#define ROOT 0								// radacina

 // tip runda
#define R_ALEGERE 1
#define R_LIDER 2

// status proces
#define S_LIDER 1
#define S_NONLIDER 0

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
	int my_rank;							// rank of process 
	int count;								// number of processes  
	MPI_Status status;						// return status for receive 
	int i, j, k;

	int nrDimensiuni = 2;					// plasa de procesoare are 1 dimensiune
	int nrElemDim = NP;						// numarul de elemente pe fiecare dimensiune in parte = NP
	int dims[NP] = {1, NP};					// elementele din dimensiune
	int periods[] = { 1, 1 };				// periodicitatea este pe ambele dimensiuni
	int gasitLider = false;
	int statut = S_LIDER;					// initial, fiecare proces considera ca e lider (ca e cel mai bun)
	int ID;									// ID-ul fiecarui proces
	int mesaj;								// mesajul primit / trimis de procese
	int stanga;								// vecinii procesului curent
	int dreapta;
	int runda = R_ALEGERE;					// tag for messages
	int vectorID_uri[NP] = { 1, 8, 2, 6, 4, 7, 3, 5 };

	MPI_Comm commCart;						// construim o topologie de tip cartezian

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &count);

	MPI_Cart_create(MPI_COMM_WORLD, nrDimensiuni, dims, periods, 1, &commCart);

	// aflam rank-ul procesului curent
	MPI_Comm_rank(commCart, &my_rank);

	// aflam numarul de procese
	MPI_Comm_size(commCart, &count);

	MPI_Scatter(vectorID_uri, 1, MPI_INT, &ID, 1, MPI_INT, ROOT, commCart);
	
	if (my_rank == NP-1) {
		dreapta = 0;
	}
	else {
		dreapta = my_rank + 1;
	}
	if (my_rank == 0) {
		stanga = NP - 1;
	}
	else {
		stanga = my_rank - 1;
	}
	
	MPI_Send(&ID, 1, MPI_INT, dreapta, runda, commCart);

	//printf("[Proces %d] Am trimis ID-ul \"%d\" catre vecinul din dreapta %d.\n", my_rank, ID, dreapta);
	//printf("%d trimite catre %d ; ", ID, dreapta);	
	
	while (!gasitLider)
	{		
		MPI_Recv(&mesaj, 1, MPI_INT, stanga, MPI_ANY_TAG, commCart, &status);

		switch (status.MPI_TAG) 
		{
			case R_ALEGERE:
				if (mesaj == ID)
				{
					statut = S_LIDER;
					runda = R_LIDER;
					MPI_Send(&ID, 1, MPI_INT, dreapta, runda, commCart);
					gasitLider = true;
				}
				else if (mesaj > ID)
				{
					MPI_Send(&mesaj, 1, MPI_INT, dreapta, runda, commCart);
				}
				break;
			case R_LIDER:
				statut = S_NONLIDER;
				runda = R_LIDER;
				MPI_Send(&mesaj, 1, MPI_INT, dreapta, runda, commCart);
				gasitLider = true;
				break;
		}
	}
	
	if (statut == S_LIDER)
	{
		printf("Procesul %d cu ID-ul %d este lider!\n", my_rank, ID);
	}
	else
	{
		printf("Procesul %d cu ID-ul %d este nonlider.\n", my_rank, ID);
	}

	MPI_Comm_free(&commCart);

	/* shut down MPI */
	MPI_Finalize();

	return 0;
}
