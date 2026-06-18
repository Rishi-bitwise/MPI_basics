#include<stdio.h>
#include<mpi.h>
#include<stddef.h>
#include<stdlib.h>
#include<math.h>

#define TOLERANCE 0.0001

typedef struct 
{
	float x;
	float y;
	float z;
	float w;
}Transform_vector;

float xfunc(Transform_vector tv)
{
    float y = tv.y, z = tv.z, w = tv.w;
    return (6 + y - 2*z) / 10;
}

float yfunc(Transform_vector tv)
{
    float x = tv.x, z = tv.z, w = tv.w;
    return (25 + x + z - 3*w) / 11;
}

float zfunc(Transform_vector tv)
{
    float x = tv.x, y = tv.y, w = tv.w;
    return (-11 - 2*x + y + w) / 10;
}

float wfunc(Transform_vector tv)
{
    float x = tv.x, y = tv.y, z = tv.z;
    return (15 - 3*y + z) / 8;
}

int main(int argc, char**argv)
{
	
	int size, rank;

	Transform_vector tv;
	tv.x = 1;
	tv.y = 1;
	tv.z = 1;
	tv.w = 1;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// int blocklengths[4] = {1,1,1,1};
	// MPI_Datatype types = {MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT};
	// MPI_Aint offsets[4];
	// offsets[0] = offsetof(Transform_vector, xval);
	// offsets[1] = offsetof(Transform_vector, yval);
	// offsets[2] = offsetof(Transform_vector, zval);
	// offsets[3] = offsetof(Transform_vector, wval);

	// MPI_Datatype mpi_struct_type;

	// MPI_Type_create_struct(4, blocklengths, offsets, types, &mpi_struct_type);
	// MPI_Commit(&mpi_struct_type);

	float error=67, newerror;

	float localnew;
	float recvbuf[4];

	while (error>TOLERANCE)
	{	

		if (rank==0)
		{
			localnew = xfunc(tv);
			error = fabs(localnew-tv.x);
		}

		if (rank==1)
		{
			localnew = yfunc(tv);
			error = fabs(localnew-tv.y);
		}

		if (rank==2)
		{
			localnew = zfunc(tv);
			error = fabs(localnew-tv.z);
		}

		if (rank==3)
		{
			localnew = wfunc(tv);
			error = fabs(localnew-tv.w);
		}

		 if (rank==0)
		 {
		 	printf("Process %d says error is %f, localnew = %f \n", rank,error, localnew);
		 	fflush(stdout);
		 }

		MPI_Allreduce(&error, &newerror, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
		error = newerror;

		printf("Process %d: localnew is %f, error is %f\n", rank, localnew, error);

		MPI_Allgather(&localnew, 1, MPI_FLOAT, recvbuf, 1, MPI_FLOAT, MPI_COMM_WORLD);
		tv.x = recvbuf[0];
		tv.y = recvbuf[1];
		tv.z = recvbuf[2];
		tv.w = recvbuf[3];
	}

	MPI_Finalize();
	printf("---------\n\n");
	printf("Process %d: %f, %f, %f, %f\n",rank, tv.x, tv.y,tv.z,tv.w);


	return 0;
}