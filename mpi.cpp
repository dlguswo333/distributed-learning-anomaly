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

void func(int my_rank, int other_rank){
    int *my_buf=new int[len];
    int *other_buf=new int[len];
    MPI_Request req;

    auto start=chrono::steady_clock::now();

    MPI_Irecv(my_buf, len, MPI_INT, other_rank, 0, MPI_COMM_WORLD, &req);
    MPI_Send(other_buf, len, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
    MPI_Wait(&req, MPI_STATUS_IGNORE);

    auto end=chrono::steady_clock::now();

    cout << my_rank << " : " << chrono::duration_cast<chrono::microseconds>(end - start).count()/M << "\n";

    delete[] my_buf;
    delete[] other_buf;
    return;
}

void improve_func(int my_rank, int other_rank){
    int *my_buf=new int[len];
    int *other_buf=new int[len];
    MPI_Request req;
    pthread_t childs[2];

    auto start=chrono::steady_clock::now();


    MPI_Irecv(my_buf, len, MPI_INT, other_rank, 0, MPI_COMM_WORLD, &req);
    MPI_Send(other_buf, len, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
    MPI_Wait(&req, MPI_STATUS_IGNORE);

    auto end=chrono::steady_clock::now();

    cout << "wth?\n";
    cout << my_rank << " : " << chrono::duration_cast<chrono::microseconds>(end - start).count()/M << "\n";

    delete[] my_buf;
    delete[] other_buf;
    return;
}
void recv(void *st){

}
    
int main(int argc, char *argv[]){
    int rank;
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if(provided<MPI_THREAD_MULTIPLE){
        cout << "MPI does not provide thread level\n";
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    cout << "Hello from " << rank << endl;
    if(rank==0){
        func(0, 1);
    }
    else{
        func(1, 0);
    }
    MPI_Finalize();
    return 0;
}
