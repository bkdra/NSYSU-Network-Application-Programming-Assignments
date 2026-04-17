#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int opt;
    int showlines = 10; // default number of lines to show

    // parse command line options
    while((opt = getopt(argc, argv, "n:")) != -1) {
        switch(opt) {
            case 'n': // n option to specify number of lines to show
                showlines = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-n] filename\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "Expected filename after options\n");
        exit(EXIT_FAILURE);
    }
    char *filename = argv[optind]; // filename is the last argument after options
    int fp = open(filename, O_RDONLY);
    if (fp == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    posix_fadvise(fp, 0, 0, POSIX_FADV_RANDOM); // advise the kernel that we will be reading the file in a random access pattern
    
    // start to read the file from the end and find the last showlines lines
    char *output = malloc(1);
    int outputlen = 0;
    int count = 0;
    int isReadEnd = 0;
    int endPos=-1;
    off_t file_size = lseek(fp, 0, SEEK_END); // get the size of the file and start reading from the end
    off_t pos = file_size - 1;
    
    // check whether the file is smaller than BUF_SIZE, if it is, we can read the whole file at once, otherwise we need to read it in chunks of BUF_SIZE
    if(pos < BUF_SIZE) {
        lseek(fp, 0, SEEK_SET);
        isReadEnd = 1;
    } else {
        lseek(fp, -BUF_SIZE, SEEK_CUR);
    }

    while(pos >= 0 && count < showlines) {
        char buffer[BUF_SIZE];
        ssize_t bytes_read;
        int readThisTime = 0;
        
        // if we have reached the beginning of the file, we need to read the remaining part of the file, otherwise we can read a full chunk of BUF_SIZE
        if(!isReadEnd) 
        {
            bytes_read = read(fp, buffer, BUF_SIZE);
        }
        else
        {
            bytes_read = read(fp, buffer, pos);
        }

        // check how many lines we have read in this chunk
        for (int i = bytes_read  - 1; i >= 0; i--) {
            if (buffer[i] == '\n') {
                count++;
            }
            if (count == showlines) {
                endPos = i;
                break;
            }
            readThisTime++;
        }

        // build the output string by concatenating the current chunk with the previous output, we need to be careful about the order of concatenation, since we are reading the file from the end, we need to concatenate the current chunk before the previous output
        char *temp = malloc(outputlen + readThisTime + 1);
        memcpy(temp+readThisTime, output, outputlen);
        if(endPos == -1) 
        {
            memcpy(temp, buffer, readThisTime);
        }
        else
        {
            memcpy(temp, buffer+endPos+1, bytes_read-endPos-1);
        }
        temp[outputlen + readThisTime + 1] = '\0';
        free(output);
        output = temp;
        outputlen+= readThisTime;

        // update the position of the file pointer
        // if we have reached the beginning of the file, we need to start next reading from the beginning of the file
        // otherwise we can just move the file pointer back by BUF_SIZE to read the next chunk
        pos -= BUF_SIZE;
        if(pos - BUF_SIZE < 0) {
            lseek(fp, 0, SEEK_SET);
            isReadEnd = 1;
        } else {
            lseek(fp, -2*BUF_SIZE, SEEK_CUR); // move twice the BUF_SIZE is because we have read one chunk of BUF_SIZE this time, so we need to move back by another BUF_SIZE to read the next chunk
        }
    }
    printf("%s", output);
    close(fp);
    return 0;
}