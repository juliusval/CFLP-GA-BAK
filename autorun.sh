for i in {1..4}; 
do 
    printf '========Sh=========Shell run:'"$i"'============Sh========\n'; 
    mpirun -n 9 ./mpi_prog.o; 
done
