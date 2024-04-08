# b09a3
A simple command line program written in C that reports different metrics of system utilization, able to run the different queries of the system concurrently. The program provides information about basic system information, CPU utilization, memory utilization, and users’ information.

## how to use
open in terminal the press "make" then input the "./prog" into the terminal then press the enter.
and in the prog you can add flages as the a1 did
you can press ctrl c to stop it and the program will ask you do you really want to quit an press Y or y to quit , others otherwise
if you press ctrl z you will do nothing to the program

## how i get this
i use 3 forks to hav child process to find the memory usage , users and the cpu usage
i set the ctrl c and ctrl z handlers to deal with the interruptions
i create string list to store the sting which contains the informations about the cpu memory

## functions
### void ctrl_c_handler(int sig) 
to deal with the ctrl c 
### void ctrl_z_handler(int sig)
to deal with ctrl z
### void addStringToList(StringList* list, const char* str)
add string to list
### void printLast_and_first_Line(const StringList* list) 
print the last line and the first line typivally to deal with the cpu output
### void dealing_memory_graph(StringList* list)
make the sting list of memoty into grapghically
### void dealing_cpu_graph(StringList *list)
make the sting list of cpu into grapghically
### char* getUserInfo()
toacquire the userinfo
### char* get_memory_info()
memory info
### void printbasicsysteminfo()
print informations about the system
### int getusage()
get the usage of the program
### char* getNumCores()
get the numbers of the core
