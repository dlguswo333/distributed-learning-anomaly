#include <omp.h>
#include <iostream>
using namespace std;

int main(){
    int num_threads=4;
    #pragma omp parallel for num_threads(num_threads)
    for(int i=0;i<num_threads;++i){
        cout << "Hello from " << omp_get_thread_num() << "\n";
    }
    return 0;
}
