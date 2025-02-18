// thread.cpp
// It runs one sender and one receiver but they are multithreaded, with MPI and openmp.
#include <mpi.h>
#include <iostream>
#include <chrono>
#include <string>

using namespace std;

const double M=1000000;
long len=0;
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



void send(int other_rank, int thread_num, int start, int end){
    MPI_Send(other_buf, end-start, MPI_INT, other_rank, thread_num, MPI_COMM_WORLD);
    cout << "Sent " << end-start << " elements to " << other_rank << endl;
    return;
}

void recv(int other_rank, int thread_num, int start, int end){
    MPI_Recv(my_buf, end-start, MPI_INT, other_rank, thread_num, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    cout << "Received " << end-start << " elements from " << other_rank << endl;
    return;
}
    
int main(int argc, char *argv[]){
    int rank, size;
    int provided;
    int num_threads=4;
    chrono::time_point<chrono::steady_clock> s;
    char hostname[20];
    int name_len;
    chrono::time_point<chrono::steady_clock> e;

    try{
        len=stoi(argv[1]);
        num_threads=stoi(argv[2]);
    }catch(...){
        cout << "Usage: (executable) (buffer length) (# of child threads)\n";
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
    //cout << "Hello from " << rank << "/" << size << endl;
    if(rank==0){
        for(int i=0;i<num_threads;++i){
            MPI_Get_processor_name(hostname, &name_len);
            //cout << "rank: " << rank << " " << (len/num_threads) << endl;
            s=chrono::steady_clock::now();
            send(rank+size/2, i, (len/num_threads)*(i), (len/num_threads)*(i+1));
            e=chrono::steady_clock::now();
            //cout << rank << " " << thread_num << " " << chrono::duration<double>(e.time_since_epoch()).count() << endl;
            //cout << hostname << " " << rank << " " << thread_num << " " << chrono::duration_cast<chrono::microseconds>(e - s).count()/M << endl;
        }
    }
    else{
        for(int i=0;i<num_threads;++i){
            //cout << "rank: " << rank << " " << (len/num_threads) << endl;
            recv(rank-size/2, i, (len/num_threads)*(i), (len/num_threads)*(i+1));
        }
    }
    /*
    if(rank==0){
        cout << rank << " : " << chrono::duration_cast<chrono::microseconds>(e - s).count()/M << "\n";
    }
    */
    delete[] my_buf;
    delete[] other_buf;
    MPI_Finalize();
    return 0;
}
