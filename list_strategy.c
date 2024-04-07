#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// to creat a type to store string list
typedef struct {
    char* strings[1024]; // Array of string pointers
    int count; // Current number of strings in the list
} StringList;

void initStringList(StringList* list) {
    list->count = 0;
    for (int i = 0; i < 1024; ++i) {
        list->strings[i] = NULL;
    }
}

void addStringToList(StringList* list, const char* str) {
    if (list->count >= 1024) {
        printf("List is full, cannot add more strings.\n");
        return;
    }
    // Allocate memory for the new string and copy it
    list->strings[list->count] = strdup(str);
    if (list->strings[list->count] == NULL) {
        perror("Failed to allocate memory for string");
        exit(EXIT_FAILURE);
    }
    list->count++;
}

void updateStringInList(StringList* list, int index, const char* newString) {
    // Check if the index is valid
    if (index < 0 || index >= list->count) {
        printf("Index out of bounds\n");
        return;
    }
    
    // Free the old string
    free(list->strings[index]);
    
    // Allocate memory for the new string and copy it
    list->strings[index] = strdup(newString);
    if (list->strings[index] == NULL) {
        perror("Failed to allocate memory for new string");
        exit(EXIT_FAILURE);
    }
}

void printLast_and_first_Line(const StringList* list) {
    if (list->count > 0) {
        printf("%s\n ", list->strings[0]);
        printf("%s\n", list->strings[list->count - 1]);
    } else {
        printf("The list is empty.\n");
    }
}

void printStringList(const StringList* list) {
    if (list->count > 0) {
        for (int i = 0; i < list->count; ++i) {
            printf("%s\n", list->strings[i]);
        }
    } else {
        printf("The list is empty.\n");
    }
}

// to drwa the memoty graph
void dealing_memory_graph(StringList* list) {
    if (list->count < 2) {
        return; // Need at least two entries to compare
    }
    double last_memory = 0.0, this_memory = 0.0;
    if (sscanf(list->strings[list->count-2], "%lf", &last_memory) != 1 ||
        sscanf(list->strings[list->count-1], "%lf", &this_memory) != 1) {
        return; // Error parsing the numbers
    }
    
    double diff = this_memory - last_memory;
    // Allocate new memory for the modified last string with graphical representation
    char *result = malloc(strlen(list->strings[list->count-1]) + 256); // +256 for extra characters
    if (!result) {
        perror("Failed to allocate memory for result");
        exit(EXIT_FAILURE);
    }

    // Copy the original last string into the result
    strcpy(result, list->strings[list->count-1]);

    // Append graphical representation
    strcat(result, "   |");
    int graphLength = (int)(fabs(diff) * 100); // Convert difference to positive integer scale
    for (int i = 0; i < graphLength; i++) {
        strcat(result, diff > 0 ? "#" : ":");
    }
    strcat(result, diff > 0 ? "*" : "@");
    
    // Append the difference
    char cur[100];
    sprintf(cur, "(%.2lf)", diff); // Use `diff` directly
    strcat(result, cur);

    // Free the original last string and update the list
    free(list->strings[list->count-1]);
    list->strings[list->count-1] = result;
}

void dealing_cpu_graph(StringList *list) {
    if (list->count == 0) {
        printf("List is empty.\n");
        return;
    }

    // Parse CPU usage from the last string
    double cpu_usage = 0.0;
    if (sscanf(list->strings[list->count - 1], "CPU usage is %lf %%", &cpu_usage) != 1) {
        printf("Failed to parse CPU usage.\n");
        return;
    }

    // Prepare new string
    char *result = malloc(strlen(list->strings[list->count - 1]) + 256); // Allocate with extra space
    if (!result) {
        perror("Failed to allocate memory for CPU graph");
        exit(EXIT_FAILURE);
    }

    // Copy existing string to result and append CPU graph
    strcpy(result, list->strings[list->count - 1]);
    strcat(result, " |");
    
    int bars = (int)(cpu_usage); // Convert to integer to represent each percent as a bar
    for (int i = 0; i < bars; i++) {
        strcat(result, "|"); // Append bars to visualize CPU usage
    }

    // Update the list with the new string
    free(list->strings[list->count - 1]); // Free old string
    list->strings[list->count - 1] = result; // Point to new string
}