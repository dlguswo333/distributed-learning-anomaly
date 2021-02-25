#include <mpi.h>
#include <iostream>
#include <chrono>

typedef struct{
    int *buf;
    int len;
    int other_rank;
    MPI_Request *req;
}St1;

typedef struct{
    int *buf;
    int len;
    int other_rank;
    MPI_Request *req;
}St2;

using namespace std;

const double M=1000000;
const int len=1000000000;

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



void send(int other_rank, int start, int end){
    int *other_buf=new int[len];
    MPI_Send(other_buf, end-start, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
    return;
}

void recv(int other_rank, int start, int end){
    int *my_buf=new int[len];
    MPI_Recv(my_buf, end-start, MPI_INT, other_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return;
}
    
int main(int argc, char *argv[]){
    int rank, size;
    int provided;
    chrono::time_point<chrono::steady_clock> s;
    chrono::time_point<chrono::steady_clock> e;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if(provided<MPI_THREAD_MULTIPLE){
        cout << "MPI does not provide thread level\n";
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    cout << "Hello from " << rank << "/" << size << endl;
    if(rank==0){
        s=chrono::steady_clock::now();
    }
    if(rank<size/2){
        send(rank+size/2, (len/size)*rank, (len/size)*(rank+1));
    }
    else{
        recv(rank-size/2, (len/size)*rank, (len/size)*(rank+1));
    }
    if(rank==0){
        e=chrono::steady_clock::now();
        cout << rank << " : " << chrono::duration_cast<chrono::microseconds>(e - s).count()/M << "\n";
    }
    MPI_Finalize();
    return 0;
}
