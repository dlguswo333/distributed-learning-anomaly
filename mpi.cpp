#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <chrono>
#include <string>

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
const long len=1073741824;
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
    cout << "Sends " << end-start << " elements to " << other_rank << endl;
    //MPI_Send(other_buf, end-start, MPI_INT, other_rank, thread_num, MPI_COMM_WORLD);
    return;
}

void recv(int other_rank, int thread_num, int start, int end){
    cout << "Receives " << end-start << " elements from " << other_rank << endl;
    //MPI_Recv(my_buf, end-start, MPI_INT, other_rank, thread_num, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return;
}
    
int main(int argc, char *argv[]){
    int rank, size;
    int provided;
    int num_threads=4;
    chrono::time_point<chrono::steady_clock> s;
    other_buf=new int[len];
    my_buf=new int[len];
    char hostname[20];
    int name_len;
    chrono::time_point<chrono::steady_clock> e;

    /*
    try{
        num_threads=stoi(argv[1]);
    }catch(int expn){
        cout << "(executable) (# of child threads)\n";
        return -1;
    }
    */

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    if(provided<MPI_THREAD_MULTIPLE){
        cout << "MPI does not provide thread level\n";
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //cout << "Hello from " << rank << "/" << size << endl;
    if(rank==0){
        #pragma omp parallel for num_threads(num_threads)
        for(int i=0;i<num_threads;++i){
            auto thread_num=omp_get_thread_num();
            MPI_Get_processor_name(hostname, &name_len);
            //cout << "rank: " << rank << " " << (len/num_threads) << endl;
            s=chrono::steady_clock::now();
            send(rank+size/2, thread_num, (len/num_threads)*(thread_num), (len/num_threads)*(thread_num+1));
            e=chrono::steady_clock::now();
            //cout << hostname << " " << rank << " " << thread_num << " " << chrono::duration_cast<chrono::microseconds>(e - s).count()/M << endl;
        }
    }
    else{
        #pragma omp parallel for num_threads(num_threads)
        for(int i=0;i<num_threads;++i){
            auto thread_num=omp_get_thread_num();
            //cout << "rank: " << rank << " " << (len/num_threads) << endl;
            recv(rank-size/2, thread_num, (len/num_threads)*(thread_num), (len/num_threads)*(thread_num+1));
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
