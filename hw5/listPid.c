#include<stdio.h>
#include<stdlib.h>
#include<pwd.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>

int userIdFromName(const char *name)
{
    struct passwd *pwd;
    uid_t u;
    char* endptr;

    if(name == NULL || *name == '\0')
        return -1;
    
    u = strtol(name, &endptr, 10);
    if(*endptr == '\0')
        return u;
    
    pwd = getpwnam(name);
    if(pwd == NULL)
        return -1;
    
    return pwd->pw_uid;
}

int main(int argc, char *argv[])
{
    char *name;
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <username or uid>\n", argv[0]);
        return EXIT_FAILURE;
    }
    name = argv[1];
    int uid = userIdFromName(name);
    if(uid == -1)
    {
        fprintf(stderr, "Error: Invalid username or uid '%s'\n", name);
        return EXIT_FAILURE;
    }

    DIR *dir;
    struct dirent *entry;
    char* path;
    dir = opendir("/proc");
    if(dir == NULL)
    {
        perror("opendir");
        return EXIT_FAILURE;
    }

    while((entry = readdir(dir)) != NULL)
    {
        if(entry->d_type == DT_DIR)
        {
            int pid = atoi(entry->d_name);
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
                    char processName[256];
                    while(fgets(line, sizeof(line), fp)) // read one line at a time
                    {
                        if(strncmp(line, "Name:", 5) == 0) // check whether the start of the line is "Name:"
                            sscanf(line, "Name: %s", processName); // extract the process name

                        else if(strncmp(line, "Uid:", 4) == 0) // check whether the start of the line is "Uid:"
                        {
                            int processUid;
                            sscanf(line, "Uid: %d", &processUid); // extract the process uid
                            if(processUid == uid)
                                printf("%d\t%s\n", pid, processName);
                            break;
                        }

                        if(strncmp(line, "Gid:", 4) == 0)
                            break; // stop reading after Gid: line since we only care about Name and Uid
                    }
                    fclose(fp);
                }
            }
        }
    }
    closedir(dir);
    free(path);
}