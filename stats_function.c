#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <utmp.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>



char* getUserInfo() {
    struct utmp *utent;
    setutent(); // Open the utmp file
    char * res = malloc(1024);
    if (res == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE); // Handle error as appropriate
    }
    sprintf(res, "%s %-s %s\n", "Login Name", "Terminal", "Host");
    while ((utent = getutent()) != NULL) {
        if (utent->ut_type == USER_PROCESS) {
            sprintf(res,"%s %s %s\n", utent->ut_user, utent->ut_line, utent->ut_host);
        }
    }
    endutent(); // Close the utmp file
    return res; // Caller is responsible for freeing this memory
}

char* get_memory_info() {
    struct sysinfo info;
    sysinfo(&info);

    // Dynamically allocate memory for the result string
    char* result = malloc(1024);
    if (result == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE); // Handle error as appropriate
    }

    long total_phys_mem = info.totalram / 1024;
    long used_phys_mem = (info.totalram - info.freeram) / 1024;
    long total_virt_mem = (info.totalswap + info.totalram) / 1024;
    long used_virt_mem = (info.totalswap - info.freeswap + info.totalram - info.freeram) / 1024;

    // Store the formatted memory usage in the dynamically allocated string
    sprintf(result, "%.2f GB / %.2f GB  -- %.2f GB / %.2f GB",
            (double)used_phys_mem / 1024 / 1024, (double)total_phys_mem / 1024 / 1024,
            (double)used_virt_mem / 1024 / 1024, (double)total_virt_mem / 1024 / 1024);
    
    // Optionally store the result in a list if needed
    // addStringToList(memorylist, count, result);

    return result; // Return the dynamically allocated string
}



typedef struct CPUStats {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
} CPUStats;

void readCPUStats(CPUStats* stats) {
    char line[128];
    FILE* file = fopen("/proc/stat", "r");
    if (!file) {
        perror("Error opening /proc/stat");
        exit(EXIT_FAILURE);
    }

    if (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
               &stats->user, &stats->nice, &stats->system, &stats->idle,
               &stats->iowait, &stats->irq, &stats->softirq, &stats->steal,
               &stats->guest, &stats->guest_nice);
    }
    fclose(file);
}

char* get_cpu_info() {
    CPUStats prev_stats, curr_stats;
    unsigned long long prev_total_time, curr_total_time;

    // Dynamically allocate memory for the result string
    char* result = (char*)malloc(1024 * sizeof(char));
    if (!result) {
        perror("Failed to allocate memory for result string");
        exit(EXIT_FAILURE);
    }

    readCPUStats(&prev_stats);
    sleep(1); // Simulate workload
    readCPUStats(&curr_stats);

    // Calculation as before
    prev_total_time = prev_stats.user + prev_stats.nice + prev_stats.system +
                      prev_stats.idle + prev_stats.iowait + prev_stats.irq +
                      prev_stats.softirq + prev_stats.steal + prev_stats.guest +
                      prev_stats.guest_nice;

    curr_total_time = curr_stats.user + curr_stats.nice + curr_stats.system +
                      curr_stats.idle + curr_stats.iowait + curr_stats.irq +
                      curr_stats.softirq + curr_stats.steal + curr_stats.guest +
                      curr_stats.guest_nice;

    unsigned long long total_time_diff = curr_total_time - prev_total_time;
    unsigned long long idle_time_diff = curr_stats.idle - prev_stats.idle;

    double cpu_usage = 100.0 * (total_time_diff - idle_time_diff) / total_time_diff;
    sprintf(result, "CPU usage is %.2f %%", cpu_usage);

    return result; // Caller must free this memory
}

char* getNumCores() {
    FILE* file = fopen("/proc/cpuinfo", "r"); // Open file "/proc/cpuinfo"
    if (!file) {
        perror("Error opening /proc/cpuinfo");
        return NULL; // Return NULL to indicate failure
    }

    int numCores = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "processor") == line) {
            numCores++; // Count the cores
        }
    }

    fclose(file); // Close file

    // Convert the number of cores to a string
    // Assuming the number of cores won't exceed 999 for buffer size purposes
    char* result = (char*)malloc(1024 * sizeof(char)); // Allocate memory for the result string
    if (result == NULL) {
        perror("Failed to allocate memory");
        return NULL; // Return NULL to indicate failure
    }

    sprintf(result, "Num cores: %d", numCores); // Convert int to string and store in result

    return result; // Return the dynamically allocated string
}


int getusage(){
        struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}


/*
most of the other things to print 
*/
void printbasicsysteminfo(){
        struct utsname systemInfo;
        struct sysinfo sysInfo;

    if (uname(&systemInfo) != -1) {
        printf("### System Information ###\n");
        printf("System Name: %s\n", systemInfo.sysname);//system name
        printf("Machine Name: %s\n", systemInfo.nodename);//machine name
        printf("release : %s\n", systemInfo.release);//release
        printf("version : %s\n", systemInfo.version);// version
        printf("Architecture Name: %s\n", systemInfo.machine);// architecture name

    } else {
        perror("uname");//check error
    }

}

void printlasttime() {
    double uptimeInSeconds;
    FILE *uptimeFile = fopen("/proc/uptime", "r"); // Open the uptime file
    if (!uptimeFile) { // Check if the file was successfully opened
        perror("Failed to open /proc/uptime");
        return; // Exit if file opening failed
    }

    if (fscanf(uptimeFile, "%lf", &uptimeInSeconds) != 1) { // Ensure reading uptime succeeds
        perror("Failed to read uptime");
        fclose(uptimeFile);
        return;
    }

    fclose(uptimeFile);

    // Calculate days, hours, minutes, and seconds from uptime
    int days = (int)uptimeInSeconds / (24 * 3600);
    uptimeInSeconds -= days * 24 * 3600;
    int hours = (int)uptimeInSeconds / 3600;
    uptimeInSeconds -= hours * 3600;
    int minutes = (int)uptimeInSeconds / 60;
    int seconds = (int)uptimeInSeconds - minutes * 60;

    // Total hours since reboot
    int totalHours = days * 24 + hours;

    printf("System running since last reboot: %d days %02d:%02d:%02d (%d:%02d:%02d)\n",
           days, hours, minutes, seconds, totalHours, minutes, seconds);
}