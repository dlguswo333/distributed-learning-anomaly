
EXE=exe

all:
	mpic++ mpi.cpp -o $(EXE)

clean:
	rm -f $(EXE)

run:
	mpiexec -np 2 $(EXE)
