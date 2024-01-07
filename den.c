
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int global_time = 0;
int total_wait=0;
int total_turnaround=0;
int process_count=0;
//TODO 
//all processes must have their own all_instr so that they can store remaining time for the spesific instruction they are executing
//close the process files after premption so that you can opean them again 
//implement class convertion after a number of timequantum burst (for example 3 timequantum burst for silver to gold)
// Define the instruction execution times
//fazladan context switch time ekliyor olabilir , enqueue yaptıktan sonra qsort edip yeni bir tane alıyor aldığı aynı process ise eklemesin ???
//eğer aynı priority olanlar arasında time quantum olsun istiyorsan p harfi önüne bir fazlasını ekle string comparisonda aşağıya gitsin o 
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
    int remaining_burst;
    int preempted; //if the process is preempted then it will be 1
    struct Instruction *pr_instruct; //array of instructions for each process to store remaining time for each instruction
};
//initiate the values 
struct Process init_process(struct Process process){
    process.executionTime=0;
    process.preempted=0;
    process.remaining_burst=0;
    process.currentLine=1;
    process.priority=0;
    process.timequantum=0;
    return process;
}

// Comparison function for sorting processes based on priority and arrival time
int compareProcesses(const void *a, const void *b) {
    const struct Process *p1 = (const struct Process *)a;
    const struct Process *p2 = (const struct Process *)b;
    // least arrival time first , if arrival times are equal then least priority first , if priorities are equal then least name first
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
        return 1; //higher priority first
    }
    else if (p1->priority > p2->priority)
    {
        return -1;
    }
    else
    {
        return strcmp(p1->name, p2->name);
    }
}

int compareProcesses_same_priority(const void *a, const void *b) {
    const struct Process *p1 = (const struct Process *)a;
    const struct Process *p2 = (const struct Process *)b; //arrival time is not important here
    if (p1->priority < p2->priority)
    {
        return 1; //higher priority first
    }
    else if (p1->priority > p2->priority)
    {
        return -1;
    }
    else
    {
        return 0;
    }

}


struct ReadyQueue_same_priority
{
    struct Process processes[50]; //hold processes with same priority
    int front;
    int rear;
};
// Structure for the ready queue
struct ReadyQueue //this structure must be changed to hold queues that store processes with same priority,queue may be a single elemnt or more than one
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
    if (queue->front == -1)  //if the queue is empty
    {
        queue->front = 0; //set the front to 0
    }
    queue->rear++; 
    queue->processes[queue->rear] = process; //add the process to the rear of the queue
}

// Function to dequeue a process from the ready queue
struct Process dequeue(struct ReadyQueue *queue)
{
    struct Process process = queue->processes[queue->front];
    if (queue->front == queue->rear) //if there is only one element in the queue
    {
        queue->front = -1;
        queue->rear = -1; //reset the queue
    }
    else
    {
        queue->front++; //move the front to the next element
    }
    return process;
}
int main()
{
    // Read instruction times
    struct Instruction instr;
    // exactly 20 instructions
    struct Instruction all_instr[21];

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
    FILE *definitionFile = fopen("Example_Inputs_Outputs_v3/def8.txt", "r");
    struct ReadyQueue readyQueue;
    //also stor the processes in an array to sort them later
    struct Process process_array[50];

    readyQueue.front = -1;
    readyQueue.rear = -1;
    int num_of_proc=0;
    i = 0;
    while (fscanf(definitionFile, "%s %d %d %s", readyQueue.processes[i].name, &readyQueue.processes[i].priority, &readyQueue.processes[i].arrivalTime, readyQueue.processes[i].classType) == 4)
    {
        // execution time is not assigned yet
        readyQueue.processes[i].currentLine = 1; // Start execution from the first line,1 yaptım öyle devam ettim sonra 
        readyQueue.processes[i].pr_instruct = all_instr; //assign the instruction array to the process later change during preemption
        readyQueue.processes[i].preempted=0;
        readyQueue.processes[i].executionTime = 0;
        printf("this issss the priority %d\n",readyQueue.processes[i].priority);
        if (strcmp(readyQueue.processes[i].classType, "SILVER") == 0)
        {
            readyQueue.processes[i].timequantum = 80;
            readyQueue.processes[i].remaining_burst = 3;
        }
        else if (strcmp(readyQueue.processes[i].classType, "GOLD") == 0)
        {
            readyQueue.processes[i].timequantum = 120;
            readyQueue.processes[i].remaining_burst = 5;
        }
        else if (strcmp(readyQueue.processes[i].classType, "PLATINUM") == 0)
        {
            readyQueue.processes[i].timequantum = 120;
            readyQueue.processes[i].remaining_burst = 8000;
        }
        else
        {
            printf("Invalid class type\n");
            exit(1);
        }

        // Enqueue the process into the ready queue
        enqueue(&readyQueue, readyQueue.processes[i]);
        process_array[i]=readyQueue.processes[i];
        i++;
        num_of_proc=i;
        process_count=i;
    }

    fclose(definitionFile);
    printf("Processes are :\n");
    for (int i = readyQueue.front; i <= readyQueue.rear; i++)
    {
        printf("%s %d %d %s\n", readyQueue.processes[i].name, readyQueue.processes[i].priority, readyQueue.processes[i].arrivalTime, readyQueue.processes[i].classType);
    }

    // Sort the processes array based on priority and arrival time
    qsort(readyQueue.processes, num_of_proc, sizeof(struct Process), compareProcesses);
    //also sort the array with respect to arrival time and priority,actually only arrival is enough 
    qsort(process_array, num_of_proc, sizeof(struct Process), compareProcesses);
    printf("Sorted processes are :\n");
    for (int i = readyQueue.front; i <= readyQueue.rear; i++)
    {
        printf("%s %d %d %s\n", readyQueue.processes[i].name, readyQueue.processes[i].priority, readyQueue.processes[i].arrivalTime, readyQueue.processes[i].classType);
    }
    int check_point = 0;
    // Scheduler
    //collect the processes that comes at time zero and put them into the ready queue
    struct ReadyQueue same_priority_queue;
    same_priority_queue.front=-1;
    same_priority_queue.rear=-1;
    int current_pr=0;
    struct ReadyQueue preempted_queue;
    preempted_queue.front=-1;
    preempted_queue.rear=-1;
    
    while (!isReadyQueueEmpty(&readyQueue) || !isReadyQueueEmpty(&same_priority_queue) ) //while there is a process in one of them 
    {  //CONTEXT SWİTCH OCCURED 
    printf("is ready queue empty %d\n",isReadyQueueEmpty(&readyQueue));
    printf("is sameeee priority queue empty %d\n",isReadyQueueEmpty(&same_priority_queue));
    //if same priority queue is empty but ready queue is not then add the first process to the same_priority_queue
    if (isReadyQueueEmpty(&same_priority_queue) && !isReadyQueueEmpty(&readyQueue)){
        printf("same priority queue is empty but ready queue is not\n");
        enqueue(&same_priority_queue, dequeue(&readyQueue));
        printf("same artık dolu");
        current_pr+=1; //bunu azaltmayı unutma cıkıs yaoanlar icin 
        struct Process switchedProcess=same_priority_queue.processes[same_priority_queue.front];
        global_time=switchedProcess.arrivalTime;
        printf("new process from future : %s\n",switchedProcess.name);
    }

    //before context switch, check if there is a new process arrived add it to the ready queue and sort it
        //int a= readyQueue.front;
        while (readyQueue.processes[readyQueue.front].arrivalTime <= global_time && readyQueue.front <= readyQueue.rear && readyQueue.front!=-1)
        {

            printf("hello new incomer %d , global time is : %d\n",readyQueue.processes[readyQueue.front].name, global_time);
            enqueue(&same_priority_queue, dequeue(&readyQueue));
            current_pr+=1;
            //qsort(same_priority_queue.processes, same_priority_queue.rear-same_priority_queue.front+1, sizeof(struct Process), compareProcesses_same_priority);
            //after dequeue this process is moved and removed from ready queue
            //remove the incomers from ready queue and add them to same_priority_queue

            //a++;
        }
        //readyQueue.front=a; 
        //readyQueue.front=a; //a son kez bir fazladan artıyor o yğzden
        
        if (preempted_queue.front!=-1){
            printf("preempted queue is not empty\n");
            //if there is a preempted process then enqueue it to the same_priority_queue
            enqueue(&same_priority_queue,dequeue(&preempted_queue));
            //qsort(same_priority_queue.processes, same_priority_queue.rear-same_priority_queue.front+1, sizeof(struct Process), compareProcesses_same_priority);
            
        }
        //before sorting print same priority queue
        printf("before sorting same priority queue is :\n");
        for (int i = same_priority_queue.front; i <= same_priority_queue.rear; i++)
        {
            printf("%s %d %d %s\n", same_priority_queue.processes[i].name, same_priority_queue.processes[i].priority, same_priority_queue.processes[i].arrivalTime, same_priority_queue.processes[i].classType);
        }
        qsort(same_priority_queue.processes+same_priority_queue.front, same_priority_queue.rear-same_priority_queue.front+1, sizeof(struct Process), compareProcesses_same_priority);
        printf("sortingde kullanılan current pr %d\n",current_pr);
        printf("rear and front of same priority queue is %d %d\n",same_priority_queue.rear,same_priority_queue.front);
        //print same priority queue processes array 


        //print the contents of the same_priority_queue
        if (same_priority_queue.front!=-1){
            printf("same priority queue iso :\n");
            for (int i = same_priority_queue.front; i <= same_priority_queue.rear; i++)
            {
                printf("%s %d %d %s\n", same_priority_queue.processes[i].name, same_priority_queue.processes[i].priority, same_priority_queue.processes[i].arrivalTime, same_priority_queue.processes[i].classType);
            }
        }
        else{
            printf("same priority queue is empty\n");
        }
        global_time += 10;  //bunu bazen arttırmaya gerek olmayabilir 
        printf("CONTEXT SWITCH \n");
        // Dequeue the next process from the ready queue,this may be also another ready queue storing same priority processes in that case round robin scheduling will be applied to this whole queue 
        //if length of the queue is 1 then no need to apply round robin scheduling
        //if length of the queue is more than 1 then apply round robin scheduling
        
        int is_platinum=0;
        //if there is more than one process with the same priority then store all of them in a queue and apply round robin scheduling to this queue
        struct Process currentProcess = dequeue(&same_priority_queue);    
         //ensure that some processes are removed and same pri becomes empty 
         //if the dequeued process is a preempted one enqueue it again to the same_priority_queue
         //if (currentProcess.preempted==1){
           //  enqueue(&same_priority_queue,currentProcess);
             //currentProcess.preempted=0;
             //currentProcess=dequeue(&same_priority_queue);
         //}

         //enqueue the preempted process again to the same_priority_queue

         
        int time_limit=currentProcess.timequantum;
        //struct Instruction* all_instr = currentProcess.pr_instruct;
        printf("DEQUEUED PROCESS %s at %d ms\n", currentProcess.name, global_time);
        if (strcmp(currentProcess.classType, "PLATINUM") == 0)
        {
            is_platinum=1;
        }
        // Use processes list as a ready queue
        
        //open the file of current process named currentProcess.name.txt
        char filename[20];
        strcpy(filename, currentProcess.name);
        strcat(filename, ".txt");
        FILE* processFile = fopen(filename, "r");// begin to read from current line of the process start from where we left 
        int lineNumber = 1;
        
        while (lineNumber < currentProcess.currentLine){
            char temp[100];
            if (fgets(temp, sizeof(temp), processFile) == NULL){ //change the pointer to the line we want to read
                fprintf(stderr, "Error reading process file %s this is currentline %d", filename,currentProcess.currentLine);
                exit(EXIT_FAILURE);
            }
            lineNumber++;
        }
        

        int preemption=0; 
        while (!preemption){ //for each processes each instruction, for platinum this loop run continually until the end of the process
            char instructionName[10];
            fscanf(processFile, "%s", instructionName); //take the instruction at the line currentProcess.currentLine
            printf("Acurrent line is %d , current instr is %s \n",currentProcess.currentLine,instructionName);

            //find the instruction in the array of instructions
            for (int i = 0; i < 21; i++) {
                if (strcmp(all_instr[i].name, instructionName) == 0) { 
                    printf("found instruction %s\n", instructionName);
                    //execute the found instruction
                    //take the single instrution that is smaller or euqual to the time quantum,execute it , then check if execute the next one or context switch 
                    //current instruction is totally safe to execute,no preemption , also if the process is a platinum its all instruction will be in here 
                    printf("Executing  safe instruction or platinum %s for %d ms\n", instructionName, all_instr[i].exec_time);
                    printf("is it platinum? %d\n",is_platinum);
                    global_time += all_instr[i].exec_time;
                    //update the current line of the process
                    currentProcess.currentLine++;
                    //update the execution time of the process
                    currentProcess.executionTime += all_instr[i].exec_time; 
                    printf("for process %s execution time is %d\n",currentProcess.name,currentProcess.executionTime);
                    if (!is_platinum) {
                        time_limit-=all_instr[i].exec_time; 
                        //everytime time_limit of the process falls below zero, reduce the remaining burst by one 
                    }
                    //this ensures that a context switch will occur after time quantum
                
                    //check if the process is finished
                    printf("finish check this is the instr name : %s new line num is %d\n",instructionName,currentProcess.currentLine);
                    if(strcmp(instructionName,"exit")==0) { //or instructionName is exit 
                        //process is finished
                        preemption=1; //yeni ekledim irem 7 ocak 
                        printf("Process %s is finished\n", currentProcess.name);
                        //calculate turnaround time
                        int turnaroundTime = global_time - currentProcess.arrivalTime;
                        total_turnaround+=turnaroundTime;
                        printf("Turnaround time for process %s is %d ms\n", currentProcess.name, turnaroundTime);
                        //calculate waiting time
                        current_pr-=1; //bir tane process azaldı
                        int waitingTime = turnaroundTime - (currentProcess.executionTime); //+1 i sildim 
                        total_wait+=waitingTime;
                        printf("GLOBAL and turnaround and execution %d %d %d \n",global_time,turnaroundTime,currentProcess.executionTime);
                        printf("Waiting time for process %s is %d ms\n", currentProcess.name, waitingTime);
                        num_of_proc--; //bir tane process artık azaldı qsort için daha az sayı verilemli 
                        //delete from the ready queue
                        for (int i = same_priority_queue.front; i <= same_priority_queue.rear; i++)
                        {
                            if (strcmp(same_priority_queue.processes[i].name, currentProcess.name) == 0)
                            {
                                for (int j = i; j < same_priority_queue.rear; j++)
                                {
                                    same_priority_queue.processes[j] = same_priority_queue.processes[j + 1];
                                }
                                same_priority_queue.rear--;
                                break;
                            }
                        }

                        

                    }
                    //check if 
                } // instruction bitti sonrakine geçsin mi yoksa preemption mı olacak
            }
            //instruction arayan for bitti,eğer platinium loop devam etmeli diğer instruction için ama eğer
            // if is_platinum preempiton=0;
            if (strcmp(instructionName,"exit")==0){
                //break;
                preemption=1;
                //no enqueue for the process
            }
            else {
                //if it is not finished check the ready queue if there any incoming and higher priority process,if there is then preempt
                //if there is no incoming process then continue with the next instruction as this process it the highest in the same_process_queue
                if (!isReadyQueueEmpty(&readyQueue) && !is_platinum) //ya readyqueden gelir preemption ya da aynı priorityli bir sonraki process gelir zaten var olan 
                { //belki not is platinum klenmlei 
                 // Not the last process
                    struct Process nextProcess = readyQueue.processes[readyQueue.front];
                    int i = readyQueue.front;
                    while (nextProcess.arrivalTime <= global_time && i<readyQueue.rear && readyQueue.rear!=-1) { //eğer eşitse aşağıda çıkarılacak
                        printf("next process is  %s\n",nextProcess.name);
                        //higher pri gelmiş olabilir ama time_limit dolmamış olabilir bu durumda prempt edilirse tekrar schedule edildiğinde 
                        //time_limit yine time_quantumdan başlar şu an time_quantumu azaltmıyorum,örneğin 120 iken 100 de preempt edilirse bir sonrakine 20de değil 120 de burst u bir azalır 
                        //check if there is a higher priority process
                        if (currentProcess.priority < nextProcess.priority) { //dikkat burada p1 in yanına p2 gelirse sonraki cs yine p1 den başlar isimden dolayı 
                            //dışarıdan gelen aynı oriority ise preempt edemez time limit dolmalı etmesi için 
                            printf("arrival of a hşgher pri from outside \n");
                            preemption=1; //yeni ekledim irem 7 ocak
                            //buraya eklemiyorum 
                            currentProcess.remaining_burst--; //time quantumu bitmemiş olsa da burst azaltıyorum 
                            enqueue(&preempted_queue, currentProcess); //enqueue the process into the ready queue
                        //store the line of the process
                        //update the current line of the process
                        //currentProcess.currentLine++; yine buradan devam edeceği için arttırma line ı
                        //NOT : bu durumda p1 den sonra aynı priority ile p2 gelirse tekrar p1 den devam eder ama context switch ekler araya 
                            break;
                        }
                        if(currentProcess.priority==nextProcess.priority && time_limit<=0){
                            printf("arrival of same pri from external preempt this is new comer : %s\n",nextProcess.name);
                            preemption=1; //yeni ekledim irem 7 ocak
                            currentProcess.remaining_burst--; //time quantumu bitmemiş olsa da burst azaltıyorum
                            currentProcess.preempted=1;
                            enqueue(&preempted_queue, currentProcess); //enqueue the process into the ready queue
                            break;
                        }
                        i++;
                        nextProcess = readyQueue.processes[i];
                        //check all incoming processes not just the immediate next one 
                        //update the current line of the process
                    }
                }
                if (isReadyQueueEmpty(&preempted_queue)){ //if it is empty we may check the last preemption possibility 
                    if (!isReadyQueueEmpty(&same_priority_queue) && time_limit<=0){ //eğer time quantum aşıldıysa aynı pri gelebilir
                        //if the next process in the same_priority_queue has the same priority then continue with the next instruction
                        //to achieve this increment the Ascıı value of the name of the process P1 to (P+1)1 so that it will be sorted after any potential P(x)
                        struct Process nextProcess = same_priority_queue.processes[same_priority_queue.front];
                        int i = same_priority_queue.front;
                        printf("acaba beni bekleyen aynı priorityli var mı\n");
                        printf("this is same pri queue before deciding to siwtch \n");
                        for (int i = same_priority_queue.front; i <= same_priority_queue.rear; i++){
                            printf("%s %d %d %s\n", same_priority_queue.processes[i].name, same_priority_queue.processes[i].priority, same_priority_queue.processes[i].arrivalTime, same_priority_queue.processes[i].classType);
                        }
                        
                            if (nextProcess.priority == currentProcess.priority) { //ama bunu yapmak için ayrıca current processin time quantuma ulasmıs olması lazım 
                                //burayı bir kere kontrol etme loopta et def8 düzelir 
                                preemption=1;
                                currentProcess.preempted=1;
                                printf("same priority process %s is coming instead\n",nextProcess.name);
                                currentProcess.remaining_burst--;
                                //currentProcess.name[0]=currentProcess.name[0]+1;
                                printf("new process name is %s\n",currentProcess.name);
                                enqueue(&preempted_queue, currentProcess); //enqueue the process into the ready queue
                                //ya da bu yöntem yerine priority değerini 0.1 arttır veya 0.05 arttır ? 
                                ///increment the Ascıı value of the name of the process P1 to (P+1)1 so that it will be sorted after any potential P(x)
                                //for example P1 to Q1 , P2 to Q2 , P3 to Q3
                            printf("new process name is %s\n",currentProcess.name);
                                //ensure it is the least highest between the same priority processes
                                //printf("new process name is %s\n",currentProcess.name);

                            }
                    }
            //continue;
                }
            }
            //remaining burst control 
            if (currentProcess.remaining_burst == 0) {
                printf("burst is finished\n");
                //change the class of the process
                if (strcmp(currentProcess.classType, "SILVER") == 0) {
                    printf("SILVER to GOLD\n");
                    strcpy(currentProcess.classType, "GOLD");
                    currentProcess.timequantum = 120;
                    currentProcess.remaining_burst = 5;
                }
                else if (strcmp(currentProcess.classType, "GOLD") == 0) {
                    printf("GOLD to PLATINUM\n");
                    strcpy(currentProcess.classType, "PLATINUM");
                    currentProcess.timequantum = 120;
                    currentProcess.remaining_burst = 8000;
                }
            }
            

        }//while loop for each instruction of the process time limit ve preemption check ediyor 
        
        
        // Update the process information (e.g., current line, priority, etc.) after execution

        // Check if the process is finished or preempted

        // If a new process arrives during execution, add it to the array and re-sort

        //process is either preempted or finished , 
    }
    //print with 2 decimal points

    printf("Average waiting time is %.2f ms\n", (float)total_wait /process_count);
    printf("this is total wait %d\n",total_wait);
    printf("this is total turnaround %d\n",total_turnaround);
    printf("this is process count %d\n",process_count);

    printf("Average turnaround time is %.2f ms\n", (float)total_turnaround / process_count);

    return 0;
}


        
