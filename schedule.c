
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
    int check_point = 0;
    // Scheduler
    while (!isReadyQueueEmpty(&readyQueue))
    {  //CONTEXT SWİTCH OCCURED 
        global_time += 10; 
        // Dequeue the next process from the ready queue
        int is_platinum=0;
        struct Process currentProcess = dequeue(&readyQueue);
        if (strcmp(currentProcess.classType, "PLATINUM") == 0)
        {
            is_platinum=1;
        }

        // Use processes list as a ready queue
        if (!isReadyQueueEmpty(&readyQueue))
        {
            // Not the last process
            struct Process nextProcess = readyQueue.processes[readyQueue.front];

            check_point = nextProcess.arrivalTime; //decide at this point what to do (preempt or not)
        }
        else
        {
            // Last process
            printf("Last process\n");
            check_point = global_time + 10000;
        }
        //open the file of current process named currentProcess.name.txt
        char filename[20];
        strcpy(filename, currentProcess.name);
        strcat(filename, ".txt");
        FILE* processFile = fopen(filename, "r");// begin to read from current line of the process start from where we left 
            int preemption=0; 
            while (!preemption){
            char instructionName[10];
            fscanf(processFile, "%s", instructionName); //take the instruction at the line currentProcess.currentLine

            //find the instruction in the array of instructions
            for (int i = 0; i < 20; i++) {
                if (strcmp(all_instr[i].name, instructionName) == 0) { 

                    //execute the instruction
                    if (all_instr[i].exec_time > currentProcess.timequantum) {//we can preempt plaitnums if they exceed time quantum
                        //preempt the process
                        preemption=1;
                        //update the current line of the process
                        currentProcess.currentLine++;
                        //update the execution time of the process
                        currentProcess.executionTime += currentProcess.timequantum;
                        //update the execution time of the instruction
                        all_instr[i].exec_time -= currentProcess.timequantum;
                        //update the timequantum of the process
                        currentProcess.timequantum = 0;
                        //enqueue the process into the ready queue
                        enqueue(&readyQueue, currentProcess); //enqueue the process into the ready queue
                        //sort the ready queue
                        qsort(readyQueue.processes, 1, sizeof(struct Process), compareProcesses);
                        //update the global time
                        global_time += currentProcess.timequantum;
                        //update the timequantum of the process
                        printf("Preempting process %s at %d ms\n", currentProcess.name, global_time);

                    }
                    else if(global_time+all_instr[i].exec_time>check_point && !is_platinum){ //do not preempt platinum
                        //update the current line of the process
                        preemption=1;
                        currentProcess.currentLine++;
                        //update the execution time of the process
                        currentProcess.executionTime += check_point-global_time;
                        //update the execution time of the instruction
                        all_instr[i].exec_time -= check_point-global_time; //for example if it was 100 and we preempted after 20ms , the instr will continue from 80
                        //enqueue the process into the ready queue
                        enqueue(&readyQueue, currentProcess); //enqueue the process into the ready queue
                        //sort the ready queue
                        qsort(readyQueue.processes, 1, sizeof(struct Process), compareProcesses);
                        //update the global time
                        global_time += check_point-global_time;
                        printf("Preempting process %s at %d ms\n", currentProcess.name, global_time);

                    }
                    else{
                        //current instruction is totally safe to execute,no preemption , also if the process is a platinum its all instruction will be in here 
                    printf("Executing instruction %s for %d ms\n", instructionName, all_instr[i].exec_time);
                    global_time += all_instr[i].exec_time;
                    //update the current line of the process
                    currentProcess.currentLine++;
                    //update the execution time of the process
                    currentProcess.executionTime += all_instr[i].exec_time;
                    }
                    //check if the process is finished
                    if (strcmp(instructionName,"exit")==0) { //or instructionName is exit 
                        //process is finished
                        printf("Process %s is finished\n", currentProcess.name);
                        //calculate turnaround time
                        int turnaroundTime = global_time - currentProcess.arrivalTime;
                        printf("Turnaround time for process %s is %d ms\n", currentProcess.name, turnaroundTime);
                        //calculate waiting time
                        int waitingTime = turnaroundTime - currentProcess.executionTime;
                        printf("Waiting time for process %s is %d ms\n", currentProcess.name, waitingTime);

                    }
                }
                }
            }

        



        // Implement the execution logic here based on the given instructions

        // Update the process information (e.g., current line, priority, etc.) after execution

        // Check if the process is finished or preempted

        // If a new process arrives during execution, add it to the array and re-sort
    }

    return 0;
}

        
