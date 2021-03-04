// single.cpp
// It runs one sender and one receiver but they are multithreaded, with MPI and openmp.
#include <mpi.h>
#include <iostream>
#include <chrono>
#include <string>

using namespace std;

const double M=1000000;
int len=0;
int *my_buf=NULL, *other_buf=NULL;
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



void send(int other_rank, int tag, int start, int end){
    MPI_Send(other_buf, end-start, MPI_INT, other_rank, tag, MPI_COMM_WORLD);
    cout << "Sent " << end-start << " elements to " << other_rank << endl;
    return;
}

void recv(int other_rank, int tag, int start, int end){
    MPI_Recv(my_buf, end-start, MPI_INT, other_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    cout << "Received " << end-start << " elements from " << other_rank << endl;
    return;
}
    
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

    other_buf=new int[len];
    my_buf=new int[len];

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    if(provided<MPI_THREAD_MULTIPLE){
        cout << "MPI does not provide thread level\n";
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    auto s=chrono::system_clock::now();

    int send_to=(rank+1)%size;
    int recv_from=(rank-1+size)%size;
    if(rank%2==0){
        send(send_to, 0, 0, len);
        recv(recv_from, 0, 0, len);
    }
    else{
        recv(recv_from, 0, 0, len);
        send(send_to, 0, 0, len);
    }

    auto e=chrono::system_clock::now();
    cout << rank << " " << chrono::duration_cast<chrono::microseconds>(e-s).count()/M << endl;

    delete[] my_buf;
    delete[] other_buf;
    MPI_Finalize();
    return 0;
}
