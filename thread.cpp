// thread.cpp
// It runs one sender and one receiver but they are multithreaded, with MPI and openmp.
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <chrono>
#include <string>
#include "timer.h"

using namespace std;

const double M=1000000;
int len=0;
int *buf=NULL;
MPI_Comm comms[2];
    
int main(int argc, char *argv[]){
    int rank, size, chunk_size;
    int provided;
    char hostname[20];
    int name_len;

    try{
        len=stoi(argv[1]);
    }catch(...){
        cout << "Usage: (executable) (buffer length)\n";
        return -1;
    }

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    if(provided<MPI_THREAD_MULTIPLE){
        cout << "MPI does not provide thread level\n";
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Will it really work? To duplicate the communicator.
    //for(int i=0;i<2;++i){
    //    MPI_Comm_dup(MPI_COMM_WORLD, &comms[i]);
    //}

    // Every process spawns two threads.
    // Thread 0 sends.
    // Thread 1 receives.
    buf=new int[len];
    if(len%size!=0){
	cout << "Buffer size cannot be divided\n";
	return -1;
    }
    chunk_size=len/size;

    MPI_Status recv_stat;
    MPI_Request recv_req;

    int send_to=(rank+1)%size;
    int recv_from=(rank-1+size)%size;

    #pragma omp parallel num_threads(2)
    {
	if(omp_get_thread_num()==0){
	    for(int i=0;i<size-1;++i){
		int *send_ptr=buf+chunk_size*((rank-i+size)%size);
		MPI_Send(send_ptr, chunk_size, MPI_INT, send_to, send_to, MPI_COMM_WORLD);
	    }
	}
	else{
	    for(int i=0;i<size-1;++i){
		int *recv_ptr=buf+chunk_size*((rank+i+size)%size);
		MPI_Recv(recv_ptr, chunk_size, MPI_INT, recv_from, rank, MPI_COMM_WORLD, &recv_stat);
	    }
	}
    }

    Timer t;
    MPI_Barrier(MPI_COMM_WORLD);
    t.start();

    #pragma omp parallel num_threads(2)
    {
	if(omp_get_thread_num()==0){
	    for(int i=0;i<size-1;++i){
		int *send_ptr=buf+chunk_size*((rank-i+size)%size);
		MPI_Send(send_ptr, chunk_size, MPI_INT, send_to, send_to, MPI_COMM_WORLD);
	    }
	}
	else{
	    for(int i=0;i<size-1;++i){
		int *recv_ptr=buf+chunk_size*((rank+i+size)%size);
		MPI_Recv(recv_ptr, chunk_size, MPI_INT, recv_from, rank, MPI_COMM_WORLD, &recv_stat);
	    }
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
