#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
    DIR *dir;
    struct dirent *entry;
    char* path;
    char* filename;

    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
    filename = argv[1];

    dir = opendir("/proc");
    if(dir == NULL)
    {
        perror("opendir");
        return EXIT_FAILURE;
    }

    while((entry = readdir(dir)) != NULL)
    {
        int pid = atoi(entry->d_name);
        if(pid>0)
        {
            path = strdup("/proc/");
            strcat(path, entry->d_name);
            strcat(path, "/fd/");

            if(access(path, R_OK) == 0)
            {
                DIR *fd_dir = opendir(path);
                if(fd_dir == NULL)
                {
                    perror("opendir");
                    printf("Failed to open path : /proc/%d/fd\n", pid);
                }
                if(fd_dir != NULL)
                {
                    struct dirent *fd_entry;
                    while((fd_entry = readdir(fd_dir)) != NULL)
                    {
                        if(fd_entry->d_name[0] == '.')
                            continue; // skip . and .. entries
                        char fd_path[256] = "/proc/";
                        char fd_content[1024];
                        strcat(fd_path, entry->d_name);
                        strcat(fd_path, "/fd/");
                        strcat(fd_path, fd_entry->d_name);
                        
                        if(access(fd_path, R_OK) != 0)
                        {
                            printf("Failed to access fd path: %s\n", fd_path);
                            continue;
                        }
                        ssize_t len = readlink(fd_path, fd_content, sizeof(fd_content));
                        if(len != -1)
                        {
                            fd_content[len] = '\0';
                            if(strcmp(fd_content, filename) == 0)
                            {
                                printf("Found! PID: [%d] opened %s\n", pid, filename);
                            }
                        }
                        else
                        {
                            perror("readlink");
                            printf("Failed to read link for fd %s of process %d\n", fd_path, pid);
                        }
                    }
                }
                closedir(fd_dir);
            }
            else
            {
                printf("Failed to access path: %s\n", path);
                free(path);
            }
        }
    }
    closedir(dir);
    return EXIT_SUCCESS;
}