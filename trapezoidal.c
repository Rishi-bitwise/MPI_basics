//Number of parts needs to be a multiple of the number of processors.

#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<mpi.h>
#include<stddef.h>

double func(double a)
{
	return (cos(a)+sin(a));
}

typedef struct {
	int nparts;
	double alimit;
	double blimit;
}INTEGRATION_STRUCT;

int main(int argc, char**argv)
{
	int rank, size;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	INTEGRATION_STRUCT IC;
	if (rank==0)
	{
		printf("Enter the number of Partitions, the lower limit and the upper limit for evaluating the integral cos(x)+sin(x)\n\n");
		fflush(stdout);
		scanf("%d %lf %lf", &IC.nparts, &IC.alimit, &IC.blimit);
	}

	int blocklengths[3] = {1,1,1};

	MPI_Aint offset[3];
	offset[0] = offsetof(INTEGRATION_STRUCT, nparts);
	offset[1] = offsetof(INTEGRATION_STRUCT, alimit);
	offset[2] = offsetof(INTEGRATION_STRUCT, blimit);

	MPI_Datatype types[3] = {MPI_INT, MPI_DOUBLE, MPI_DOUBLE};

	MPI_Datatype mpi_struct_type;

	MPI_Type_create_struct(3, blocklengths, offset, types, &mpi_struct_type);
	MPI_Type_commit(&mpi_struct_type);


	MPI_Bcast(&IC, 1, mpi_struct_type, 0, MPI_COMM_WORLD);


	// double window = (BLIMIT-ALIMIT)/NPARTS;
	// double alocal = ALIMIT + rank*window;
	// double blocal = alocal + window;

	//printf("The values in IC are %d, %d, %d\n", IC.nparts, IC.alimit, IC.blimit);

	int nparts = IC.nparts;
	double alimit = IC.alimit;
	double blimit = IC.blimit;

	double ds = (blimit-alimit)/nparts;
	int total_ds_per_process = nparts/size;					
	double windowlength_per_process = ds*total_ds_per_process;

	double alocal = alimit + rank*windowlength_per_process;
	double blocal = alocal + windowlength_per_process;
	//printf("window length is %lf\n", windowlength_per_process);
	printf("Process %d : alocal=%lf, blocal=%lf\n", rank, alocal, blocal);

	double total = func(alocal)+func(blocal);
	total = total/2;

	for(int i=1; i<total_ds_per_process-1; i++)
	{
		total += func(alocal + i*ds);			//summing up the heights for now. the breadth is the same for everyone anyways...
	}

	total = total*ds;
	double final = 0;
	MPI_Reduce(&total, &final, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	//MPI_Reduce(MPI_IN_PLACE, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if(rank==0)
	{
		printf("The final Integrated value, that is the area under the curve is %lf", final);
	}

	MPI_Finalize();

	return 0;
}