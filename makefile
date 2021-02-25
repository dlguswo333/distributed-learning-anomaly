
EXE=exe
NP=4
HALF_NP=2

all:
	mpic++ mpi.cpp -o $(EXE)

clean:
	rm -f $(EXE)

run_local:
	mpiexec --allow-run-as-root -np $(NP) $(EXE)

run_remote:
	mpiexec --allow-run-as-root -np $(NP) -H localhost:$(HALF_NP),tb3-ib:$(HALF_NP) -mca plm_rsh_args "-p 3333" $(EXE)
