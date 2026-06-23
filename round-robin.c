#include<stdio.h>
#include<mpi.h>
#include<string.h>

int main(int argc, char**argv)
{
	int size, rank;
	char sendbuf[50], recvbuf[50];
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank!=0)
	{
		sprintf(sendbuf, "Hi this is a message from proc:%d",rank);
		MPI_Send(sendbuf, 50, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
	}

	else{
		for (int i=1;i<size;i++)
		{
			MPI_Recv(recvbuf, 50 ,	MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);
			printf("THis is the message received by process 0: %s\n", recvbuf);
		}
		printf("His this is process 0 signing off");
	}

	MPI_Finalize();
	return 0;
}
