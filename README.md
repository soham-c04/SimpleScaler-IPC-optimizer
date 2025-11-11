# Automated SimpleScaler IPC optimizer

Our goal is to produce the optimal IPC for a given C code by changing the default.cfg configuration file to an optimal configuration which produces the best possible IPC, by performing an exhaustive search on all possible combinations of varying range of parameters, which is given by user.
<br> <br>

## Table of Contents
- [**Benchmarks**](https://github.com/soham-c04/SimpleScaler-IPC-optimizer?tab=readme-ov-file#benchmarks)
- [**Important Points**](https://github.com/soham-c04/SimpleScaler-IPC-optimizer?tab=readme-ov-file#important-points)
- [**How to run**](https://github.com/soham-c04/SimpleScaler-IPC-optimizer?tab=readme-ov-file#how-to-run)
- [**Parameter Input**](https://github.com/soham-c04/SimpleScaler-IPC-optimizer?tab=readme-ov-file#parameter-input)
- [**Error Codes**](https://github.com/soham-c04/SimpleScaler-IPC-optimizer?tab=readme-ov-file#error-codes)
- [**Cross Compiler Setup**](https://github.com/soham-c04/SimpleScaler-IPC-optimizer?tab=readme-ov-file#cross-compiler-setup)
<br>

## Benchmarks
Shows the comparison between the IPC produced when default.cfg is used, the best possible IPC, and the worst possible IPC(to show the worst case scenario if parameters in config.cfg had been chosen incorrectly) from the set of input parameters

| Program | Default IPC | Best IPC | Worst IPC |
|:----------:|:----------:|:----------:|:----------:|
| [Matrix Multiplication](https://github.com/soham-c04/SimpleScaler-IPC-optimizer/blob/main/benchmarks/matmul.c) | 2.0848 | 2.1711 | 0.9015 | 
| [Testcase](https://github.com/soham-c04/SimpleScaler-IPC-optimizer/blob/main/benchmarks/testcase.c) | 1.7804 | 3.0522 | 0.4153 | 

<br>

## Important Points
- Size of cache/TLB is always kept constant i.e. whatever is initially present in default.cfg. This is to ensure valid comparison.
- To change size of cache change values in default.cfg.
- Block sizes greater than 64 might not be supported in this version of SimpleScalar.
- The total simulation/computation time is = $\text{code running time} \times \text{(product of no. of values each parameter can take)}$.
- Hence, with each increase in additional parameter value, the search space and time requried for search blows up exponentially.
<br> <br>

## How to Run
- Make sure Environment for SimpleScalar cross-compiler is properly setup. ([How to do?](https://github.com/soham-c04/SimpleScaler-IPC-optimizer?tab=readme-ov-file#cross-compiler-setup))
- The parent of [optimizer.cpp](https://github.com/soham-c04/SimpleScaler-IPC-optimizer/blob/main/optimizer.cpp) and **installn_files** should be the same. Otherwise, replace `../installn_files` in [optimizer.cpp](https://github.com/soham-c04/SimpleScaler-IPC-optimizer/blob/main/optimizer.cpp) by path to that folder.
- Run the command `./run.sh` (Use `chmod +x run.sh` first if it shows Permission denied).
- Enter relative path to the C code to be optimized.
- Enter relative path to parameters file.
<br> <br>

## Parameter Input
- Create a file with read access (can be .txt file).
- Enter parameters non-cache/non-TLB parameters in the following format:
  - Enter the keyword to parameter as present in default.cfg file. E.g:-
    - `-bpred`: Branch Predictor
    - `-decode:width`: Instruction decode B/W (insts/cycle)
    - `-issue:width`: Instruction issue B/W (insts/cycle)
  - On the same line enter a list of single-space-separated values you want that parameter to take.
  - Go to a new-line and then repeat until required non-cache/non-TLB parameters are given.
- Enter a newline (press Enter).
- Enter cache/TLB paramters in following format:
  - Enter inputs to (atmost) 5 rows in order for: dl1-cache, il1-cache, dl2-cache, d-tlb, i-tlb
  - In each row enter
    - list of single-space-separated associativity values (Use 0 for default value)
    - 2 spaces
    - list of single-space-separated block sizes (Use 0 for default value)
    - 2 spaces
    - list of single-space-separated single-character Replacement policies (e.g - l,f,r) (Use 0 for default value).
  - Not all rows need to be entered. Say, if only values in first 3-rows are given, then dl1, il1 and dl2 will respectively iterate over those values and remaining d-tlb and i-tlb will take default values from default.cfg.
  - For a given row all 3 columns need to be entered even if by 0 (default value).
- Adding extra/unnecessary spaces, tabs or newlines other than the ones mentioned above will result in error in reading from file.
- Example files - [parameters.txt](https://github.com/soham-c04/SimpleScaler-IPC-optimizer/blob/main/parameters.txt), [parameters1.txt](https://github.com/soham-c04/SimpleScaler-IPC-optimizer/blob/main/parameter_inputs/parameters1.txt), [parameters2.txt](https://github.com/soham-c04/SimpleScaler-IPC-optimizer/blob/main/parameter_inputs/parameters2.txt)
<br> <br>

## Error Codes
If the code returns with an exit code of 0, this implies that the simulation ran perfectly. Otherwise, following are the reasons and possible fix for each exit code:
- **1**
  - Reason: void.c unable to run with config.cfg file 
  - Fix: Check if all parameters in config.cfg file are having values within correct bounds (e.g. cache parameters). Check out.txt file and look for hints to errors in there.
- **2** and **4**
  - Reason: Rewriting of instruction count and cycle count into out.txt failed
  - Fix:    Check if out.txt was correctly generated/overwritten after previous command.
- **3**
  - Reason: Unable to run Given C code with sim-outorder and config.cfg.
  - Fix:    Fixing possible runtime error in the code (division by 0, out of bounds memory access). Should be compatible with older gcc version 2.7.2.3.
- **5**
  - Reason: Compilation of given C code failed.
  - Fix:    Fixing Syntax errors w.r.t gcc 2.7.2.3.
- **6**
  - Reason: Copying from default.cfg to config.cfg failed.
  - Fix:    Keep default.cfg in same directory as optimizer.cpp.
<br> <br>

## Cross Compiler Setup
- Note that the following setup was done in WSL2 in Windows 11.
- Extract [simplescalar-installn-files.zip](http://www.cse.iitd.ac.in/%7Edrajeswari/CSL718/simplescalar-installn-files.zip) to any folder.
- Run the following commands in exact order:<br>
Here `~` expands to `/home/<user>` in my case

| No. | Location | Command |
|:---:|:----------:|:--------:|
| 0 | Doesn't matter | cd ~ |
| 1 | ~ | cp -r \<path to installn_files> ~/ |
| 2 | ~ | cd installn_files |
| 3 | ~/installn_files | tar -xvzf simplesim-3v0d.tgz |
| 4 | ~/installn_files | tar -xvzf simpletools-2v0.tgz |
| 5 | ~/installn_files | tar -xvzf simpleutils-990811.tar.gz |
| 6 | ~/installn_files | tar -xvzf gcc-2.7.2.3.ss.tar.gz |
| 7 | ~/installn_files | rm -rf gcc-2.6.3 |
| 8 | ~/installn_files | export IDIR=$(pwd) |
| 9 | ~/installn_files | export HOST=i686-pc-linux |
| 10 | ~/installn_files | export TARGET=sslittle-na-sstrix |
| 11 | ~/installn_files | sudo apt update |
| 12 | ~/installn_files | sudo apt install build-essential flex bison |
| 13 | ~/installn_files | which flex |
| 14 | ~/installn_files | cd $IDIR/simpleutils-990811 |
| 15 | ~/installn_files/simpleutils-990811 | nano ld/ldlex.l (replace yy_current_buffer with YY_CURRENT_BUFFER) |
| 16 | ~/installn_files/simpleutils-990811 | ./configure --host=$HOST --target=$TARGET --with-gnu-as --with-gnu-ld --prefix=$IDIR |
| 17 | ~/installn_files/simpleutils-990811 | make CFLAGS=-O |
| 18 | ~/installn_files/simpleutils-990811 | make install |
| 19 | ~/installn_files/simpleutils-990811 | cd $IDIR/simplesim-3.0 |
| 20 | ~/installn_files/simplesim-3.0 | make config-pisa |
| 21 | ~/installn_files/simplesim-3.0 | make |
| 22 | ~/installn_files/simplesim-3.0 | ./sim-safe tests/bin.little/test-math |
| 23 | ~/installn_files/simplesim-3.0 | cd $IDIR/gcc-2.7.2.3 |
| 24 | ~/installn_files/gcc-2.7.2.3 | ./configure --host=$HOST --target=$TARGET --with-gnu-as --with-gnu-ld --prefix=$IDIR |
| 25 | ~/installn_files/gcc-2.7.2.3 | chmod -R +w . |
| 26 | ~/installn_files/gcc-2.7.2.3 | nano protoize.c → change #include <varargs.h> → #include <stdarg.h> |
| 27 | ~/installn_files/gcc-2.7.2.3 | nano obstack.h → change )++ → ++) |
| 28 | ~/installn_files/gcc-2.7.2.3 | cp ./patched/sys/cdefs.h ../sslittle-na-sstrix/include/sys/cdefs.h |
| 29 | ~/installn_files/gcc-2.7.2.3 | cp ../sslittle-na-sstrix/lib/libc.a ../lib/ |
| 30 | ~/installn_files/gcc-2.7.2.3 | cp ../sslittle-na-sstrix/lib/crt0.o ../lib/ |
| 31 | ~/installn_files/gcc-2.7.2.3 | cp $IDIR/ar $IDIR/sslittle-na-sstrix/bin/ |
| 32 | ~/installn_files/gcc-2.7.2.3 | cp $IDIR/ranlib $IDIR/sslittle-na-sstrix/bin/ |
| 33 | ~/installn_files/gcc-2.7.2.3 | sudo apt-get update |
| 34 | ~/installn_files/gcc-2.7.2.3 | sudo apt-get install libc6-dev-i386 gcc-multilib g++-multilib |
| 35 | ~/installn_files/gcc-2.7.2.3 | make LANGUAGES=c CFLAGS=-O CC="gcc -m32" |
| 36 | ~/installn_files/gcc-2.7.2.3 | nano insn-output.c → add \ at ends of lines 675, 750, 823 |
| 37 | ~/installn_files/gcc-2.7.2.3 | nano gcc.c → add int sys_nerr = 0; below extern int sys_nerr; |
| 38 | ~/installn_files/gcc-2.7.2.3 | make LANGUAGES=c CFLAGS=-O CC="gcc -m32" |
| 39 | ~/installn_files/gcc-2.7.2.3 | make enquire |
| 40 | ~/installn_files/gcc-2.7.2.3 | $IDIR/simplesim-3.0/sim-safe ./enquire -f > float.h-cross |
| 41 | ~/installn_files/gcc-2.7.2.3 | make LANGUAGES=c CFLAGS=-O CC="gcc -m32" install |
| 42 | ~/installn_files/gcc-2.7.2.3 | mkdir ~/simplescalar-test |
| 43 | ~/installn_files/gcc-2.7.2.3 | cd ~/simplescalar-test |
| 44 | ~/simplescalar-test | nano hello.c |
| 45 | ~/simplescalar-test | Write the following inside hello.c:<br>#include <stdio.h><br><br>void main(){<br>printf("Cross Compiler Working!!\\n");<br>} |
| 46 | ~/simplescalar-test | $IDIR/bin/sslittle-na-sstrix-gcc -o hello hello.c |
| 47 | ~/simplescalar-test | $IDIR/simplesim-3.0/sim-safe hello |
