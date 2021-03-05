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
    int rank, size;
    int provided;
    char hostname[20];
    int name_len;

    try{
        len=stoi(argv[1]);
    }catch(...){
        cout << "Usage: (executable) (buffer length)\n";
        return -1;
    }

    buf=new int[(long)len*2];

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    if(provided<MPI_THREAD_MULTIPLE){
        cout << "MPI does not provide thread level\n";
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Status recv_stat;
    MPI_Request recv_req;

    int send_to=(rank+1)%size;
    int recv_from=(rank-1+size)%size;

    Timer t;

    MPI_Irecv(buf, len/size, MPI_INT, recv_from, rank, MPI_COMM_WORLD, &recv_req);
    MPI_Send(buf+len, len/size, MPI_INT, send_to, send_to, MPI_COMM_WORLD);
    cout << rank << " sent " << t.get_now() << endl;
    MPI_Wait(&recv_req, &recv_stat);
    cout << rank << " tested " << t.get_now() << endl;


    delete[] buf;
    MPI_Finalize();
    return 0;
}
