// thread.cpp
// It runs one sender and one receiver but they are multithreaded, with MPI and openmp.
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <chrono>
#include <string>

using namespace std;

const double M=1000000;
int len=0;
int *buf=NULL;
MPI_Comm comms[2];
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



void send(int other_rank, int tag, int start, int len){
    MPI_Send(buf+start, len, MPI_INT, other_rank, tag, comms[other_rank%2]);
    cout << "Sent " << len << " elements to " << other_rank << endl;
    return;
}

void recv(int other_rank, int tag, int start, int len){
    MPI_Recv(buf+start, len, MPI_INT, other_rank, tag, comms[(other_rank+1)%2], MPI_STATUS_IGNORE);
    cout << "Received " << len << " elements from " << other_rank << endl;
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

    try{
        buf=new int[(long)len*2];
    }catch( ... ){
        cout << "Allocating memory failed.\n";
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
    for(int i=0;i<2;++i){
        MPI_Comm_dup(MPI_COMM_WORLD, &comms[i]);
    }

    // Every process spawns two threads.
    // Thread 0 sends.
    // Thread 1 receives.
    #pragma omp parallel for num_threads(2)
    for(int i=0;i<2;++i){
        int send_to=(rank+1)%size;
        int recv_from=(rank-1+size)%size;
        if(i==0){
            auto s=chrono::system_clock::now();
            send(send_to, send_to, 0, len/size);
            auto e=chrono::system_clock::now();
            cout << rank << " send " << chrono::duration_cast<chrono::microseconds>(e-s).count()/M << endl;
        }
        else{
            auto s=chrono::system_clock::now();
            recv(recv_from, rank, len, len/size);
            auto e=chrono::system_clock::now();
            cout << rank << " recv " << chrono::duration_cast<chrono::microseconds>(e-s).count()/M << endl;
        }
    }
    

    delete[] buf;
    MPI_Finalize();
    return 0;
}
