
EXE=exe

all:
	mpic++ mpi.cpp -o $(EXE)

clean:
	rm -f $(EXE)

run:
	mpiexec --allow-run-as-root -np 2 $(EXE)
