#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAXLENGTH 2048

void childProcess(int *);

void makeBaseString(char *);
void appendString(char *);

int main() {
    int fd[2];
    pipe(fd);

    char parentPart [] = "parentPart";
    write(fd[1], parentPart, MAXLENGTH);
    printf("Записали в pipe из родителя %s\n", parentPart);


    if (fork() > 0) {
        childProcess(fd);
    } else  {

        char result[MAXLENGTH];
        read(fd[0], result, MAXLENGTH);
        printf("Прочитали из пайпа в родителе %s\n",result);

        close(fd[0]);
        close(fd[1]);
    }
}

void childProcess(int *fd) {
    char childPart [] = "childPart";
    char parentPart[MAXLENGTH];
    read(fd[0], parentPart, MAXLENGTH);
    printf("Прочитали из канала в потомке %s\n", parentPart);

    char* result = strcat(parentPart, childPart);

    write(fd[1], result, MAXLENGTH);
    printf("Записали в pipe в потомке %s\n", result);

    close(fd[0]);
    close(fd[1]);
}






