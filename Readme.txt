Environment setup:
1- Install the following packages before building the Toolchain and Kernel:
	
	nasm
	qemu
	gcc
	make

2- Build the Toolchain

Execute the following commands on the x64BareBones project directory:
  user@linux:$ cd Toolchain
  user@linux:$ make all

3- Set up tools
	
	a- Modify Tools.inc to point to your compilers/linkers appropriate for building the project
	b- Modify run.sh to point to your qemu-system-x86_64 executable to run the project once built

4- Build the Kernel

From the project directory, run:
  user@linux:$ make all

5- Run the kernel

From the project directory, run:
  user@linux:$ ./run.sh


Author: Rodrigo Rearden (RowDaBoat)
Collaborator: Augusto Nizzo McIntosh