// bi_single.cpp
// It runs one sender and one receiver bi-directionally.
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include "timer.h"

using namespace std;

const double M=1000000;
int len=1073741824;
int *send_buf=NULL, *recv_buf=NULL;
    
int main(int argc, char *argv[]){
    int rank, size;
    int num_thread=-1;
    char hostname[20];
    int name_len;

    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(size!=2){
        cout << "# of processes is not 2\n";
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Every process spawns two threads.
    // Thread 0 sends.
    // Thread 1 receives.
    send_buf=new int[len];
    recv_buf=new int[len];

    MPI_Status recv_stat;
    MPI_Request recv_req;

    Timer t;
    MPI_Barrier(MPI_COMM_WORLD);
    t.start();

    if(rank==0){
        MPI_Send(send_buf, len, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Irecv(recv_buf, len, MPI_INT, 1, 1, MPI_COMM_WORLD, &recv_req);
        MPI_Wait(&recv_req, &recv_stat);
    }
    else{
        MPI_Send(send_buf, len, MPI_INT, 0, 1, MPI_COMM_WORLD);
        MPI_Irecv(recv_buf, len, MPI_INT, 0, 0, MPI_COMM_WORLD, &recv_req);
        MPI_Wait(&recv_req, &recv_stat);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if(rank==0){
	    cout << t.seconds() << endl;
    }
    

    delete[] send_buf;
    delete[] recv_buf;
    MPI_Finalize();
    return 0;
}
