#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int global_time = 0;

// Define the instruction execution times
struct Instruction {
    char name[10];
    int exec_time;
};

// Define a process
struct Process {
    char name[10];
    int executionTime;
    int priority;
    char classType[10];
    int arrivalTime;
    int currentLine;
    int timequantum;
};

// Comparison function for sorting processes based on priority and arrival time
int compareProcesses(const void *a, const void *b) {
    const struct Process *p1 = (const struct Process *)a;
    const struct Process *p2 = (const struct Process *)b;
    // sort based on first arrival time then priority then name of the process
    if (p1->arrivalTime < p2->arrivalTime)
    {
        return -1;
    }
    else if (p1->arrivalTime > p2->arrivalTime)
    {
        return 1;
    }
    else if (p1->priority < p2->priority)
    {
        return -1;
    }
    else if (p1->priority > p2->priority)
    {
        return 1;
    }
    else
    {
        return strcmp(p1->name, p2->name);
    }
}

// Structure for the ready queue
struct ReadyQueue
{
    struct Process processes[50];
    int front;
    int rear;
};

// Function to check if the ready queue is empty
int isReadyQueueEmpty(struct ReadyQueue *queue)
{
    return queue->front == -1;
}

// Function to enqueue a process into the ready queue
void enqueue(struct ReadyQueue *queue, struct Process process)
{
    if (queue->front == -1)
    {
        queue->front = 0;
    }
    queue->rear++;
    queue->processes[queue->rear] = process;
}

// Function to dequeue a process from the ready queue
struct Process dequeue(struct ReadyQueue *queue)
{
    struct Process process = queue->processes[queue->front];
    if (queue->front == queue->rear)
    {
        queue->front = -1;
        queue->rear = -1;
    }
    else
    {
        queue->front++;
    }
    return process;
}

int main()
{
    // Read instruction times
    struct Instruction instr;
    // exactly 20 instructions
    struct Instruction all_instr[20];

    FILE *instructionFile = fopen("instructions.txt", "r");
    // for each line in the file, read the instruction name and execution tim
    int i = 0;
    while (fscanf(instructionFile, "%s %d", instr.name, &instr.exec_time) == 2)
    {
        // store the instructions in an array
        all_instr[i] = instr;
        i++;
        printf("%s %d\n", instr.name, instr.exec_time);
    }

    // Read processes from definition.txt and store in the ready queue
    FILE *definitionFile = fopen("definition.txt", "r");
    struct ReadyQueue readyQueue;
    readyQueue.front = -1;
    readyQueue.rear = -1;

    while (fscanf(definitionFile, "%s %d %d %s", readyQueue.processes[0].name, &readyQueue.processes[0].priority, &readyQueue.processes[0].arrivalTime, readyQueue.processes[0].classType) == 4)
    {
        // execution time is not assigned yet
        readyQueue.processes[0].currentLine = 0; // Start execution from the first line
        if (strcmp(readyQueue.processes[0].classType, "SILVER") == 0)
        {
            readyQueue.processes[0].timequantum = 80;
        }
        else if (strcmp(readyQueue.processes[0].classType, "GOLD") == 0)
        {
            readyQueue.processes[0].timequantum = 120;
        }
        else if (strcmp(readyQueue.processes[0].classType, "PLATINUM") == 0)
        {
            readyQueue.processes[0].timequantum = 120;
        }
        else
        {
            printf("Invalid class type\n");
            exit(1);
        }

        // Enqueue the process into the ready queue
        enqueue(&readyQueue, readyQueue.processes[0]);
    }

    fclose(definitionFile);

    // Sort the processes array based on priority and arrival time
    qsort(readyQueue.processes, 1, sizeof(struct Process), compareProcesses);
    printf("Sorted processes are :\n");
    for (int i = readyQueue.front; i <= readyQueue.rear; i++)
    {
        printf("%s %d %d %s\n", readyQueue.processes[i].name, readyQueue.processes[i].priority, readyQueue.processes[i].arrivalTime, readyQueue.processes[i].classType);
    }

    // Scheduler
    while (!isReadyQueueEmpty(&readyQueue))
    {
        // Dequeue the next process from the ready queue
        struct Process currentProcess = dequeue(&readyQueue);

        // Use processes list as a ready queue
        if (!isReadyQueueEmpty(&readyQueue))
        {
            // Not the last process
            struct Process nextProcess = readyQueue.processes[readyQueue.front];

            int check_point = nextProcess.arrivalTime;
        }
        else
        {
            // Last process
            printf("Last process\n");
            int check_point = global_time + 10000;
        }

        // Open the file of the current process named currentProcess.name.txt
        char filename[20];
        strcpy(filename, currentProcess.name);
        strcat(filename, ".txt");
        FILE *processFile = fopen(filename, "r");
        while (global_time < check_point)
        {
            // Read the next instruction from the file
            char instructionName[10];
            fscanf(processFile, "%s", instructionName);

            // Find the instruction in the array of instructions
            for (int i = 0; i < 20; i++)
            {
                if (strcmp(all_instr[i].name, instructionName) == 0)
                {
                    // Execute the instruction
                    printf("Executing instruction %s for %d ms\n", instructionName, all_instr[i].exec_time);
                    global_time += all_instr[i].exec_time;

                    // Update the current line of the process
                    currentProcess.currentLine++;

                    // Update the execution time of the process
                    currentProcess.executionTime += all_instr[i].exec_time;

                    // Check if the process is finished
                    if (strcmp(instructionName, "exit") == 0)
                    {
                        // Process is finished
                        printf("Process %s is finished\n", currentProcess.name);

                        // Calculate turnaround time
                        int turnaroundTime = global_time - currentProcess.arrivalTime;
                        printf("Turnaround time for process %s is %d ms\n",
