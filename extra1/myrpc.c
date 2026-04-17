#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>

#define BUF_SIZE 1024
#define DELIMITERS " \t\n"

int main(int argc, char* argv[])
{
	while(1)
	{
		
		printf("Commander, please input command: ");
		fflush(stdout);
		
		char command[BUF_SIZE + 1];
		if(fgets(command, sizeof(command), stdin) == NULL)
		{
			break;
		}
		command[strcspn(command, "\n")] = '\0';

		char *fileName = NULL;
		int fd = -1;
		if(argc > 1 &&strcmp(argv[1], "-f") == 0)
		{
			fileName = argv[2];
			fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC, 0777);
		}
		

		int to_pipe[2];
		int back_pipe[2];
		
		if(pipe(to_pipe) == -1)
		{
			perror("pipe");
			exit(1);
		}	
		if(pipe(back_pipe) == -1)
		{
			perror("pipe");
			exit(1);
		}
		
		switch(fork())
		{
			case -1:
				perror("fork");
				exit(1);
				break;
			
			case 0: // executor
				char executor_buf[BUF_SIZE];
				close(to_pipe[1]);
				close(back_pipe[0]);
				
				ssize_t exec_len = read(to_pipe[0], executor_buf, BUF_SIZE - 1);
				if(exec_len <= 0)
				{
					close(to_pipe[0]);
					close(back_pipe[1]);
					_exit(0);
				}
				executor_buf[exec_len] = '\0';
				close(to_pipe[0]);
				
				pid_t pid;
				switch(pid = fork())
				{
					case -1:
						perror("fork");
						exit(1);
						break;
					case 0:
						char* read_argv[100];
						int index = 0;
						
						char* token = strtok(executor_buf, DELIMITERS);
						while(token)
						{	
							read_argv[index++] = token;
							token = strtok((char*)NULL, DELIMITERS);
						}
						
						read_argv[index] = NULL;

						if(back_pipe[1] != 1)
						{
							dup2(back_pipe[1], 1);
							close(back_pipe[1]);
						}
						execvp(read_argv[0], read_argv);
						perror("execvp");
						exit(1);
						break;
					default:
						if(waitpid(pid, (int *)NULL, 0) == -1)
						{
							perror("waitpid");
							exit(1);
						}
						break;
				}
				close(back_pipe[1]);
				_exit(0);
			
			default: // commander
				close(to_pipe[0]);
				close(back_pipe[1]);
				
				write(to_pipe[1], command, strlen(command) + 1);
				close(to_pipe[1]);
				int n;
				char commander_buf[BUF_SIZE];
				while((n = read(back_pipe[0], commander_buf, BUF_SIZE)) > 0)
				{
					if(fd == -1)
					{
						if (write(STDOUT_FILENO, commander_buf, n) != n) 
						{
							perror("write");
						}
					}
					else
					{
						if (write(fd, commander_buf, n) != n) 
						{
							perror("write");
						}
					}
					
				}
				if(n==-1)
				{
					perror("read");
				}
				wait((int *)NULL);
		}	
	}
}