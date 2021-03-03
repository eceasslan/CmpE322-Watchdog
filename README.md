# CmpE322-Watchdog

Aim of this project is implementing a watchdog process which is significant in large projects.

"executor" process sends signals according to given input file. "watchdog" process creates children processes that wanted and waits until one of them is terminated. If so, watchdog writes the information of terminated process in an output file and restart it. "process" processes wait until receiving a signal. If so, they write the information of received signal in an output file.

"watchdog" realizes if the system is disturbed and immediately recreates the standart execution environment.

Note: executor.cpp is not developed by me, it was already provided to the students.
