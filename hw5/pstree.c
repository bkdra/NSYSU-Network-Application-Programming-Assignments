#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>
#define MAX_PROCESS_COUNT 1024

typedef struct {
    int pid;
    int ppid;
    char name[256];
}Process;

void printTree(int level, Process* relations[MAX_PROCESS_COUNT][256], int child_num[MAX_PROCESS_COUNT], int pid, int pids[MAX_PROCESS_COUNT], int pids_index);
int main()
{
    DIR *dir;
    struct dirent *entry;
    char* path, parent_path;
    int pids[MAX_PROCESS_COUNT];
    int pids_index = 0;
    Process* relations[MAX_PROCESS_COUNT][256] = {NULL}; // store the parent-child relationships in an adjacency matrix
    // example:
    // relations[0][1] store the 0th process's 1st child process
    // where 0th process's pid is stored in pids[0]
    // pids_index records the index for pids, and also can be seen as the number of process which has child.

    int child_num[MAX_PROCESS_COUNT] = {0};
    // child_num[i] store the number of child process for the process whose pid is stored in pids[i]
    Process* init_proc[10] = {NULL};
    // store the init process separately since it has no parent, and we can start to print the process tree from the init process
    int init_proc_index = 0;

    dir = opendir("/proc");

    if(dir == NULL)
    {
        perror("opendir");
        return EXIT_FAILURE;
    }

    while((entry = readdir(dir)) != NULL)
    {
        int pid = atoi(entry->d_name);
        int is_orphan = 0;
        if(pid > 0)
        {
            // construct the path to the status file of the process
            path = strdup("/proc/");
            strcat(path, entry->d_name);
            strcat(path, "/status");
            FILE *fp = fopen(path, "r");

            if(fp != NULL)
            {
                char line[256];
                Process* currentProcess = malloc(sizeof(Process));
                currentProcess->pid = pid;
                currentProcess->ppid = -1;

                while(fgets(line, sizeof(line), fp)) // read one line at a time
                {
                    if(strncmp(line, "Name:", 5) == 0) // check whether the start of the line is "Name:"
                        sscanf(line, "Name: %s", currentProcess->name); // extract the process name

                    else if(strncmp(line, "PPid:", 5) == 0) // check whether the start of the line is "PPid:"
                    {
                        sscanf(line, "PPid: %d", &currentProcess->ppid); // extract the parent pid

                        char ppid_str[10];
                        snprintf(ppid_str, sizeof(ppid_str), "%d", currentProcess->ppid);
                        parent_path = strdup("/proc/");
                        strcat(parent_path, ppid_str);
                        if(currentProcess->ppid != 0 && access(parent_path, R_OK) != 0) // check whether the parent process exists, if not, mark the process as orphan (HW's request)
                        {
                            is_orphan = 1;
                            printf("Process %d is an orphan process since its parent process %d does not exist.\n", currentProcess->pid, currentProcess->ppid);
                        }

                        if(currentProcess->ppid == 0)
                        {
                            init_proc[init_proc_index++] = currentProcess; // store the init process separately since it has no parent
                        }
                        break;
                    }

                    if(strncmp(line, "TracerPid:", 10) == 0)
                        break;
                }
                fclose(fp);

                if(is_orphan)
                {
                    is_orphan = 0;
                    free(currentProcess);
                    continue;
                }
                    
                // if the process has parent, mark the relationship between the process and its parent in the adjacency matrix
                // 做法: 檢查該Process的parent是否在pids中，如果沒有，就存進去。並讓tmp_index作為其parent的pid在pids陣列中儲存的位置。
                // 接著把自己作為其parent的child，存進relations內。
                if(currentProcess->ppid != -1 && currentProcess->ppid != 0) // check whether this process has parent
                {
                    int tmp_index = -1;
                    for(int j = 0; j < pids_index; j++)
                    {
                        if(pids[j] == currentProcess->ppid)
                        {
                            tmp_index = j;
                            break;
                        }
                    }
                    if(tmp_index == -1)
                    {
                        tmp_index = pids_index;
                        pids[pids_index++] = currentProcess->ppid;
                    }
                    relations[tmp_index][child_num[tmp_index]] = currentProcess; // mark the relationship in the adjacency matrix
                    child_num[tmp_index]++;
                }
            }
        }
    }
    // print the root of process tree and run recursive function to print the process tree
    for(int i = 0; i < init_proc_index; i++)
    {
        Process* initProcess = init_proc[i];
        int printLen;
        char* content;
        char pid_str[10];
        snprintf(pid_str, sizeof(pid_str), "%d", initProcess->pid);
        content = strdup(initProcess->name);
        strcat(content, ",[");
        strcat(content, pid_str);
        strcat(content, "]");
        printLen = strlen(content);

        printf("%s", content); // print the pid with indentation based on the level in the tree
        free(content);
        for(int j=0; j < 30-printLen; j++)
            printf("-");
        printTree(1, relations, child_num, initProcess->pid, pids, pids_index); // print the subtree of the init process
        printf("\n");
    }
}

void printTree(int level, Process* relations[MAX_PROCESS_COUNT][256], int child_num[MAX_PROCESS_COUNT], int pid, int pids[MAX_PROCESS_COUNT], int pids_index)
{
    // find all of process's child for this specific pid
    int ppid_index = -1;
    int curr_pid_index = -1;

    for(int i = 0; i < pids_index; i++)
    {
        if(pids[i] == pid)
        {
            ppid_index = i;
            break;
        }
    }

    for(int i = 0; i < child_num[ppid_index]; i++)
    {
        curr_pid_index = -1;

        // if current process has siblings, print a new line and indentation before printing the sibling process
        if(i!=0)
        {
            printf("\n");
            for(int j=0; j < level; j++)
                printf("                              "); // print indentation based on the level in the tree
        }
        
        // find current process's index in pids, which will be used to find the number of child of current process
        for(int j=0;j<pids_index;j++)
        {
            if(pids[j] == relations[ppid_index][i]->pid)
            {
                curr_pid_index = j;
                break;
            }
        }

        int printLen;
        char* content;
        char pid_str[10];
        snprintf(pid_str, sizeof(pid_str), "%d", relations[ppid_index][i]->pid);

        content = strdup(relations[ppid_index][i]->name);
        strcat(content, ",[");
        strcat(content, pid_str);
        strcat(content, "]");
        printLen = strlen(content);

        printf("-");
        printf("%s", content); // print the pid with indentation based on the level in the tree
        free(content);

        // if the current process has child, print the subtree of the current process
        if(curr_pid_index != -1 && child_num[curr_pid_index] != 0)
        {
            for(int j=0; j < 29-printLen; j++)
                printf("-");
            printTree(level + 1, relations, child_num, relations[ppid_index][i]->pid, pids, pids_index); // recursively print the subtree of the child process
        }
        
        free(relations[ppid_index][i]);
    }
}