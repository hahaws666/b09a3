#include "stats_function.c"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "list_strategy.c"


bool isInteger(const char* str) {
    if (str == NULL || *str == '\0') {
        // Empty string or NULL pointer
        return false;
    }

    // Skip leading whitespace characters
    while (*str == ' ' || *str == '\t') {
        str++;
    }

    // Check for an optional sign
    if (*str == '+' || *str == '-') {
        str++;
    }

    //Check if the remaining characters are digits
    while (*str != '\0') {
        if (!isdigit(*str)) {
            return false;
        }
        str++;
    }

    return true;
}// find if char is int

//to store and get the pid for finding memory usage
int store_get_mem_pid(int k) {
    static int g=-1;
    if (k==-1){
        return g;
    }
    else
    {
        g=k;
        return 0;
    }
    
}




//to store and get the pid for finding user usage
int store_get_user_pid(int k) {
    static int g=-1;
    if (k==-1){
        return g;
    }
    else
    {
        g=k;
        return 0;
    }
    
}


//to store and get the pid for finding cpu usage
int store_get_cpu_pid(int k) {
    static int g=-1;
    if (k==-1){
        return g;
    }
    else
    {
        g=k;
        return 0;
    }
    
}


void ctrl_c_handler(int sig) {
    int child_pid=store_get_mem_pid(-1);
    int child_pid2=store_get_user_pid(-1);
    int child_pid3=store_get_cpu_pid(-1);

    printf("do you wanna quit? press[n/y]\n");
    char c;
    c = getchar(); // read user input (y or n)
    if (c== 'y' || c== 'Y' )
    {
        //printf("lalala\n");
        //exit(0);
        // Check if the child process PID is valid
        if (child_pid > 0 && child_pid2>0 && child_pid3 >0) {
        //printf("\nTerminating child process...\n");
        // Send SIGTERM signal to the child process
        kill(child_pid, SIGTERM);
        kill(child_pid2,SIGTERM);
        kill(child_pid3,SIGTERM);
        
        exit(0);
    }
    }

}

void ctrl_z_handler(int sig){
    if (signal(sig, ctrl_z_handler) == SIG_ERR) {
    perror("signal");
    exit(1);
  }
}



int main(int argc, char ** argv) {

    // receiving the command line
    int samples =10;
    int delay =1;
    int samplefound=0;
    int delayfound=0;
    int printsys=0;
    int printuser=0, printgraph=0,printsequential=0;// the init status of what to print


    // receiving the command line
    if (argc == 1) {//print as the most basic form
        printuser = 1;
        printsys = 1;
    }
    else{
        int i=1;
        while(i<argc) {
        char *stringPart = strtok(argv[i], "=");
        //int intPart;
        // char *token; 
        if (strcmp(stringPart, "--system") == 0) {
            printf("This is system.\n");
            printsys=1;
            // displayMemoryandCpu(delay ,samples,1);
            // printbasicsysteminfo(); // Display
            
        }
        else if (strcmp(stringPart, "--sequential") == 0) {
            printsequential=1;
            // displayMemoryandCpu(delay ,samples,0);
            // printuserinfo();
            // printf("--------------------------------------------------------\n");
            // getNumCores();
            // printf("--------------------------------------------------------\n");
            // printbasicsysteminfo(); // Display
            // printlasttime(); // Display last time
        }
        else if ( strcmp(stringPart, "--user") == 0){
            printf("This is user.\n");
            printuser=1;
            // print_sessions_users();
            // printuserinfo();
        } 
        else if(strcmp(stringPart, "--graphics") == 0 ||strcmp(stringPart, "--g") == 0){
            printf("This is graphic.\n");
            printgraph=1;
            // displaymemoryandcpuingarph(delay ,samples,0); 
        }
        else if (strcmp(stringPart, "--tdelay") == 0)
        {
            //change delay
            delay = atoi(strtok(NULL, "")); 
            delayfound = 1;
            printf("tdelay is set to %d seconds.\n", delay);
        }
        else if (strcmp(stringPart,"--samples")==0)
        {
            // change sample
            samples = atoi(strtok(NULL, "")); 
            samplefound=1;
            printf("samples is set to %d \n",samples);
        }
        else if (isInteger(stringPart) ==true)
        {
            if (samplefound ==0)
            {
                // change sample
                samplefound=1;
                samples=atoi(argv[i]);
                printf("samples is set to %d \n",samples);
            }
            else if (samplefound ==1 &&delayfound ==0){
                //change delay
                delayfound=1;
                delay=atoi(argv[i]);
                printf("tdelay is set to %d seconds.\n", delay);
            }
            else
            {
                printf("invalid input.\n");//other
            }
            
            
        }
        else
        {
            printf("invalid input.\n");
            break;
        }
        
        i++;
        }
    }

    // initalizing the results 
    StringList * cpulist= malloc(sizeof(StringList));
    initStringList(cpulist);
    StringList * memory_list= malloc(sizeof(StringList));
    initStringList(memory_list);
    addStringToList(memory_list,"### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)");
    char * core_info= getNumCores();
    addStringToList(cpulist,core_info);

    for(int i=0; i<samples ; i++){
    // Set up the SIGINT signal handler
    int mem_child_pid, user_child_pid, cpu_child_pid;
    int mem_pipe[2],user_pipe[2],cpu_pipe[2];
    char mem_buf[1024],user_buf[1024],cpu_buf[1024];
    signal(SIGINT, SIG_DFL);


    // Create the pipefor user
    if (pipe(user_pipe) == -1 || pipe(mem_pipe)==-1|| pipe(cpu_pipe)==-1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    user_child_pid = fork();
    store_get_user_pid(user_child_pid);

    if (user_child_pid == -1) {
        perror("fork");
        exit(1);
    }
    if (user_child_pid == 0) {    /* Child process */
        close(user_pipe[0]);  // Close unused read end

        // Write a message to the pipe
        char *userInfo = getUserInfo();
        write(user_pipe[1], userInfo, strlen(userInfo) + 1); // Include null terminator

        close(user_pipe[1]);  // Close the write end, signaling to the parent that all data has been sent
        exit(0);
    }
    mem_child_pid = fork();
    store_get_mem_pid(mem_child_pid);
    if (mem_child_pid ==-1)
    {
        perror("fork");
        exit(1);
    }
    if (mem_child_pid ==0)
    {
        close(mem_pipe[0]); // Close unused read   
        char * mem_info = get_memory_info();
        write(mem_pipe[1], mem_info,strlen(mem_info) + 1); // Include null terminator
        close(mem_pipe[1]); // close the pipe
        exit(0);
    }

    cpu_child_pid = fork();
    store_get_cpu_pid(cpu_child_pid);
    if (cpu_child_pid ==-1)
    {
        perror("fork");
        exit(1);
    }
    if (cpu_child_pid ==0)
    {
        close(cpu_pipe[0]);
        char *cpu_info = get_cpu_info();
        write(cpu_pipe[1], cpu_info,strlen(cpu_info)+1); // Include null terminator
        close(cpu_pipe[1]);
        exit(0);
    }
    /* Parent process */
    // if (signal(SIGINT, ctrl_c_handler) == SIG_ERR ||
    //   signal(SIGTSTP, ctrl_z_handler) == SIG_ERR) {
    // perror("signal");
    // exit(1);
    // }
    signal(SIGINT, ctrl_c_handler);
    signal(SIGTSTP, ctrl_z_handler);
    waitpid(user_child_pid,NULL,0);        // Wait for the child process to exit
    waitpid(mem_child_pid,NULL,0); // wait for the child process of memory
    waitpid(cpu_child_pid,NULL,0); //

    close(user_pipe[1]);  // Close unused write end
    close(mem_pipe[1]); // Close unused write end
    close(cpu_pipe[1]); // Close unused write end


        // Read the message from the pipe
    ssize_t user_size = read(user_pipe[0], user_buf, sizeof(user_buf));
    ssize_t mem_size = read(mem_pipe[0], mem_buf, sizeof(mem_buf));
    ssize_t cpu_size = read(cpu_pipe[0],cpu_buf,sizeof(cpu_buf));

    if (user_size > 0 && mem_size > 0 && cpu_size > 0) {
        if (printsys +printuser ==0)
        {
            printsys=1;
            printuser=1;
        }
        
        if (printsequential==0)
        {
            printf("\033[H\033[J"); // This clears the screen (Unix-based systems)
        }
        int memory_usage = getusage();
        printf("Nbr of samples: %d -- every %d secs\n", samples,delay);
        printf("memory usage: %d kilobytes\n", memory_usage);
        printf("---------------------------------------\n");
        if (printsys==1)
        {
            if (printgraph==1)
            {
                printf("graph\n");
                addStringToList(memory_list,mem_buf);
                dealing_memory_graph(memory_list);
                //dealing with the cpu imformation
                addStringToList(cpulist,cpu_buf);
                dealing_cpu_graph(cpulist);
                printStringList(memory_list);
                for (int j = i; j < samples; j++)
                {
                    printf("\n");
                }
                printf("---------------------------------------\n");
                if (printuser==1)
        {
            //print user information
            printf("%s %-s %s\n", "Login Name", "Terminal", "Host");
            printf("---------------------------------------\n");
            printf(" %s\n", user_buf);
        }
                printStringList(cpulist);
            }
            else
            {
                addStringToList(memory_list,mem_buf);
                addStringToList(cpulist,cpu_buf);
                printStringList(memory_list);
                for (int j = i; j < samples; j++)
                {
                    printf("\n");
                }
                printf("---------------------------------------\n");
                if (printuser==1)
                {
                    //print user information
                    printf("%s %-s %s\n", "Login Name", "Terminal", "Host");
                    printf("---------------------------------------\n");
                    printf(" %s\n", user_buf);
                    printf("---------------------------------------\n");
                }
                printLast_and_first_Line(cpulist);
                printf("---------------------------------------\n");
            }
            
            
        }

        if (printuser * printsys == 1)
        {
                printbasicsysteminfo(); // Display
                printlasttime(); // Display last time
        }
        
        
        

    } else {
       printf("No message received.\n");
    }

    close(user_pipe[0]);  // Close the read end
    close(cpu_pipe[0]);
    close(mem_pipe[0]);
    sleep(delay);
    }

    return 0;
}