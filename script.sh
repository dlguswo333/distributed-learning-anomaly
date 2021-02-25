printf "local\n" >> /data/out.txt
for i in {1..20}
do
    { time make run-local ;} 2>> /data/out.txt
    sleep 10s;
done

printf "remote\n" >> /data/out.txt
for i in {1..20}
do
    { time make run-remote ;} 2>> /data/out.txt
    sleep 10s;
done
