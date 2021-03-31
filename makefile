SHELL:=/bin/bash
NP=2
HALF_NP=1
LEN=1073741824
NUM_THREAD=2
COMPILER=mpigxx
OMP_FLAG=-fopenmp

SINGLE_SRC=single.cpp
THREAD_SRC=thread.cpp
SERIAL_SRC=serial.cpp
BI_SINGLE_SRC=bi_single.cpp
BI_THREAD_SRC=bi_thread.cpp

TIMER_SRC=timer.cpp
SINGLE_EXE=single
THREAD_EXE=thread
SERIAL_EXE=serial
BI_SINGLE_EXE=bi_single
BI_THREAD_EXE=bi_thread

all: single thread serial bi_single
	
single: $(SINGLE_SRC)
	$(COMPILER) $(SINGLE_SRC) $(TIMER_SRC) -o $(SINGLE_EXE)

thread: $(THREAD_SRC)
	$(COMPILER) $(THREAD_SRC) $(TIMER_SRC) $(OMP_FLAG) -o $(THREAD_EXE)

serial: $(SERIAL_SRC)
	$(COMPILER) $(SERIAL_SRC) $(TIMER_SRC) -o $(SERIAL_EXE)

bi_single: $(BI_SINGLE_SRC)
	$(COMPILER) $(BI_SINGLE_SRC) $(TIMER_SRC) -o $(BI_SINGLE_EXE)

bi_thread: $(BI_THREAD_SRC)
	$(COMPILER) $(BI_THREAD_SRC) $(TIMER_SRC) $(OMP_FLAG) -o $(BI_THREAD_EXE)

clean:
	rm -f $(SINGLE_EXE) $(THREAD_EXE) $(SERIAL_EXE) $(BI_THREAD_EXE) $(BI_SINGLE_EXE)

run-local:
	time mpiexec --allow-run-as-root -np $(NP) $(SINGLE_EXE) $(LEN) 
	time mpiexec --allow-run-as-root -np $(NP) $(THREAD_EXE) $(LEN) 
	#time mpiexec --allow-run-as-root -np $(NP) $(SERIAL_EXE) $(LEN) $(NUM_THREAD)

run-remote:
	time mpiexec --allow-run-as-root -mca btl_tcp_if_exclude eno0,eno1 --np $(NP) -H localhost:$(HALF_NP),tb3-ib:$(HALF_NP) -mca plm_rsh_args "-p 3333" $(SINGLE_EXE) $(LEN) 
	time mpiexec --allow-run-as-root -mca btl_tcp_if_exclude eno0,eno1 --np $(NP) -H localhost:$(HALF_NP),tb3-ib:$(HALF_NP) -mca plm_rsh_args "-p 3333" $(THREAD_EXE) $(LEN) 
	#time mpiexec --allow-run-as-root -mca btl_tcp_if_exclude eno0,eno1 --np $(NP) -H localhost:$(HALF_NP),tb3-ib:$(HALF_NP) -mca plm_rsh_args "-p 3333" $(SERIAL_EXE) $(LEN) $(NUM_THREAD)
