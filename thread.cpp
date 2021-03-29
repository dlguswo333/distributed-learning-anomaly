// thread.cpp
// It runs one sender and one receiver but they are multithreaded, with MPI and openmp.
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
int *buf=NULL;
vector<MPI_Comm> comms;
    
int main(int argc, char *argv[]){
    int rank, size, chunk_size;
    int num_thread=-1;
    int provided;
    char hostname[20];
    int name_len;

    try{
        num_thread=stoi(argv[1]);
    }catch(...){
        cout << "Usage: (executable) (# of threads per process)\n";
        return -1;
    }
    comms.resize(num_thread);

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

    for(int i=0;i<num_thread;++i){
       MPI_Comm_dup(MPI_COMM_WORLD, &comms[i]);
    }

    // Every process spawns two threads.
    // Thread 0 sends.
    // Thread 1 receives.
    buf=new int[len];
    if(len%num_thread!=0){
        cout << "Buffer size cannot be divided\n";
        return -1;
    }
    chunk_size=len/size;

    MPI_Status recv_stat;
    MPI_Request recv_req;

    Timer t;
    MPI_Barrier(MPI_COMM_WORLD);
    t.start();

    if(rank==0){
        // Send.
        #pragma omp parallel for num_threads(num_thread)
        for(int i=0;i<num_thread;++i){
            MPI_Send(buf+chunk_size*i, chunk_size, MPI_INT, 1, 0, comms[i]);
        }
    }
    else{
        // Receive.
        #pragma omp parallel for num_threads(num_thread)
        for(int i=0;i<num_thread;++i){
            MPI_Recv(buf+chunk_size*i, chunk_size, MPI_INT, 0, 0, comms[i], &recv_stat);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if(rank==0){
	    cout << t.seconds() << endl;
    }
    

    delete[] buf;
    MPI_Finalize();
    return 0;
}
