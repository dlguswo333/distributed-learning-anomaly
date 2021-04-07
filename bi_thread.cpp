// bi_thread.cpp
// It runs one sender and one receiver bi-directionally, and multi-threaded.
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include "timer.h"

using namespace std;

const double M=1000000;
int len=805306368;
int *send_buf=NULL, *recv_buf=NULL;
vector<MPI_Comm> comms;
    
int main(int argc, char *argv[]){
    int rank, size, provided;
    char hostname[20];
    int name_len;

    comms.resize(2);

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    if(provided<MPI_THREAD_MULTIPLE){
        cout << "MPI does not provide thread level\n";
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(size!=2){
        cout << "# of processes is not 2\n";
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for(int i=0;i<2;++i){
       MPI_Comm_dup(MPI_COMM_WORLD, &comms[i]);
    }

    // Every process spawns two threads.
    send_buf=new int[len];
    recv_buf=new int[len];

    MPI_Status recv_stat;
    MPI_Request recv_req;

    Timer t;
    MPI_Barrier(MPI_COMM_WORLD);
    t.start();

    #pragma omp parallel num_threads(2)
    {
        if((rank%2==0 && omp_get_thread_num()==0) || (rank%2==1 && omp_get_thread_num()==1)){
            // Send.
            MPI_Send(send_buf, len, MPI_INT, (rank+1)%2, 0, comms[(rank+1)%2]);
        }
        else{
            // Receive.
            MPI_Recv(recv_buf, len, MPI_INT, (rank+1)%2, 0, comms[rank%2], &recv_stat);
        }
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
