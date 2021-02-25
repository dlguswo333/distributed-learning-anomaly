
EXE=exe

all:
	mpic++ mpi.cpp -o $(EXE)

clean:
	rm -f $(EXE)

run_local:
	mpiexec --allow-run-as-root -np 2 $(EXE)

run_remote:
	mpiexec --allow-run-as-root -np 2 -H localhost:1,tb3-ib:1 -mca plm_rsh_args "-p 3333" $(EXE)
