#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
    
int status_fork;
void go_out(int sig) {
    if(status_fork==0){
        write(1,"Завершили процесс\n",strlen("Завершили процесс\n"));
        exit(0);
    }
    else{
        kill(status_fork,SIGTERM);
        status_fork=0;
    }
}

int main(int argc, char* argv[]){
    char bufer[256]={};
    char* pointer;
    int status_read;
    (void)signal(SIGINT,go_out);
    while(1){
        fgets(bufer,256,stdin);//читаем команду
        pointer=strtok(bufer," \n");
        status_read=1;
        while(pointer!=NULL){//загружаем параметры команды в argv[]
            argv[status_read]=pointer;
            status_read++;
            pointer=strtok(NULL," \n");
        }
        argc=status_read;//фиксируем кол-во аршументов, которое было введено

        if(argc==2)
            argv[argc]="";
        if(strcmp(argv[1],"ls")==0){
            status_fork=fork();
            if(status_fork==0){//дочерний процесс
                //pause();Для проверки работы ctrl+c
                if(argv[2]!="")
                    execlp("/bin/ls",argv[1],argv[2],(char*)NULL);
                else 
                    execlp("/bin/ls",argv[1],(char*)NULL);
                exit(0);
            }
        }
        else if(strcmp(argv[1],"cat")==0){
            status_fork=fork();
            if(status_fork==0){//дочерний процесс
                if(argv[2]=="")
                    write(1,"Нет файлов!\n",strlen("Нет файлов!\n"));
                else{
                    char param[128]={};
                    memset(param,*argv[0],strlen(argv[0]));
                    for(int i=1;i<argc;i++)
                        argv[i-1]=argv[i];
                    argv[argc-1]=NULL;
                    execvp("cat",argv);
                    argv[0]=param;
                }
                exit(0);
            }
        }
        else{
            write(1,"Такой команды нет\n",strlen("Такой команды нет\n"));
        }

        memset(bufer,0,strlen(bufer));//очищаем команду
        for(int i=1;i<argc;i++)
            memset(argv[i],0,strlen(argv[i]));
    }    
    return 0;
}
