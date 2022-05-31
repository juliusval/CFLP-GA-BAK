printf '========Sh=========Test run:'"$i"'============Sh========\n'; 
mpic++ *.cpp -o mpi_prog.o
mpirun -n 9 ./mpi_prog.o; 
