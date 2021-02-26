SHELL:=/bin/bash
NP=2
HALF_NP=1
LEN=1073741824
NUM_THREAD=2
SINGLE_SRC=single.cpp
THREAD_SRC=thread.cpp
SINGLE_EXE=single
THREAD_EXE=thread

all: single thread
	
single: $(SINGLE_SRC)
	mpic++ $(SINGLE_SRC) -o $(SINGLE_EXE)

thread: $(THREAD_SRC)
	mpic++ $(THREAD_SRC) -fopenmp -o $(THREAD_EXE)

clean:
	rm -f $(SINGLE_EXE) $(THREAD_EXE)

run-local:
	time mpiexec --allow-run-as-root -np $(NP) $(SINGLE_EXE) $(LEN) 
	time mpiexec --allow-run-as-root -np $(NP) $(THREAD_EXE) $(LEN) $(NUM_THREAD)

run-remote:
	time mpiexec --allow-run-as-root -mca btl_tcp_if_exclude eno0,eno1 --np $(NP) -H localhost:$(HALF_NP),tb3-ib:$(HALF_NP) -mca plm_rsh_args "-p 3333" $(SINGLE_EXE) $(LEN) 
	time mpiexec --allow-run-as-root -mca btl_tcp_if_exclude eno0,eno1 --np $(NP) -H localhost:$(HALF_NP),tb3-ib:$(HALF_NP) -mca plm_rsh_args "-p 3333" $(THREAD_EXE) $(LEN) $(NUM_THREAD)
