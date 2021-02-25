
EXE=exe
NP=8
HALF_NP=4

all:
	mpic++ mpi.cpp -o $(EXE)

clean:
	rm -f $(EXE)

run-local:
	mpiexec --allow-run-as-root -np $(NP) $(EXE)

run-remote:
	mpiexec --allow-run-as-root -mca btl_tcp_if_exclude eno0,eno1 --np $(NP) -H localhost:$(HALF_NP),tb3-ib:$(HALF_NP) -mca plm_rsh_args "-p 3333" $(EXE)
