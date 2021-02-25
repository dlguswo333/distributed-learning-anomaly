
EXE=exe
NP=2
HALF_NP=1

all:
	mpic++ mpi.cpp -fopenmp -o $(EXE)

clean:
	rm -f $(EXE)

run-local:
	mpiexec --allow-run-as-root -np $(NP) $(EXE) 4

run-remote:
	mpiexec --allow-run-as-root -mca btl_tcp_if_exclude eno0,eno1 --np $(NP) -H localhost:$(HALF_NP),tb3-ib:$(HALF_NP) -mca plm_rsh_args "-p 3333" $(EXE)
