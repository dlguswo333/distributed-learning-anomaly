printf "41\n\n"
for i in {1..10}
do
    time mpiexec --allow-run-as-root --np 5 -H localhost:4,tb3-ib:1 -mca plm_rsh_args "-p 3333" single 1073741825
    sleep 10s;
done

printf "32\n\n"
for i in {1..10}
do
    time mpiexec --allow-run-as-root --np 5 -H localhost:3,tb3-ib:2 -mca plm_rsh_args "-p 3333" single 1073741825
    sleep 10s;
done

printf "23\n\n"
for i in {1..10}
do
    time mpiexec --allow-run-as-root --np 5 -H localhost:2,tb3-ib:3 -mca plm_rsh_args "-p 3333" single 1073741825
    sleep 10s;
done

printf "14\n\n"
for i in {1..10}
do
    time mpiexec --allow-run-as-root --np 5 -H localhost:1,tb3-ib:4 -mca plm_rsh_args "-p 3333" single 1073741825
    sleep 10s;
done

