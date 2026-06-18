#include<stdio.h>
#include<mpi.h>


int main(int argc, char**argv)
{
	MPI_Status status;
	int rank, size, send_data;
	int temp;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	send_data= rank*10;
	for (int offset=size/2; offset>0; offset= offset/2)
	{	
		if (rank<2*offset)
		{
			if (rank>=offset)
			{
			MPI_Send(&send_data, 1, MPI_INT, rank-offset, 1, MPI_COMM_WORLD);
			printf("sending %d from process %d to process %d\n",send_data, rank, rank-offset );
			}
			else
			{
				
				MPI_Recv(&temp, 1, MPI_INT, rank+offset, 1, MPI_COMM_WORLD, &status);
				printf("current process is %d received data %d from %d \n",rank,temp, rank+offset);
				send_data += temp;
			}
		}
	}

	if (rank==0)
	{
		printf("The final reduced value is %d\n", send_data);
	}
	MPI_Finalize();
}


