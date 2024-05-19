// Online C compiler to run C program online
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>  
#include <math.h>    
#define MAX_CORES 4

struct core {
    int id;
    int busy;
    struct processes *queue;
};

struct processes {
    char name;
    int RT; // Remaining Time
    int WT; // Waiting Time
    int TAT; // Turnaround Time
    int CT; // Completion Time (for FCFS)
    
    int id;
    int dur;
    int prio;
    int execu;
    int esp;
    int arr_time;  // New field for arrival time
    
    int stride;
    int pass_value;
    struct processes *next;
};

// Function prototypes
void initialize_cores(struct core cores[], int num_cores);
void assign_process(struct core cores[], int num_cores, struct processes *new_proc, int algorithm_type);

void calculate_stride(struct processes *loc, int process_number);

struct processes *process_input(int id, int dur, int prio,  int arr_time);
int count_processes(struct processes *loc);  
void robbin_round(struct processes *loc, int quantum, int process_number);
void fcfs(struct processes *loc, int process_number);
void sjf(struct processes *loc, int process_number);
void stf(struct processes *loc, int process_number);
void mlfq(struct processes *loc, int process_number);
void list_processes(struct processes *loc);
void free_processes(struct processes *loc);


int main() {
    while(1){
    int num_cores, process_number, quantum, algorithm_type,arr_time;
    char choice;
    struct processes *list_proc = NULL;
    struct core cores[MAX_CORES];
    
    printf("Do you want to use data from a file? (y/n): ");
    scanf(" %c", &choice);
    
    if (choice == 'y' || choice == 'Y') 
    {
        // Read data from a file
        FILE *file = fopen("process_data.txt", "r");
        if (file == NULL) 
        {
            printf("Error opening file.\n");
            return 1;
        }

        fscanf(file, "%d %d", &num_cores, &process_number);
        initialize_cores(cores, num_cores);

        //fscanf(file, "%d", &process_number);

        for (int i = 0; i < process_number; i++) 
        {
            int id, dur, prio;
            fscanf(file, "%d %d %d %d", &id, &dur, &prio, &arr_time);

            struct processes *new_proc = process_input(id, dur, prio, arr_time);
            new_proc->next = list_proc;
            list_proc = new_proc;
        }

        fclose(file);
    } else {
        // Read data manually

    printf("Enter the number of cores: ");
    scanf("%d", &num_cores);

    initialize_cores(cores, num_cores);

    printf("Enter the number of processes: ");
    scanf("%d", &process_number);

   
   
    for (int i = 0; i < process_number; i++) {
        int id, dur, prio;
        printf("Enter details for Process %d:\n", i + 1);
        printf("ID: ");
        scanf("%d", &id);
        printf("Duration: ");
        scanf("%d", &dur);
        printf("Priority: ");
        scanf("%d", &prio);
        printf("Arrival Time: ");
        scanf("%d", &arr_time);

        struct processes *new_proc = process_input(id, dur, prio,arr_time);
        new_proc->next = list_proc;
        list_proc = new_proc;
    }
    }
    
    printf("Enter the time quantum for Round Robin: ");
    scanf("%d", &quantum);

    printf("Select the scheduling algorithm:\n");
    printf("1. Round Robin\n");
    printf("2. FCFS\n");
    printf("3. SJF\n");
    printf("4. STF\n");
    printf("5. MLFQ\n");
    printf("6. Stride\n"); 
    printf("Enter the corresponding number: ");
    scanf("%d", &algorithm_type);

    //calculate_stride(list_proc, process_number);

    // Schedule processes on each core
    for (int i = 0; i < process_number; i++) 
    {
        assign_process(cores, num_cores, list_proc, algorithm_type);
        list_proc = list_proc->next;
    }

    // Display final schedules for each core
    for (int i = 0; i < num_cores; i++) {
        printf("\n\tCore %d Schedule:\n", cores[i].id);
        switch (algorithm_type) {
            case 1: // Round Robin
                robbin_round(cores[i].queue, quantum, process_number / num_cores);
                break;
            case 2: // FCFS
                fcfs(cores[i].queue, process_number / num_cores);
                break;
            case 3: // SJF
                sjf(cores[i].queue, process_number / num_cores);
                break;
            case 4: // STF
                stf(cores[i].queue, process_number / num_cores);
                break;
            case 5: // MLFQ
                mlfq(cores[i].queue, process_number / num_cores);
                break; 
            case 6: // Stride
                calculate_stride(cores[i].queue, process_number / num_cores);
                break;
            default:
                printf("Invalid algorithm type\n");
                break;
        }

        printf("\n");

    }

    // gantt_chart(cores, num_cores, process_number,quantum);
    free_processes(list_proc);
    }
    return 0;
}

void initialize_cores(struct core cores[], int num_cores) {
    for (int i = 0; i < num_cores; i++) {
        cores[i].id = i + 1;
        cores[i].busy = 0;
        cores[i].queue = NULL;
    }
}

// Calculate stride values for each process based on priority
void calculate_stride(struct processes *new_proc, int number_of_processes) {
    // Constants
    const int constant = 100;

    // Arrays to store waiting time, turnaround time, original order, process IDs, and arrival times
    int TOTAL_TIME = 30;
    int wt[10] = {0};
    int temp_wait[10] = {0};
    int tat[10] = {0};
    struct processes *tmp = new_proc;
    //int original_order[10];
    int process_ids[10];
    int arrival_time[10];

    // Initialize arrays
    for (int i = 0; i < number_of_processes; ++i) {
        //original_order[i] = i;
        process_ids[i] = tmp->id;
        arrival_time[i] = tmp->arr_time;
        temp_wait[i] = 0;
        tmp = tmp->next;
    }

    // Calculate strides for each process
    tmp = new_proc;
    while (tmp != NULL) {
        tmp->stride = constant / tmp->prio;
        tmp-> pass_value = 0;
        tmp = tmp->next;
    }

    // Run the scheduler
    for (int time = 0; time < TOTAL_TIME; ++time) {  // Adjust the time limit as needed
        struct processes *selected_process = NULL;
        int min_pass_value = 100;
        //int min_stride = INT_MAX;

        // Find the process with the smallest stride
        tmp = new_proc;
        while (tmp != NULL) {
            // if (tmp->stride < min_stride && tmp->arr_time <= time && tmp->execu > 0) {
            //     min_stride = tmp->stride;
            //     selected_process = tmp;
            // }
            if (tmp->pass_value == 0 ) {

                    if (tmp->stride < min_pass_value) {
                    min_pass_value = tmp->stride;
                    selected_process = tmp;
                    }
                }else{
                    
                    if (tmp->pass_value < min_pass_value && tmp->execu > 0) {
                    min_pass_value = tmp->pass_value;
                    selected_process = tmp;
                    }
            }
            tmp = tmp->next;
        }

        //inc non running procs
        tmp = new_proc;
        while (tmp != NULL) {
            if (tmp->id != selected_process->id) {
               temp_wait[tmp->id]++;
            }else{
                temp_wait[tmp->id] = 0;
            }
            tmp = tmp->next;
        }

        // Run the selected process for a time quantum
        if (selected_process != NULL) {
            int index = selected_process->id - 1;  // Adjust for 0-based array indexing

            // Calculate waiting time and turnaround time
            wt[index] += temp_wait[index];
            tat[index] = wt[index] + selected_process->execu;

            printf("Time %d: Running Process %d\n", time, selected_process->id);
            selected_process->execu--;

            // Update stride after running
            //selected_process->stride = constant / selected_process->prio;
            selected_process->pass_value += selected_process->stride;
        } else {
            printf("Time %d: Idle\n", time);
        }
    }

    // Print results
    printf("\nResults:\n");
    printf("Process\tWaiting Time\tTurnaround Time\n");
    for (int i = 0; i < number_of_processes; ++i) {
        printf("%d\t%d\t\t%d\n", process_ids[i], wt[i], tat[i]);
    }
}

/*void assign_process(struct core cores[], int num_cores, struct processes *new_proc, int algorithm_type) {
    // Find a non-busy core or one with the shortest queue or with the smallest stride
    int min_queue_size = num_cores + 1;
    int selected_core = -1;
    unsigned int min_stride = UINT_MAX;

    for (int i = 0; i < num_cores; i++) 
    {
        if (!cores[i].busy && (cores[i].queue == NULL || cores[i].queue->next == NULL)) 
        {
            selected_core = i;
            break;
        } else if (cores[i].queue != NULL && algorithm_type == 3 && count_processes(cores[i].queue) < min_queue_size) 
        {
            min_queue_size = count_processes(cores[i].queue);
            selected_core = i;
        }else if (algorithm_type == 6 && cores[i].queue->stride < min_stride) {
            min_stride = cores[i].queue->stride;
            selected_core = i;
        }
    }

    if (selected_core != -1) {
        struct processes *proc_copy = process_input(new_proc->id, new_proc->dur, new_proc->prio,new_proc->arr_time);
        proc_copy->next = cores[selected_core].queue;
        cores[selected_core].queue = proc_copy;
        cores[selected_core].busy = 1;
    } else {
        // All cores are busy, add the process to the core with the shortest queue
        // All cores are busy, add the process to the core with the smallest stride
        int min_queue_index = 0;
        int min_stride_index = 0;
        for (int i = 1; i < num_cores; i++) {
            if (count_processes(cores[i].queue) < count_processes(cores[min_queue_index].queue)) {
                min_queue_index = i;
            }else if (cores[i].queue->stride < cores[min_stride_index].queue->stride) {
                min_stride_index = i;
            }
        }

        struct processes *proc_copy = process_input(new_proc->id, new_proc->dur, new_proc->prio,new_proc->arr_time);
        proc_copy->next = cores[min_queue_index].queue;
        cores[min_queue_index].queue = proc_copy;
    }
}*/

void assign_process(struct core cores[], int num_cores, struct processes *new_proc, int algorithm_type) {
    // Find a non-busy core or any core if all are busy
    int selected_core = -1;

    for (int i = 0; i < num_cores; i++) {
        // If current core is not busy, assign the process to it
        if (!cores[i].busy) {
            selected_core = i;
            break;
        }
    }

    if (selected_core == -1) {
        // All cores are busy, add the process to the core with the shortest queue
        int min_queue_index = 0;
        for (int i = 1; i < num_cores; i++) {
            if (count_processes(cores[i].queue) < count_processes(cores[min_queue_index].queue)) {
                min_queue_index = i;
            }
        }
        selected_core = min_queue_index;
    }

    // Assign the process to the selected core
    struct processes *proc_copy = process_input(new_proc->id, new_proc->dur, new_proc->prio, new_proc->arr_time);
    proc_copy->next = cores[selected_core].queue;
    cores[selected_core].queue = proc_copy;
    cores[selected_core].busy = 1;
}

int count_processes(struct processes *loc) {
    int count = 0;
    struct processes *tmp = loc;
    while (tmp != NULL) {
        count++;
        tmp = tmp->next;
    }
    return count;
}

void free_processes(struct processes *loc) {
    struct processes *tmp;
    while (loc != NULL) {
        tmp = loc;
        loc = loc->next;
        free(tmp);
    }
}

void robbin_round(struct processes *loc, int quantum, int process_number) {
    int count, time = 0, remain = process_number, flag = 0;
    int waiting_time = 0, turnaround_time = 0;
    int bt[10], wt[10], tat[10];
    int start_time[10], process_ids[10], arrival_time[10];

    struct processes *tmp = loc;
    for (count = 0; count < process_number; count++) {
        bt[count] = tmp->dur;
        wt[count] = 0;
        tat[count] = 0;
        process_ids[count] = tmp->id;
        arrival_time[count] = tmp->arr_time; // Store the arrival time of each process
        tmp = tmp->next;
    }

    // Sort the processes based on their arrival time
    for (int i = 0; i < process_number - 1; i++) {
        for (int j = i + 1; j < process_number; j++) {
            if (arrival_time[i] > arrival_time[j]) {
                // Swap the arrival time
                int temp = arrival_time[i];
                arrival_time[i] = arrival_time[j];
                arrival_time[j] = temp;

                // Swap the burst time
                temp = bt[i];
                bt[i] = bt[j];
                bt[j] = temp;

                // Swap the process ID
                temp = process_ids[i];
                process_ids[i] = process_ids[j];
                process_ids[j] = temp;
            }
        }
    }

    // Initialize the start time array
    start_time[0] = arrival_time[0];

    printf("\nGantt Chart:\n");
    while (remain != 0) {
        for (count = 0; count < process_number; count++) {
            if (bt[count] > 0 && arrival_time[count] <= time) {
                if (bt[count] > quantum) {
                    // Process is running
                    printf("| Process %d ", process_ids[count]);
                    for (int t = 0; t < quantum; t++) {
                        printf("  ");
                    }
                    time += quantum;
                    bt[count] -= quantum;
                } else {
                    // Process is finishing
                    printf("| Process %d ", process_ids[count]);
                    for (int t = 0; t < bt[count]; t++) {
                        printf("  ");
                    }
                    printf(" ");
                    time += bt[count];
                    wt[count] = time - quantum - arrival_time[count]; // Update waiting time
                    tat[count] = time - arrival_time[count];          // Update turnaround time
                    bt[count] = 0;
                    remain--;
                    turnaround_time += tat[count];
                }
                start_time[count] = time;
            }
        }
    }

    printf("|\n\n");

    printf("Round Robin Scheduling with Details:\n");
    for (int i = 0; i < process_number; i++) {
        printf("\tProcess: %d\tStart time: %d\tFinish: %d\n", process_ids[i], start_time[i], start_time[i] + bt[i]);
    }

    printf("\nTracing State Over Time:\n");
    for (int i = 0; i < process_number; i++) {
        printf("Process %d:\n", process_ids[i]);
        printf("\tArrival Time: %d\n", arrival_time[i]);
        printf("\tStart Running Time: %d\n", start_time[i]);
        printf("\tBurst Time: %d\n", bt[i]);
        printf("\tFinish Time: %d\n", start_time[i] + bt[i]);
    }

    printf("\n\tAverage Waiting Time = %f\n", waiting_time * 1.0 / process_number);
    printf("\tAverage Turnaround Time = %f\n", turnaround_time * 1.0 / process_number);
}

void fcfs(struct processes *loc, int process_number) {
    int waiting_time = 0, turnaround_time = 0, bt[10], wt[10], tat[10];
    int start_time[10];  // Array to store the start time of each process
    int process_ids[10]; // Array to store the real IDs of each process
    int arrival_time[10]; // Array to store the arrival time of each process
    struct processes *tmp = loc;

    for (int i = 0; i < process_number; i++) {
        bt[i] = tmp->dur;
        wt[i] = 0;
        tat[i] = 0;
        process_ids[i] = tmp->id;
        arrival_time[i] = tmp->arr_time; // Store the arrival time of each process
        tmp = tmp->next;
    }

    // Sort the processes based on their arrival time
    for (int i = 0; i < process_number - 1; i++) {
        for (int j = i + 1; j < process_number; j++) {
            if (arrival_time[i] > arrival_time[j]) {
                // Swap the arrival time
                int temp = arrival_time[i];
                arrival_time[i] = arrival_time[j];
                arrival_time[j] = temp;

                // Swap the burst time
                temp = bt[i];
                bt[i] = bt[j];
                bt[j] = temp;

                // Swap the process ID
                temp = process_ids[i];
                process_ids[i] = process_ids[j];
                process_ids[j] = temp;
            }
        }
    }

    // Initialize the start time array
    start_time[0] = arrival_time[0];

    // Calculate waiting time, turnaround time, and start time
    for (int i = 1; i < process_number; i++) {
        start_time[i] = start_time[i - 1] + bt[i - 1];
        wt[i] = start_time[i] - arrival_time[i];
        tat[i] = wt[i] + bt[i];
        waiting_time += wt[i];
        turnaround_time += tat[i];
    }

    // Display Gantt chart
    printf("\nGantt Chart:\n");
    for (int i = 0; i < process_number; i++) {
        printf("| Process %d ", process_ids[i]);
        for (int t = 0; t < bt[i]; t++) {
            printf("  ");
        }
    }
    printf("|\n");

    // Display details including start time
    printf("\nFCFS Scheduling with Details:\n");
    for (int i = 0; i < process_number; i++) {
        printf("\tProcess: %d\tStart time: %d\tFinish: %d\n", process_ids[i], start_time[i], start_time[i] + bt[i]);
    }

    // Tracing state over time
    printf("\nTracing State Over Time:\n");
    for (int i = 0; i < process_number; i++) {
        printf("Process %d:\n", process_ids[i]);
        printf("\tArrival Time: %d\n", arrival_time[i]);
        printf("\tStart Running Time: %d\n", start_time[i]);
        printf("\tBurst Time: %d\n", bt[i]);
        printf("\tFinish Time: %d\n", start_time[i] + bt[i]);
    }

    printf("\n\tAverage Waiting Time = %f\n", waiting_time * 1.0 / process_number);
    printf("\tAverage Turnaround Time = %f\n", turnaround_time * 1.0 / process_number);
}

void sjf(struct processes *loc, int process_number) {
    int wt[10], tat[10], bt[10], i, j, temp;
    struct processes *tmp = loc;
    int original_order[10];
    int process_ids[10];
    int arrival_time[10]; // Array to store the arrival time of each process

    for (i = 0; i < process_number; i++) {
        bt[i] = tmp->dur;
        wt[i] = 0;
        tat[i] = 0;
        original_order[i] = i;
        process_ids[i] = tmp->id;
        arrival_time[i] = tmp->arr_time; // Store the arrival time of each process
        tmp = tmp->next;
    }

    // Sort the processes based on their burst time and arrival time
    for (i = 0; i < process_number - 1; i++) {
        for (j = 0; j < process_number - i - 1; j++) {
            if (bt[j] > bt[j + 1] || (bt[j] == bt[j + 1] && arrival_time[j] > arrival_time[j + 1])) {
                temp = bt[j];
                bt[j] = bt[j + 1];
                bt[j + 1] = temp;

                // Swap original order
                temp = original_order[j];
                original_order[j] = original_order[j + 1];
                original_order[j + 1] = temp;

                // Swap process IDs
                temp = process_ids[j];
                process_ids[j] = process_ids[j + 1];
                process_ids[j + 1] = temp;

                // Swap arrival time
                temp = arrival_time[j];
                arrival_time[j] = arrival_time[j + 1];
                arrival_time[j + 1] = temp;
            }
        }
    }

    int time = 0;
    for (i = 0; i < process_number; i++) {
        wt[i] = time - arrival_time[i];
        time += bt[i];
        tat[i] = wt[i] + bt[i];
    }

    

    // Display details including start time
    printf("\nSJF Scheduling with Tracing:\n");
    for (i = 0; i < process_number; i++) {
        printf("\tProcess: %d\tStart time: %d\tFinish: %d\n", process_ids[original_order[i]], arrival_time[original_order[i]] + wt[original_order[i]], arrival_time[original_order[i]] + tat[original_order[i]]);
    }

    // Tracing state over time
    printf("\nTracing State Over Time:\n");
    for (i = 0; i < process_number; i++) {
        printf("Process %d:\n", process_ids[original_order[i]]);
        printf("\tArrival Time: %d\n", arrival_time[original_order[i]]);
        printf("\tStart Time: %d\n", arrival_time[original_order[i]] + wt[original_order[i]]);
        printf("\tFinish Time: %d\n", arrival_time[original_order[i]] + tat[original_order[i]]);
    }

    int waiting_time = 0, turnaround_time = 0;
    for (i = 0; i < process_number; i++) {
        waiting_time += wt[i];
        turnaround_time += tat[i];
    }

    printf("\n\tAverage Waiting Time = %f\n", waiting_time * 1.0 / process_number);
    printf("\tAverage Turnaround Time = %f\n", turnaround_time * 1.0 / process_number);
}

void stf(struct processes *loc, int process_number) {
    int wt[10], tat[10], bt[10], i, j, temp;
    int remaining_time[10];
    int original_order[10];
    int process_ids[10];
    int arrival_time[10];
    struct processes *tmp = loc;

    for (i = 0; i < process_number; i++) {
        bt[i] = tmp->dur;
        remaining_time[i] = bt[i];
        wt[i] = 0;
        tat[i] = 0;
        original_order[i] = i;
        process_ids[i] = tmp->id;
        arrival_time[i] = tmp->arr_time;
        tmp = tmp->next;
    }

    // Sort the processes based on their remaining time and arrival time
    for (i = 0; i < process_number - 1; i++) {
        for (j = 0; j < process_number - i - 1; j++) {
            if (remaining_time[j] > remaining_time[j + 1] || (remaining_time[j] == remaining_time[j + 1] && arrival_time[j] > arrival_time[j + 1])) {
                temp = bt[j];
                bt[j] = bt[j + 1];
                bt[j + 1] = temp;

                temp = remaining_time[j];
                remaining_time[j] = remaining_time[j + 1];
                remaining_time[j + 1] = temp;

                temp = original_order[j];
                original_order[j] = original_order[j + 1];
                original_order[j + 1] = temp;

                temp = process_ids[j];
                process_ids[j] = process_ids[j + 1];
                process_ids[j + 1] = temp;

                temp = arrival_time[j];
                arrival_time[j] = arrival_time[j + 1];
                arrival_time[j + 1] = temp;
            }
        }
    }

    //int time = 0;
    printf("\nGantt Chart:\n");
    for (i = 0; i < process_number; i++) {
        printf("| Process %d ", process_ids[i]);
        for (j = 0; j < remaining_time[original_order[i]]; j++) {
            printf("  ");
        }
        //time += remaining_time[original_order[i]];
    }
    printf("|\n\n");

    int time = 0;
    for (i = 0; i < process_number; i++) {
        wt[i] = (time > arrival_time[original_order[i]]) ? time - arrival_time[original_order[i]] : 0;
        tat[i] = wt[i] + bt[i];
        time += bt[i];
    }

    printf("\nSTF Scheduling with Tracing:\n");
    for (i = 0; i < process_number; i++) {
        int finish_time = arrival_time[original_order[i]] + tat[original_order[i]];
        if (i < process_number - 1) {
            // Add remaining time of the next process to the finish time
            finish_time += remaining_time[original_order[i + 1]];
        }
        // printf("\tProcess: %d\tStart time: %d\tFinish: %d\n", process_ids[original_order[i]], arrival_time[original_order[i]], finish_time);
        printf("\tProcess: %d\tStart time: %d\tFinish: %d\n", process_ids[original_order[i]], arrival_time[original_order[i]], finish_time);
    }

    printf("\nTracing State Over Time:\n");
    for (i = 0; i < process_number; i++) {
        printf("Process %d:\n", process_ids[original_order[i]]);
        printf("\tArrival Time: %d\n", arrival_time[original_order[i]]);
        printf("\tStart Time: %d\n", arrival_time[original_order[i]]);
        int adjusted_finish_time = tat[original_order[i]];
        if (i == 0) {
        adjusted_finish_time += remaining_time[original_order[i + 1]];
        }
    printf("\tFinish Time: %d\n", arrival_time[original_order[i]] + adjusted_finish_time);
    }

    int waiting_time = 0, turnaround_time = 0;
    for (i = 0; i < process_number; i++) {
        waiting_time += wt[i];
        turnaround_time += tat[i];
    }

    printf("\n\tAverage Waiting Time = %f\n", waiting_time * 1.0 / process_number);
    printf("\tAverage Turnaround Time = %f\n", turnaround_time * 1.0 / process_number);
}
void mlfq(struct processes *loc, int process_number) {
    int choice = 0;
    
        int i, j, k = 0, r = 0, time = 0, tq1 = 5, tq2 = 8, flag = 0, n = process_number;
    char c;
    //printf("Enter no of processes:");
    //scanf("%d", &n);

    struct processes Q1[n], Q2[n], Q3[n], temp; // Added process definition

    struct processes *tmp = loc;
    for (i = 0, c = 'A'; i < n; i++, c++) {
        Q1[i].name = c;
        Q1[i]. dur = tmp->dur;
        Q1[i]. arr_time = tmp->arr_time;
        
        // printf("\nEnter the arrival time and burst time of process %c: ", Q1[i].name);
        // scanf("%d%d", &Q1[i]. arr_time, &Q1[i]. dur);
        Q1[i].RT = Q1[i]. dur; /*save burst time in remaining time for each process*/
        tmp = tmp->next;
    }

    // Sorting processes based on arrival time
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (Q1[i]. arr_time > Q1[j]. arr_time) {
                temp = Q1[i];
                Q1[i] = Q1[j];
                Q1[j] = temp;
            }
        }
    }

    // Start with least arrival time
    time = Q1[0]. arr_time;
    printf("Process in first queue following RR with qt=5");
    printf("\nProcess\t\tRT\t\tWT\t\tTAT\t\t");

    //Q1 RR
    // print proc bt wt tat [dec rt, inc time]
    for (i = 0; i < n; i++) {
        if (Q1[i].RT <= tq1) {
            time += Q1[i].RT; /*from arrival time of first process to completion of this process*/
            Q1[i].RT = 0;       /*sub tq2  hia hia*/
            Q1[i].WT = time - Q1[i]. arr_time - Q1[i]. dur; /* time process has been waiting in the first queue*/
            Q1[i].TAT = time - Q1[i]. arr_time; /*time to execute the process*/
            printf("\n%c\t\t%d\t\t%d\t\t%d", Q1[i].name, Q1[i]. dur, Q1[i].WT, Q1[i].TAT);
        } else {        /*process moves to queue 2 with qt=8*/
            Q2[k].WT = time;
            time += tq1;
            Q1[i].RT -= tq1;
            Q2[k]. dur = Q1[i].RT;
            Q2[k].RT = Q2[k]. dur;
            Q2[k].name = Q1[i].name;
            k = k + 1;
            flag = 1;
        }
    }

    if (flag == 1) {
        printf("\nProcess in second queue following RR with qt=8");
        printf("\nProcess\t\tRT\t\tWT\t\tTAT\t\t");
    }

    //Q2 RR
    for (i = 0; i < k; i++) {
        if (Q2[i].RT <= tq2) {
            time += Q2[i].RT; /*from arrival time of first process +BT of this process*/
            Q2[i].RT = 0;
            Q2[i].WT = time - tq1 - Q2[i]. dur; /* time process has been waiting in the ready queue*/
            Q2[i].TAT = time - Q2[i]. arr_time; /* time to execute the process*/
            printf("\n%c\t\t%d\t\t%d\t\t%d", Q2[i].name, Q2[i]. dur, Q2[i].WT, Q2[i].TAT);
        } else {          /*process moves to queue 3 with FCFS*/
            Q3[r]. arr_time = time;
            time += tq2;
            Q2[i].RT -= tq2;
            Q3[r]. dur = Q2[i].RT;
            Q3[r].RT = Q3[r]. dur;
            Q3[r].name = Q2[i].name;
            r = r + 1;
            flag = 2;
        }
    }

    if (flag == 2) {
        printf("\nProcess in third queue following FCFS ");
    }

    //Q3 FCFS
    for (i = 0; i < r; i++) {
        if (i == 0)
            Q3[i].CT = Q3[i]. dur + (time - tq1 - tq2);     //bt + const    /*const: time spent in first two queues*/
        else
            Q3[i].CT = Q3[i - 1].CT + Q3[i]. dur;             /*const + bt*/
    }

    for (i = 0; i < r; i++) {
        Q3[i].TAT = Q3[i].CT;
        Q3[i].WT = Q3[i].TAT - Q3[i]. dur;
        printf("\n%c\t\t%d\t\t%d\t\t%d\t\t", Q3[i].name, Q3[i]. dur, Q3[i].WT, Q3[i].TAT);
    }

    
    
}
void list_processes(struct processes *loc) {
    // Function to print the list of processes (same as before)
    struct processes *tmp = loc;
    printf("\n\n\t\t\tList of processes\n\n");
    while (tmp != NULL) {
        printf("\tProcess: %d\tPriority: %d\tDuration: %d\n", tmp->id, tmp->prio, tmp->dur);
        tmp = tmp->next;
    }
    printf("\n");
}

struct processes *process_input(int id, int dur, int prio, int arr_time) {
    struct processes *loc;
    loc = (struct processes *)malloc(sizeof(struct processes));
    if (loc == NULL) {
        printf("Allocation error.\nEnd of execution\n");
        exit(1);
    }
    loc->id = id;
    loc->dur = dur;
    loc->prio = prio;
    loc->execu = 0;
    loc->esp = 0;
    loc->arr_time = arr_time;  // Set the arrival time
    loc->next = NULL;
    return loc;
}