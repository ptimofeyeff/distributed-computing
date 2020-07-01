#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>

#define MAXLENGTH 100

int main()
{
    // We use two pipes
    // First pipe to send input string from parent
    // Second pipe to send concatenated string from child

    int fd1[2];  // Used to store two ends of first pipe
    //int fd2[2];  // Used to store two ends of second pipe

    char fixed_str[] = "forgeeks.org";
    char input_str[MAXLENGTH];
    pid_t p;

    pipe(fd1);
    //pipe(fd2);

    scanf("%s", input_str);

    p = fork();


    // Parent process
    if (p > 0)
    {
        char concat_str[MAXLENGTH];

        //close(fd1[0]);  // Close reading end of first pipe

        // Write input string and close writing end of first
        // pipe.
        write(fd1[1], input_str, strlen(input_str)+1);
        close(fd1[1]);

        // Wait for child to send a string
        wait(NULL);

        //close(fd2[1]); // Close writing end of second pipe

        // Read string from child, print it and close
        // reading end.
        read(fd1[0], concat_str, MAXLENGTH);
        printf("Concatenated string %s\n", concat_str);
        //close(fd2[0]);
        close(fd1[0]);
    }

        // child process
    else
    {
        //close(fd1[1]);  // Close writing end of first pipe

        // Read a string using first pipe
        char concat_str[MAXLENGTH];
        read(fd1[0], concat_str, MAXLENGTH);


        // Close both reading ends
        close(fd1[0]);
        //close(fd2[0]);

        // Write concatenated string and close writing end
        write(fd1[1], strcat(concat_str, fixed_str), MAXLENGTH);
        //close(fd2[1]);

        close(fd1[1]);//my

        exit(0);
    }
}