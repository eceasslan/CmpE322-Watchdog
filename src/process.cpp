/*! \file process.cpp
    \brief The file which is executed by child processes.

    \author Ece Dilara Aslan
    \date 02.01.2021
    
    This source file basically receive signals and handles them.
    If the signal is other than SIGTERM, then code writes the related information about process number and signal value into an output file.
    Otherwise, code writes related information about process number and SIGTERM into an output file, and also exits the process.
*/

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fstream>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <csignal>
#include <signal.h>
using namespace std;

/*! \var char* outputPath
    \brief Contains the path of the output file.
 
    This data is obtained from watchdog process.
*/
char* outputPath;
/*! \var int processNo
    \brief Contains the process number.
 
    This data is obtained from watchdog process.
*/
int processNo;
/*! \var ofstream fdp
    \brief It is the file stream which deals with the output file.
*/
ofstream fdp;

/*! \fn static void handle(int signalNo)
    \brief Handles signals received by the process.
 
    \param signalNo The value of the received signal.
*/
static void handle(int signalNo) {

    fdp << "P" << processNo << " received signal " << signalNo;

    if(signalNo == 15){ //the case of receiving SIGTERM
        fdp << ", terminating gracefully" << endl;
        fdp.close();
        exit(0);
    }

    else{ //the case of receiving a signal other than SIGTERM
        fdp << endl;
    }

}


/*! \brief Sleeps until receives a signal and when receives a signal, sends them to handle() function.
 
    \param argc The number of arguments sent to the process.
    \param argv An array of arguments sent to the process.
*/
int main(int argc, char *argv[]) {

    outputPath = argv[1];
    processNo = stoi(argv[2]);

    fdp.open(outputPath, ios::app);

    fdp << "P" << processNo << " is waiting for a signal" << endl;

    signal(SIGTERM, handle); //when SIGTERM received, call handle() function
    signal(SIGINT, handle);  //when SIGINT received, call handle() function
    signal(SIGHUP, handle);  //when SIGHUP received, call handle() function
    signal(SIGILL, handle);  //when SIGILL received, call handle() function
    signal(SIGTRAP, handle); //when SIGTRAP received, call handle() function
    signal(SIGBUS, handle);  //when SIGBUS received, call handle() function
    signal(SIGFPE, handle);  //when SIGFPE received, call handle() function
    signal(SIGSEGV, handle); //when SIGSEGV received, call handle() function
    signal(SIGXCPU, handle); //when SIGXCPU received, call handle() function

    while(true); //sleeps until one of the signals above is received

    return 0;

}