#include<stdio.h>
#include<mpi.h>
#include<math.h>

int main(int argc, char**argv)
{
	int rank, size;
	MPI_Status status;
	int send_data, temp;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	send_data=  (int)(rank*pow(10,rank));

	for(int offset=size/2;offset>0; offset/=2)
	{
		if (rank>=offset*2)
			break;

		if (rank>=offset)
		{
			MPI_Send(&send_data, 1, MPI_INT, rank-offset, 1, MPI_COMM_WORLD);
			printf("Process %d: Sending data = %d to Process:%d, off = %d\n",rank, send_data, rank-offset, offset);

		}
		else
		{

			MPI_Recv(&temp, 1, MPI_INT, rank+offset, 1, MPI_COMM_WORLD, &status);
			printf("Process %d: Received data = %d from Process:%d, off = %d\n", rank, temp, rank+offset, offset);
			send_data += temp;
		}
	}

	if (rank==0)
	printf("Total accumulated data is %d\n\n", send_data);

	MPI_Barrier(MPI_COMM_WORLD);

	for(int offset=1; offset<=size/2;offset*=2)
	{
		if (rank>=2*offset)
			continue;
		if (rank<offset)
		{
			MPI_Send(&send_data,1, MPI_INT, rank+offset, 1, MPI_COMM_WORLD);
			printf("--REV--Process %d: Sending data = %d to Process:%d off =%d\n",rank, send_data, rank+offset, offset);
		}
		else if(rank>=offset)
		{
			MPI_Recv(&send_data, 1, MPI_INT, rank-offset, 1, MPI_COMM_WORLD, &status);
			printf("--REV--Process %d: Received data = %d from Process:%d, off = %d\n", rank, send_data, rank-offset, offset);
		}
	}

	printf("--REV--Process %d: Accumulated BCAST data is %d\n\n", rank, send_data);
	MPI_Finalize();
	return 0;
}