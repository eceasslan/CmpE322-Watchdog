# CmpE322-Watchdog

Aim of this project is implementing a watchdog process which is significant in large projects.

"executor" process sends signals according to given input file. "watchdog" process creates children processes that wanted and waits until one of them is terminated. If so, watchdog writes the information of terminated process in an output file and restart it. "process" processes wait until receiving a signal. If so, they write the information of received signal in an output file.

"watchdog" realizes if the system is disturbed and immediately recreates the standart execution environment.

Compilation and run:
g++ executor.cpp -std=c++14 -o executor
g++ process.cpp -std=c++14 -o process
g++ watchdog.cpp -std=c++14 -o watchdog

./executor number_of_"process" instruction_path &
./watchdog number_of_"process" process_output watchdog_output

Project documentation can be reached via index.html file in the Doxygen directory.

Note: executor.cpp is not developed by me, it was already provided to the students.
