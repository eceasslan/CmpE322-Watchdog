/*! \file watchdog.cpp
    \brief The file which is executed by watchdog process.
    
    \author Ece Dilara Aslan
    \date 02.01.2021

    First, this source file creates its child processes and write their informations into an output file and also into the pipe which is a way of communication with the executor process.
    Then, it sleeps until one of its children is terminated. If a child is killed, then it immediately writes which process is killed into the output file and creates it again.
    In the case of termination of head process, it kills the remaining child processes and then creates them.

    I believe, the main idea of the project is to deal with process fails.
    We will develop larger projects and these projects will have lots of processes.
    In order to make sure that everything is running without any error, we need to check each process all the time.  So we need to know how a watchdog process can be developed.
    At the end of this project, I have learned lots of information about process operations and pipe implementation, and using these operations and pipes gave me a better understanding about these topics.
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
#include <filesystem>
#include <sys/wait.h>
#include<signal.h>
using namespace std;

/*! \var int processNumber
    \brief Contains the number of child processes.
 
    This data is obtained from command line arguments.
*/
int processNumber;
/*! \var char* processOPath
    \brief Contains the path of the output file of the child processes.
 
    This data is obtained from command line arguments.
*/
char* processOPath;
/*! \var char* watchdogOPath
    \brief Contains the path of the output file of the watchdog process.
 
    This data is obtained from command line arguments.
*/
char* watchdogOPath;
/*! \var ofstream fd
    \brief It is the file stream which deals with the output file of the watchdog process.
*/
ofstream fd;
/*! \var ofstream fd2
    \brief It is the file stream which deals with the output file of the child processes.
*/
ofstream fd2;
/*! \struct timespec deltass
    \brief It is used as a parameter in nanosleep() function in order to organize the order of the outputs.
*/
struct timespec delta = {0, 300000000};

/*! \fn static void terminate(int signalNo)
    \brief Handles SIGTERM received by the process.
 
    \param signalNo The value of the received signal.
*/
static void terminate(int signalNo){

    fd << "Watchdog is terminating gracefully";
    fd.close();
    exit(0);

}

/*! \fn int main(int argc, char *argv[])
    \brief Creates child processes, sleeps until one of them is killed and when it happens, creates the killed process again.
 
    \param argc The number of arguments sent from command line.
    \param argv An array of arguments sent from command line.
*/
int main(int argc, char* argv[]){

    processNumber = stoi(argv[1]);
    processOPath = argv[2];
    watchdogOPath = argv[3];
    
    signal(SIGTERM, terminate); //when SIGTERM received, call terminate() function

    fd2.open(processOPath, ios::out); //in order to clean up the output file of the child processes
    fd2.close();
    fd.open(watchdogOPath, ios::out);

    pid_t pids[processNumber+1];
    pids[0] = (long)getpid();

    int pipe = open("/tmp/myfifo", O_WRONLY); //opens pipe in write only mode
    char temppid[30]; //input buffer of pipe
    string temp;

    temp = "P0 " + to_string(pids[0]);
    strcpy(temppid, temp.c_str());
    write(pipe, temppid, strlen(temppid)+1); 

    for(int i=1; i<=processNumber; i++){ //creates child processes
        pid_t childpid;
        childpid = fork();
        if(childpid == -1){
            perror("Fork failed");
            return 1;
        }
        else if(childpid == 0){
            execlp("./process", "./process", processOPath, to_string(i).c_str(), NULL);
        }
        else{
            pids[i] = childpid;
            temp = "P" + to_string(i) + " " + to_string(pids[i]);
            strcpy(temppid, temp.c_str());
            write(pipe, temppid, strlen(temppid)+1);
            fd << "P" << i << " is started and it has a pid of " << pids[i] << endl;
        }
        nanosleep(&delta, &delta);
    }

    while(true){ //being executed until receiving SIGTERM

        pid_t terminated;
        terminated = wait(NULL); //sleeps until one of the child processes is killed

        if(terminated == pids[1]){ //the case of termination of the head process

            fd << "P1 is killed, all processes must be killed" << endl;

            for(int i=2; i<=processNumber; i++){ //kills all of the child processes
                kill(pids[i], SIGTERM);
                wait(NULL);
                nanosleep(&delta, &delta);
            }

            fd << "Restarting all processes" << endl;

            for(int i=1; i<=processNumber; i++){ //creates new child processes
                pid_t childpid;
                childpid = fork();
                if(childpid == -1){
                    perror("Fork failed");
                    return 1;
                }
                else if(childpid == 0){
                    execlp("./process", "./process", processOPath, to_string(i).c_str(), NULL);
                }
                else{
                    pids[i] = childpid;
                    temp = "P" + to_string(i) + " " + to_string(pids[i]);
                    strcpy(temppid, temp.c_str());
                    write(pipe, temppid, strlen(temppid)+1);
                    fd << "P" << i << " is started and it has a pid of " << pids[i] << endl;
                }
                nanosleep(&delta, &delta);
            }
        }

        else{ //the case of the termination of a process other than head process

            int i;
            for(i=2; i<=processNumber; i++){ //finds the terminated process number
                if(pids[i] == terminated){
                    break;
                }
            }

            fd << "P" << i << " is killed" << endl;
            fd << "Restarting P" << i << endl;
            
            pid_t childpid; //creates a new process for the terminated process number
            childpid = fork();
            if(childpid == -1){
                perror("Fork failed");
                return 1;
            }
            else if(childpid == 0){
                execlp("./process", "./process", processOPath, to_string(i).c_str(), NULL);
            }
            else{
                pids[i] = childpid;
                temp = "P" + to_string(i) + " " + to_string(pids[i]);
                strcpy(temppid, temp.c_str());
                write(pipe, temppid, strlen(temppid)+1);
                fd << "P" << i << " is started and it has a pid of " << pids[i] << endl;
            }
        }
    }

    return 0;

}