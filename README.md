# MPI Basics: Distributed Memory Computing

## Overview
This repository contains a collection of C programs implementing foundational parallel algorithms using the Message Passing Interface (MPI). The project explores manual communication structures, custom MPI datatypes, and parallel numerical methods.

## Technical Implementations

### 1. Two-Phase Tree Allreduce (`butterfly-structure.c`)
This program manually implements an "Allreduce" operation where every process receives the final aggregated sum. It does this in two distinct phases rather than relying on `MPI_Allreduce`.
* **Phase 1 (Reduction):** A binary tree communication pattern where the offset halves at each step. Data flows upward until Rank 0 holds the total sum.
* **Phase 2 (Broadcast):** A reverse binary tree where the offset doubles at each step. Rank 0 pushes the final accumulated data back down the tree so all participating ranks receive it.

### 2. Parallel Jacobi Solver (`jacobi.c`)
A distributed solver for a 4-variable system of linear equations using the Jacobi iterative method. 
* **Workload Distribution:** Each of the 4 MPI processes is assigned a specific function to compute the new value for one variable (`x`, `y`, `z`, or `w`).
* **Synchronization:** * `MPI_Allreduce` is used to sum the absolute local errors across all processes into a global error value. The loop continues until this global error falls below the 0.0001 tolerance.
  * `MPI_Allgather` is executed at the end of each iteration to broadcast the newly computed individual variables back into a unified `Transform_vector` on every process, preparing the system for the next mathematical step.

### 3. Custom Tree Reduction (`manual-reduce.c`)
A manual implementation of `MPI_Reduce` using a logarithmic tree structure. Processes pair up and send their data (`rank * 10`) across a shrinking offset. Sender processes break out of the loop after sending, while receiver processes accumulate the data. The final reduced sum is printed by Rank 0.

### 4. Custom Datatypes & Numerical Integration (`trapezoidal.c`)
A parallel implementation of the Trapezoidal rule to evaluate the integral of `cos(x) + sin(x)`. 
* **Custom MPI Struct:** The program defines an `INTEGRATION_STRUCT` to group the number of partitions, lower limit, and upper limit. `MPI_Type_create_struct` is used to map the memory offsets of these C variables so they can be broadcast together via a single `MPI_Bcast`.
* **Execution:** The total integration domain is divided by the number of processors. Each process computes the area of its local segment, and `MPI_Reduce` sums the partial areas into the final integral at Rank 0.

### 5. Master-Worker Communication (`round-robin.c`)
A basic string-passing exercise. All worker processes (ranks greater than 0) format a string containing their rank ID and send it to Rank 0. Rank 0 loops sequentially through the expected incoming messages and prints them. 

## Core Takeaways
* **Algorithmic Redesign:** Sequential tasks must be fundamentally restructured for distributed systems. The Jacobi solver demonstrates how individual variable calculations must be synchronized globally at every step to maintain mathematical correctness.
* **Memory Management:** Defining custom MPI datatypes (as seen in the trapezoidal integrator) is necessary when passing complex C structs across a distributed network, ensuring that memory boundaries are respected during communication.
* **Network Topologies:** Constructing manual reductions and broadcasts highlights the time complexity of data movement. Tree structures reduce communication overhead from linear to logarithmic time.
