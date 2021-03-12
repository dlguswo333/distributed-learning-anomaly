// single.cpp
// It runs one sender and one receiver but they are multithreaded, with MPI and openmp.
#include <mpi.h>
#include <iostream>
#include <chrono>
#include <string>
#include "timer.h"

using namespace std;

const double M=1000000;
int len=0;
int *buf=NULL;
/*void func(int my_rank, int other_rank){
    int *my_buf=new int[len];
    int *other_buf=new int[len];
    MPI_Request req;


    MPI_Irecv(my_buf, len, MPI_INT, other_rank, 0, MPI_COMM_WORLD, &req);
    MPI_Send(other_buf, len, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
    MPI_Wait(&req, MPI_STATUS_IGNORE);

    auto end=chrono::steady_clock::now();

    cout << my_rank << " : " << chrono::duration_cast<chrono::microseconds>(end - start).count()/M << "\n";

    delete[] my_buf;
    delete[] other_buf;
    return;
}*/
    
int main(int argc, char *argv[]){
    int rank, size, chunk_size;
    char hostname[20];
    int name_len;

    try{
        len=stoi(argv[1]);
    }catch(...){
        cout << "Usage: (executable) (buffer length)\n";
        return -1;
    }


    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    buf=new int[len];
    if(len%size!=0){
	cout << "Buffer size cannot be divied\n";
	return -1;
    }
    chunk_size=len/size;

    MPI_Status recv_stat;
    MPI_Request recv_req;

    int send_to=(rank+1)%size;
    int recv_from=(rank-1+size)%size;

    for(int i=0;i<size-1;++i){
	int *send_ptr=buf+chunk_size*((rank-i+size)%size);
	int *recv_ptr=buf+chunk_size*((rank+i+size)%size);
	MPI_Irecv(send_ptr, chunk_size, MPI_INT, recv_from, rank, MPI_COMM_WORLD, &recv_req);
	MPI_Send(recv_ptr, chunk_size, MPI_INT, send_to, send_to, MPI_COMM_WORLD);
	MPI_Wait(&recv_req, &recv_stat);
    }

    Timer t;
    MPI_Barrier(MPI_COMM_WORLD);
    t.start();

    for(int i=0;i<size-1;++i){
	int *send_ptr=buf+chunk_size*((rank-i+size)%size);
	int *recv_ptr=buf+chunk_size*((rank+i+size)%size);
	MPI_Irecv(send_ptr, chunk_size, MPI_INT, recv_from, rank, MPI_COMM_WORLD, &recv_req);
	MPI_Send(recv_ptr, chunk_size, MPI_INT, send_to, send_to, MPI_COMM_WORLD);
	MPI_Wait(&recv_req, &recv_stat);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank==0){
	cout << t.seconds() << endl;
    }

    delete[] buf;
    MPI_Finalize();
    return 0;
}
