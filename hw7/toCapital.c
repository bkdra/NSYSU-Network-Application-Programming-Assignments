#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define BUF_SIZE 1024

int main()
{
    int raw_pd[2];
    int processed_pd[2];
    if(pipe(raw_pd) == -1)
    {
        printf("Pipe(raw) failed\n");
        return 1;
    }
    if(pipe(processed_pd) == -1)
    {
        printf("Pipe(processed) failed\n");
        return 1;
    }

    switch(fork())
    {
        case -1:
            printf("Fork failed\n");
            return 1;
        case 0:
            close(raw_pd[1]);
            close(processed_pd[0]);

            char buffer[BUF_SIZE];
            int n;
            while((n = read(raw_pd[0], buffer, BUF_SIZE)) > 0)
            {
                for(int i = 0; buffer[i]; i++)
                {
                    if(buffer[i] >= 'a' && buffer[i] <= 'z')
                    {
                        buffer[i] -= ('a' - 'A');
                    }
                }
                write(processed_pd[1], buffer, strlen(buffer) + 1);
            }
            if(n < 0)
            {
                printf("Read failed\n");
            }
            close(raw_pd[0]);
            close(processed_pd[1]);
            break;
        default:
            close(raw_pd[0]);
            close(processed_pd[1]);

            while(1)
            {
                char input[BUF_SIZE];
                printf("Enter a string: ");
                fgets(input, BUF_SIZE, stdin);
                input[strcspn(input, "\n")] = 0; // Remove newline character
                write(raw_pd[1], input, strlen(input) + 1);

                char output[BUF_SIZE];
                read(processed_pd[0], output, BUF_SIZE);
                printf("Processed string: %s\n", output);
            }
            
            close(raw_pd[1]);
            close(processed_pd[0]);
            break;
    }
}