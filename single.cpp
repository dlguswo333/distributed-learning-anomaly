// single.cpp
// It runs each one sender and one receiver alone with MPI.
#include <mpi.h>
#include <iostream>
#include <chrono>
#include <string>

using namespace std;

const double M=1000000;
const long len=1073741824;
int *my_buf=NULL, *other_buf=NULL;

void send(int other_rank, int start, int end){
    cout << "Sends " << end-start << " elements to " << other_rank << endl;
    MPI_Send(other_buf, end-start, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
    return;
}

void recv(int other_rank, int start, int end){
    cout << "Receives " << end-start << " elements from " << other_rank << endl;
    MPI_Recv(my_buf, end-start, MPI_INT, other_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return;
}
    
int main(int argc, char *argv[]){
    int rank, size;
    chrono::time_point<chrono::steady_clock> s;
    other_buf=new int[len];
    my_buf=new int[len];
    char hostname[20];
    int name_len;
    chrono::time_point<chrono::steady_clock> e;

    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    //cout << "Hello from " << rank << "/" << size << endl;
    if(rank==0){
        MPI_Get_processor_name(hostname, &name_len);
        //cout << "rank: " << rank << " " << (len/num_threads) << endl;
        s=chrono::steady_clock::now();
        send(rank+size/2, 0, len);
        e=chrono::steady_clock::now();
        //cout << hostname << " " << rank << " " << thread_num << " " << chrono::duration_cast<chrono::microseconds>(e - s).count()/M << endl;
    }
    else{
        //cout << "rank: " << rank << " " << (len/num_threads) << endl;
        recv(rank-size/2, 0, len);
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
